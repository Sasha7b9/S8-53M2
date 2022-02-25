// 2022/02/18 15:30:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Hardware/HAL/HAL.h"
#include "Data/Storage.h"
#include "Utils/Math.h"
#include "Data/Processing.h"
#include "Utils/Containers/Buffer.h"
#include "Utils/Strings.h"
#include <stm32f4xx_hal.h>
#include <string.h>


#define WRITE_AND_OR_INVERSE(addr, data, ch)                                                                        \
    if(SET_INVERSE(ch))                                                                                             \
    {                                                                                                               \
        data = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(data, ValueFPGA::MIN, ValueFPGA::MAX));  \
    }                                                                                                               \
    *addr = data;


#define CLC_HI      Pin::SPI4_CLK.Set();
#define CLC_LOW     Pin::SPI4_CLK.Reset();
#define DATA_SET(x) Pin::SPI4_DAT.Write(x);

namespace FPGA
{
    uint timeStart = 0;

    DataSettings ds;

    Buffer<uint8> dataReadA;            // Буфер используется для чтения данных первого канала.
    Buffer<uint8> dataReadB;            // Буфер используется для чтения данных второго канала.

    int addition_shift = 0;

    volatile static int numberMeasuresForGates = 1000;

    uint16 ReadFlag();

    void ReadPoint();

    bool ProcessingData();

    // Прочитать данные.
    // necessaryShift - Признак того, что сигнал нужно смещать.
    // saveToStorage - Нужно в режиме рандомизатора для указания, что пора сохранять измерение
    void DataRead(bool necessaryShift, bool saveToStorage);

    void ReadRandomizeMode();

    void ReadRealMode(bool necessaryShift);

    // Инвертирует данные.
    void InverseDataIsNecessary(Chan::E, Buffer<uint8> &data);

    int CalculateShift();

    bool CalculateGate(uint16 rand, uint16 *min, uint16 *max);

    // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
    void LoadSettings();

    // Эта функция вызывается по таймеру, когда можно считывать новыый сигнал
    void OnTimerCanReadData();

    namespace AutoFinder
    {
        void AutoFind();
    }

    namespace FreqMeter
    {
        void Update(uint16 flag);
    }

    namespace Reader
    {
        // Возвращает адрес, с которого нужно читать первую точку
        extern uint16 CalculateAddressRead();
    }
}



void FPGA::Init()
{
    ClearData();
    Storage::Clear();
    FPGA::LoadSettings();
    FPGA::SetNumSignalsInSec(ENumSignalsInSec::ToNum(ENUM_SIGNALS_IN_SEC));
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
}


void FPGA::Update()
{
    ReadFlag();

    if (gStateFPGA.needCalibration)              // Если вошли в режим калибровки -
    {
        FPGA::ProcedureCalibration();            // выполняем её.
        gStateFPGA.needCalibration = false;
    }

    if (FPGA_TEMPORARY_PAUSE)
    {
        return;
    }

    if (AUTO_FIND_IN_PROGRESS)
    {
        AutoFinder::AutoFind();
        return;
    }
    
    if(FPGA_CAN_READ_DATA == 0)
    {
        return;
    }

    if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop)
    {
        return;
    }

    if (SET_SELFRECORDER)
    {
        ReadPoint();
    }

    ProcessingData();

    FPGA_CAN_READ_DATA = 0;
}


bool FPGA::ProcessingData()
{
    bool retValue = false;

    int num = (TBase::InRandomizeMode() && (!START_MODE_IS_SINGLE) && SAMPLE_TYPE_IS_EQUAL) ? TBase::StretchRand() : 1;

    for (int i = 0; i < num; i++)
    {
        uint16 flag = ReadFlag();

        if (FPGA_CRITICAL_SITUATION)
        {
            if (TIME_MS - timeStart > 500)
            {
                SwitchingTrig();
                TRIG_AUTO_FIND = 1;
                FPGA_CRITICAL_SITUATION = 0;
            }
            else if (_GET_BIT(flag, FL_TRIG))
            {
                FPGA_CRITICAL_SITUATION = 0;
            }
        }
        else if (_GET_BIT(flag, FL_DATA))
        {
            if (set.debug.showRegisters.flag)
            {
                char buffer[9];
                LOG_WRITE("флаг готовности %s", Bin2String(flag, buffer));
            }

            Panel::EnableLEDTrig(true);

            FPGA::Stop(true);

            DataRead(_GET_BIT(flag, FL_LAST_RECOR), (num == 1) || (i == num - 1));

            retValue = true;

            if (!START_MODE_IS_SINGLE)
            {
                Start();
                StateWorkFPGA::SetCurrent(StateWorkFPGA::Work);
            }
            else
            {
                FPGA::Stop(false);
            }
        }
        else
        {
            if (flag & (1 << 2))
            {
                if (START_MODE_IS_AUTO)
                {
                    FPGA_CRITICAL_SITUATION = 1;
                }
                timeStart = TIME_MS;
            }
        }

        Panel::EnableLEDTrig(_GET_BIT(flag, FL_TRIG) ? true : false);
    }

    return retValue;
}


void FPGA::SwitchingTrig()
{
    static const uint16 value[2][2] =
    {
        {1, 0},
        {0, 1}
    };

    HAL_FMC::Write(WR_TRIG, value[TRIG_POLARITY][0]);
    HAL_FMC::Write(WR_TRIG, value[TRIG_POLARITY][1]);
}


void FPGA::Start()
{
    if (!TBase::InRandomizeMode())
    {
        ClearData();
    }

    if (SET_TBASE >= TBase::MIN_P2P)
    {
        Display::ResetP2Ppoints(false);
        Timer::Enable(TypeTimer::P2P, 1, ReadPoint);
    }
    else
    {
        Timer::Disable(TypeTimer::P2P);
        Display::ResetP2Ppoints(true);
    }

    HAL_FMC::Write(WR_PRED, FPGA::Launch::PredForWrite());
    HAL_FMC::Write(WR_START, 1);
    ds.FillDataPointer();
    timeStart = TIME_MS;
    StateWorkFPGA::SetCurrent(StateWorkFPGA::Wait);

    FPGA_CRITICAL_SITUATION = 0;
}


bool FPGA::IsRunning()
{
    return (StateWorkFPGA::GetCurrent() != StateWorkFPGA::Stop);
}


void FPGA::Stop(bool pause)
{
    Timer::Disable(TypeTimer::P2P);
    HAL_FMC::Write(WR_STOP, 1);
    StateWorkFPGA::SetCurrent(pause ? StateWorkFPGA::Pause : StateWorkFPGA::Stop);
}


void FPGA::OnPressStartStop()
{
    if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop)
    {
        FPGA::Start();
    }
    else
    {
        FPGA::Stop(false);
    }
}


void FPGA::SetNumSignalsInSec(int numSigInSec)
{
    Timer::Enable(TypeTimer::NumSignalsInSec, 1000.f / numSigInSec, OnTimerCanReadData);
}


void FPGA::OnTimerCanReadData()
{
    FPGA_CAN_READ_DATA = 1;
}


void FPGA::FindAndSetTrigLevel()
{

}


void FPGA::TemporaryPause()
{

}


void FPGA::SetNumberMeasuresForGates(int number)
{
    numberMeasuresForGates = number;
}


void BUS_FPGA::WriteDAC(TypeWriteDAC::E type, uint16 data)
{
    Pin::SPI4_CS1.Reset();

    for (int i = 15; i >= 0; i--)
    {
        DATA_SET((data & (1 << i)) ? 1 : 0);
        CLC_HI
        CLC_LOW
    }

    Pin::SPI4_CS1.Set();
}


void BUS_FPGA::WriteAnalog(TypeWriteAnalog::E type, uint data)
{
    Pin::SPI4_CS2.Reset();

    for (int i = 23; i >= 0; i--)
    {
        DATA_SET((data & (1 << i)) ? 1 : 0);
        CLC_HI
        CLC_LOW
    }

    Pin::SPI4_CS2.Set();
}


void FPGA::DataRead(bool necessaryShift, bool saveToStorage)
{
    Panel::EnableLEDTrig(false);

    FPGA_IN_PROCESS_READ = 1;

    if (TBase::InRandomizeMode())
    {
        ReadRandomizeMode();
    }
    else
    {
        ReadRealMode(necessaryShift);
    }

    static uint prevTime = 0;

    if (saveToStorage || (TIME_MS - prevTime > 500))
    {
        prevTime = TIME_MS;

        if (!TBase::InRandomizeMode())
        {
            InverseDataIsNecessary(Chan::A, dataReadA);
            InverseDataIsNecessary(Chan::B, dataReadB);
        }

        Storage::AddData(dataReadA.Data(), dataReadB.Data(), ds);

        if (TRIG_MODE_FIND_IS_AUTO && TRIG_AUTO_FIND)
        {
            FPGA::FindAndSetTrigLevel();

            TRIG_AUTO_FIND = 0;
        }
    }

    FPGA_IN_PROCESS_READ = 0;
}


void FPGA::ReadRandomizeMode()
{
    int Tsm = CalculateShift();
    if (Tsm == TShift::EMPTY)
    {
        return;
    };

    int step = TBase::StretchRand();
    int index = Tsm - step - addition_shift;

    if (index < 0)
    {
        index += step;        // WARN
    }

    uint8 *pData0 = dataReadA.Pointer(index);
    const uint8 *const pData0Last = dataReadA.Last() - 1;
    uint8 *pData1 = dataReadB.Pointer(index);
    const uint8 *const pData1Last = dataReadB.Last() - 1;

    const uint8 *const first0 = dataReadA.Data();
    const uint8 *const last0 = pData0Last;
    const uint8 *const first1 = dataReadB.Data();
    const uint8 *const last1 = pData1Last;

    int numAve = NUM_AVE_FOR_RAND;

    if (SettingsDisplay::NumAverages() > numAve)
    {
        numAve = SettingsDisplay::NumAverages();
    }

    if (START_MODE_IS_SINGLE || SAMPLE_TYPE_IS_REAL)
    {
        FPGA::ClearData();
    }

    BitSet16 dataA;
    BitSet16 dataB;

    while (pData0 < dataReadA.Last())
    {
        dataA.half_word = *RD_ADC_A;
        dataB.half_word = *RD_ADC_B;

        if (pData0 >= first0 && pData0 <= last0)
        {
            WRITE_AND_OR_INVERSE(pData0, dataA.byte[0], Chan::A);
        }

        if (pData1 >= first1 && pData1 <= last1)
        {
            WRITE_AND_OR_INVERSE(pData1, dataB.byte[0], Chan::B);
        }

        pData0 += step;
        pData1 += step;

        if (pData0 >= first0 && pData0 <= last0)
        {
            WRITE_AND_OR_INVERSE(pData0, dataA.byte[1], Chan::A);
        }

        if (pData1 >= first1 && pData1 <= last1)
        {
            WRITE_AND_OR_INVERSE(pData1, dataB.byte[1], Chan::B);
        }

        pData0 += step;
        pData1 += step;
    }

    if (START_MODE_IS_SINGLE || SAMPLE_TYPE_IS_REAL)
    {
        Processing::InterpolationSinX_X(dataReadA.Data(), SET_TBASE);
        Processing::InterpolationSinX_X(dataReadB.Data(), SET_TBASE);
    }
}


void FPGA::ReadRealMode(bool necessaryShift)
{
    HAL_FMC::Write(WR_PRED, Reader::CalculateAddressRead());
    HAL_FMC::Write(WR_ADDR_READ, 0xffff);

    uint8 *pA = dataReadA.Data();
    uint8 *pB = dataReadB.Data();
    uint8 *endA = dataReadA.Last();

    if (PEAKDET_IS_ENABLE)
    {
        uint8 *p_minA = pA;
        uint8 *p_maxA = p_minA + ENUM_POINTS_FPGA::ToNumPoints();
        uint8 *p_minB = pB;
        uint8 *p_maxB = p_minB + ENUM_POINTS_FPGA::ToNumPoints();

        BitSet16 data;

        Buffer<uint8> log_min(10);
        Buffer<uint8> log_max(10);

        int index = 0;

        while (p_maxA < endA && FPGA_IN_PROCESS_READ)
        {
            data.half_word = *RD_ADC_A;
            *p_maxA++ = data.byte0;
            *p_minA++ = data.byte1;

            if (index++ < 10)
            {
                log_min[index] = data.byte1;
                log_max[index] = data.byte0;
            }

            data.half_word = *RD_ADC_B;
            *p_maxB++ = data.byte0;
            *p_minB++ = data.byte1;
        }
    }
    else
    {
        BitSet16 data;

        while (pA < endA && FPGA_IN_PROCESS_READ)
        {
            data.half_word = *RD_ADC_B;
            *pB++ = data.byte0;
            *pB++ = data.byte1;

            data.half_word = *RD_ADC_A;
            *pA++ = data.byte0;
            *pA++ = data.byte1;
        }

        int shift = necessaryShift ? -1 : 0;

        if (shift != 0)
        {
            if (shift < 0)
            {
                shift = -shift;

                for (int i = FPGA::MAX_POINTS - shift - 1; i >= 0; i--)
                {
                    dataReadA[i + shift] = dataReadA[i];
                    dataReadB[i + shift] = dataReadB[i];
                }
            }
            else
            {
                for (int i = shift; i < FPGA::MAX_POINTS; i++)
                {
                    dataReadA[i - shift] = dataReadA[i];
                    dataReadB[i - shift] = dataReadB[i];
                }
            }
        }
    }
}


void FPGA::InverseDataIsNecessary(Chan::E ch, Buffer<uint8> &data)
{
    if (SET_INVERSE(ch))
    {
        int size = data.Size();

        for (int i = 0; i < size; i++)
        {
            data[i] = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(data[i], ValueFPGA::MIN, ValueFPGA::MAX));
        }
    }
}


int FPGA::CalculateShift()            // \todo Не забыть восстановить функцию
{
    uint16 rand = HAL_ADC3::GetValue();

    uint16 min = 0;
    uint16 max = 0;

    if (SET_TBASE == TBase::_200ns)
    {
        return rand < 3000 ? 0 : -1;    // set.debug.altShift; \todo Остановились на жёстком задании дополнительного смещения. На PageDebug выбор 
                                        // закомментирован, можно раскомментировать при необходимости
    }

    if (!CalculateGate(rand, &min, &max))
    {
        return TShift::EMPTY;
    }

    if (TBase::InRandomizeMode())
    {
        float tin = (float)(rand - min) / (float)(max - min) * 10e-9f;
        int retValue = (int)(tin / 10e-9f * (float)TBase::StretchRand());
        return retValue;
    }

    if (SET_TBASE == TBase::_100ns && rand < (min + max) / 2)
    {
        return 0;
    }

    return -1;  // set.debug.altShift;      \todo Остановились на жёстком задании дополнительного смещения. На PageDebug выбор закомментирован, 
                                            //можно раскомментировать при необходимости
}


bool FPGA::CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax)
{
    if (FPGA_FIRST_AFTER_WRITE)
    {
        FPGA_FIRST_AFTER_WRITE = 0;
        return false;
    }

    if (rand < 500 || rand > 3500)
    {
        LOG_WRITE("ОШИБКА!!! считано %d", rand);
        return false;
    }

    static const int numValues = 1000;

    static float minGate = 0.0f;
    static float maxGate = 0.0f;
    static int numElements = 0;
    static uint16 min = 0xffff;
    static uint16 max = 0;

    numElements++;
    if (rand < min)
    {
        min = rand;
    }
    if (rand > max)
    {
        max = rand;
    }

    if (minGate == 0)
    {
        *eMin = min;
        *eMax = max;
        if (numElements < numValues)
        {
            return true;
        }
        minGate = min;
        maxGate = max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    if (numElements == numValues)
    {
        minGate = 0.9f * minGate + 0.1f * min;
        maxGate = 0.9f * maxGate + 0.1f * max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    *eMin = (uint16)minGate; //-V519
    *eMax = (uint16)maxGate; //-V519

    return true;
}


uint16 FPGA::ReadFlag()
{
    uint16 flag = HAL_FMC::Read(RD_FL);

    FreqMeter::Update(flag);

    return flag;
}


void FPGA::ClearData()
{
    int num_bytes = ENUM_POINTS_FPGA::ToNumBytes();

    dataReadA.Realloc(num_bytes);
    dataReadB.Realloc(num_bytes);

    dataReadA.Fill(0);
    dataReadB.Fill(0);
}


void FPGA::ReadPoint()
{
    if (_GET_BIT(ReadFlag(), FL_POINT))
    {
        BitSet16 dataA;
        BitSet16 dataB;

        dataB.half_word = *RD_ADC_B;
        dataA.half_word = *RD_ADC_A;

        Display::AddPoints(dataA.byte0, dataA.byte1, dataB.byte0, dataB.byte1);
    }
}


void BUS_FPGA::WriteToHardware(uint8 *, uint8, bool)
{

}


void BUS_FPGA::Write(uint16 *address, uint16 data, bool restart)
{
    bool is_running = FPGA::IsRunning();

    if (restart)
    {
        FPGA::Stop(false);
    }

    HAL_FMC::Write(address, data);

    if (is_running && restart)
    {
        FPGA::Start();
    }
}
