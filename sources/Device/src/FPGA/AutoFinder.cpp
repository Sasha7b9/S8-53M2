// 2022/02/21 12:24:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Data/Storage.h"
#include "Hardware/Timer.h"
#include <stm32f4xx.h>
#include <cstring>


namespace FPGA
{
    namespace AutoFinder
    {
        struct StrForAutoFind
        {
            uint startTime;
            Settings settings;
        } structAF;

        static bool FindWave(Chan);

        static Range::E FindRange(Chan);

        // Читать данные с ожиданием импульса синхронизации
        static bool ReadDataWithSynchronization(Chan, uint timeWait, Buffer<uint8> &);

        // Возвращает размах сигнала - разность между минимальным и максимальным значениями
        static int GetBound(uint8 data[1024], int *min, int *max);

        static bool FindParams(TBase::E *);

        static void FuncDrawAutoFind();

        static TBase::E CalculateTBase(float freq);
    }

    namespace Reader
    {
        // Чтение двух байт канала 1 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadA();

        // Чтение двух байт канала 2 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadB();
    }
}


void FPGA::AutoFinder::FindSignal()
{
    /*
        Алгоритм поиска.
        1. Устанавливаем закрытый вход по каналу, синхронизация - ПС.
        2. Устанавливаем развёртку 20мс/дел, растяжку - 2мВ/дел.
        3. Запускаем АЦП.
        4. Ждём синхронизацию. Если синхронизации нет - переходим на второй канал. Если есть:
        5. Включаем пиковый детектр и считываем данные. Если считанные данные занимают по высоте меньше 2 клеток -
            переходим к поиску по второму каналу. Если занимают больше 2 клеток:
        6. Устанавливаем вертикальный масштаб 5мВ, 10мВ, 20мВ и более, пока считанный сигнал полностью не впишется в
            экран.
        7. Переходим к поиску TBase.
    */

    /** \todo Оптимизировать алгоритм в плане скорости.\n
        1. Сначала находим время между флагами снихронизации.\n
        2. Затем устанавливаем такую развёртку, чтобы на длину памяти приходилось не более двух импульсов синхронизации.\n
        3. Это позволит быстрее считывать и обрабатывать данные.
    */

    Settings old = set;

    if (!FindWave(ChA))
    {
        if (!FindWave(ChB))
        {
            Display::ShowWarningBad(Warning::SignalNotFound);
            set = old;
            set.RunAfterLoad();
            FPGA::Init();
        }
    }

    FPGA::Start();
}


static bool FPGA::AutoFinder::FindWave(Chan ch)
{
    TBase::Set((TBase::E)(TBase::MIN_P2P - 1));
    SET_ENABLED(ch) = true;
    TrigSource::Set(ch);
    TrigLev::Set(ch, TrigLev::ZERO);
    RShift::Set(ch, RShift::ZERO);
    ModeCouple::Set(ch, ModeCouple::AC);
    TrigInput::Set(TrigInput::Full);

    Range::E range = FindRange(ch);

    if (range == Range::Count)
    {
        return false;
    }

    Range::Set(ch, range);

    TBase::E tbase = TBase::Count;

    FindParams(&tbase);

    return (tbase != TBase::Count);
}


static Range::E FPGA::AutoFinder::FindRange(Chan ch)
{
    PeackDetMode::E peackDet = SET_PEAKDET;
    TPos::E tPos = SET_TPOS;
    Range::E oldRange = SET_RANGE(ch);

    START_MODE = StartMode::Wait;                // Устанавливаем ждущий режим синхронизации, чтоб понять, есть ли сигнал

    Stop(false);

    PeackDetMode::Set(PeackDetMode::Enable);
    Range::Set(ch, Range::_2mV);
    TPos::Set(TPos::Left);

    int range = Range::Count;

    Buffer<uint8> data(1024);

    if (ReadDataWithSynchronization(ch, 2000, data))        // Если в течение 2 секунд не считан сигнал, то его нет на этом канале - выходим
    {
        int min = 0;
        int max = 0;

        int bound = GetBound(data.Data(), &min, &max);

        if (bound > (ValueFPGA::MAX - ValueFPGA::MIN) / 10.0 * 2)   // Если размах сигнала меньше двух клеток - тоже выходим
        {
            StartMode::Set(StartMode::Auto);

            for (range = 0; range < Range::Count; ++range)
            {
                Range::Set(ch, (Range::E)range);

                ReadDataWithSynchronization(ch, 10000, data);

                GetBound(data.Data(), &min, &max);

                if (min > ValueFPGA::MIN && max < ValueFPGA::MAX)       // Если все значения внутри экрана
                {
                    break;                                              // То мы нашли наш Range - выходим из цикла
                }
            }
        }
    }

    Range::Set(ch, oldRange);
    PeackDetMode::Set(peackDet);
    TPos::Set(tPos);

    return (Range::E)range;
}


static bool FPGA::AutoFinder::ReadDataWithSynchronization(Chan ch, uint time_wait, Buffer<uint8> &data)
{
    data.Fill(ValueFPGA::NONE);

    HAL_FMC::Write(WR_PRED, (uint16)(~(2)));
    HAL_FMC::Write(WR_POST, (uint16)(~(data.Size() + 20)));
    HAL_FMC::Write(WR_START, 1);

    while (_GET_BIT(flag.Read(), FL_PRED) == 0) { }

    Waiter waiter;

    while(_GET_BIT(flag.Read(), FL_TRIG) == 0)
    {
        if (waiter.ElapsedTime() > time_wait)
        {
            FPGA::Stop(false);

            return false;
        }
    }

    while(_GET_BIT(flag.Read(), FL_DATA) == 0) { }

    uint16 address = (uint16)(HAL_FMC::Read(RD_ADDR_LAST_RECORD) - data.Size() - 2);
    HAL_FMC::Write(WR_PRED, address);
    HAL_FMC::Write(WR_ADDR_READ, 0xffff);

    typedef BitSet16 (*pFuncRead)();

    pFuncRead funcRead = ch.IsA() ? Reader::ReadA : Reader::ReadB;

    uint8 *elem = data.Data();
    uint8 *last = data.Last();

    funcRead();

    if (SET_TBASE <= TBase::MAX_RAND)
    {
        while (elem < last)
        {
            BitSet16 bytes = funcRead();

            *elem++ = bytes.byte0;
        }
    }
    else
    {
        while (elem < last)
        {
            BitSet16 bytes = funcRead();

            *elem++ = bytes.byte0;
            *elem++ = bytes.byte1;
        }
    }

    return true;
}


static int FPGA::AutoFinder::GetBound(uint8 data[1024], int *_min, int *_max)
{
    int min = 255;
    int max = 0;

    for (int i = 0; i < 512; i++)
    {
        SET_MIN_IF_LESS(data[i], min);
        SET_MAX_IF_LARGER(data[i], max);
    }

    *_min = min;
    *_max = max;

    return max - min;
}


bool FPGA::AutoFinder::FindParams(TBase::E *tbase)
{
    TrigInput::Set(TrigInput::Full);

    Start();

    while (_GET_BIT(FPGA::flag.Read(), FL_FREQ) == 0)
    {
        FuncDrawAutoFind();
    };

    Stop(false);
    float freq = FreqMeter::GetFreq();

    TrigInput::Set(freq < 1e6f ? TrigInput::LPF : TrigInput::Full);

    Start();

    while (_GET_BIT(FPGA::flag.Read(), FL_FREQ) == 0)
    {
    };

    Stop(false);
    freq = FreqMeter::GetFreq();

    if (freq >= 50.0f)
    {
        *tbase = CalculateTBase(freq);

        if (*tbase >= TBase::MIN_P2P)
        {
            *tbase = TBase::MIN_P2P;
        }

        TBase::Set(*tbase);

        Start();

        TrigInput::Set(freq < 500e3f ? TrigInput::LPF : TrigInput::HPF);

        return true;
    }
    else
    {
        TrigInput::Set(TrigInput::LPF);
        freq = FreqMeter::GetFreq();

        if (freq > 0.0f)
        {
            *tbase = CalculateTBase(freq);

            if (*tbase >= TBase::MIN_P2P)
            {
                *tbase = TBase::MIN_P2P;
            }

            TBase::Set(*tbase);
            Timer::PauseOnTime(10);
            Start();
            return true;
        }
    }

    return false;
}


static void FPGA::AutoFinder::FuncDrawAutoFind()
{
    int width = 220;
    int height = 60;
    int x = 160 - width / 2;
    int y = 120 - height / 2;
    Painter::BeginScene(COLOR_BACK);
    Painter::FillRegion(x, y, width, height, COLOR_BACK);
    Painter::DrawRectangle(x, y, width, height, COLOR_FILL);
    PText::DrawStringInCenterRect(x, y, width, height - 20, "Идёт поиск сигнала. Подождите");

    char buffer[101] = "";
    uint progress = ((HAL_GetTick() - structAF.startTime) / 20) % 80;

    for (uint i = 0; i < progress; i++)
    {
        std::strcat(buffer, ".");
    }

    PText::DrawStringInCenterRect(x, y + (height - 30), width, 20, buffer);

    Display::DrawConsole();

    Painter::EndScene();
}


static TBase::E FPGA::AutoFinder::CalculateTBase(float freq)
{
    struct STR
    {
        float    freq;
        TBase::E tbase;
    };

    static const STR structs[TBase::Count] =
    {
        {100e6f,    TBase::_2ns  },
        {40e6f,     TBase::_5ns  },
        {20e6f,     TBase::_10ns },
        {10e6f,     TBase::_20ns },
        {3e6f,      TBase::_50ns },
        {2e6f,      TBase::_100ns},
        {900e3f,    TBase::_200ns},
        {200e3f,    TBase::_1us  },
        {400e3f,    TBase::_500ns},
        {90e3f,     TBase::_2us  },
        {30e3f,     TBase::_5us  },
        {20e3f,     TBase::_10us },
        {10e3f,     TBase::_20us },
        {4e3f,      TBase::_50us },
        {2e3f,      TBase::_100us},
        {1e3f,      TBase::_200us},
        {350.0f,    TBase::_500us},
        {200.0f,    TBase::_1ms  },
        {100.0f,    TBase::_2ms  },
        {40.0f,     TBase::_5ms  },
        {20.0f,     TBase::_10ms },
        {10.0f,     TBase::_20ms },
        {4.0f,      TBase::_50ms },
        {2.0f,      TBase::_100ms},
        {0.0f,      TBase::Count }
    };


    const STR *str = &structs[0];

    while (str->freq != 0.0f)
    {
        if (freq >= str->freq)
        {
            return str->tbase;
        }
        ++str;
    }

    return TBase::_200ms;
}
