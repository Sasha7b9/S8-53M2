// 2022/02/21 12:24:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"


namespace FPGA
{
    namespace AutoFinder
    {
        static bool FindWave(Chan);

        static Range::E FindRange(Chan);

        static bool AccurateFindParams(Chan);

        static bool ReadingCycle(uint timeWait);
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

    return AccurateFindParams(ch);
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

    ReadingCycle(1000);

    if (ReadingCycle(2000))                                 // Если в течение 2 секунд не считан сигнал, то его нет на этом канале - выходим
    {
        uint8 min = 0;
        uint8 max = 0;

        uint8 bound = GetBound(Storage::GetData_RAM(ch, 0), &min, &max);

        if (bound > (MAX_VALUE - MIN_VALUE) / 10.0 * 2)      // Если размах сигнала меньше двух клеток - тоже выходим
        {
            START_MODE = StartMode_Auto;

            for (range = 0; range < RangeSize; ++range)
            {
                /// \todo Этот алгоритм возвращает результат "Сигнал не найден", если при (range == RangeSize - 1) сигнал выходит за пределы экрана

                SetRange(ch, (Range)range);

                ReadingCycle(10000);

                GetBound(Storage::GetData_RAM(ch, 0), &min, &max);

                if (min > MIN_VALUE && max < MAX_VALUE)     // Если все значения внутри экрана
                {
                    break;                                  // То мы нашли наш Range - выходим из цикла
                }
            }
        }
    }

    SetRange(ch, oldRange);
    SetPeackDetMode(peackDet);
    SetTPos(tPos);

    return (Range)range;
}


static bool FPGA::AutoFinder::AccurateFindParams(Chan ch)
{
    TBase tBase = TBaseSize;
    FindParams(ch, &tBase);

    return true;
}


bool FPGA::AutoFinder::ReadingCycle(uint timeWait)
{
    Start();
    uint timeStart = HAL_GetTick();
    while (!ProcessingData())
    {
        FuncDrawAutoFind();
        if ((HAL_GetTick() - timeStart) > timeWait)
        {
            Stop(false);

            return false;
        }
    };
    Stop(false);

    return true;
}
