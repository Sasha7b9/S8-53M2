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

    bool IN_PROCESS_READ = false;
    bool TRIG_AUTO_FIND = false;    // Установленное в 1 значение означает, что нужно производить автоматический поиск
                                    // синхронизации, если  выбрана соответствующая настройка.
    bool AUTO_FIND_IN_PROGRESS = false;
    bool TEMPORARY_PAUSE = false;
    bool CAN_READ_DATA = false;
    bool NEED_AUTO_TRIG = false;    // Если true, нужно делать автозапуск (автоматический режим запуска и отсутсвует
                                    // синхроимпульс
    bool FIRST_AFTER_WRITE = false; // Используется в режиме рандомизатора. После записи любого параметра в альтеру
                    // нужно не использовать первое считанное данное с АЦП, потому что оно завышено и портит ворота


    uint16 ReadFlag();

    void ReadPoint();

    void ProcessingData();

    // Прочитать данные.
    void DataRead();

    void ReadPoints();

    // Инвертирует данные.
    void InverseDataIsNecessary(Chan::E, Buffer<uint8> &data);

    // Смещение с АЦП рандомизатора
    int ShiftRandomizerADC();

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

    if (TEMPORARY_PAUSE)
    {
        return;
    }

    if (AUTO_FIND_IN_PROGRESS)
    {
        AutoFinder::AutoFind();
        return;
    }
    
    if(!CAN_READ_DATA)
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

    CAN_READ_DATA = false;
}


void FPGA::ProcessingData()
{
    uint16 flag = ReadFlag();

    if (NEED_AUTO_TRIG)
    {
        if (TIME_MS - timeStart > 500)
        {
            SwitchingTrig();
            TRIG_AUTO_FIND = true;
            NEED_AUTO_TRIG = false;
        }
        else if (_GET_BIT(flag, FL_TRIG))
        {
            NEED_AUTO_TRIG = false;
        }
    }
    else if (_GET_BIT(flag, FL_DATA))
    {
        Panel::EnableLEDTrig(true);

        Stop(true);

        DataRead();

        if (!START_MODE_IS_SINGLE)
        {
            Start();
            StateWorkFPGA::SetCurrent(StateWorkFPGA::Work);
        }
        else
        {
            Stop(false);
        }
    }
    else
    {
        if (flag & (1 << 2))
        {
            if (START_MODE_IS_AUTO)
            {
                NEED_AUTO_TRIG = true;
            }
            timeStart = TIME_MS;
        }
    }

    Panel::EnableLEDTrig(_GET_BIT(flag, FL_TRIG) ? true : false);
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
    if (!TBase::InModeRandomizer())
    {
        ClearData();
    }

    ds.Init();

    if (TBase::InModeP2P())
    {
        Storage::P2P::CreateFrame(ds);
        Timer::Enable(TypeTimer::P2P, 1, ReadPoint);
    }
    else
    {
        Timer::Disable(TypeTimer::P2P);
    }

    HAL_FMC::Write(WR_PRED, FPGA::Launch::PredForWrite());
    HAL_FMC::Write(WR_START, 1);

    timeStart = TIME_MS;
    StateWorkFPGA::SetCurrent(StateWorkFPGA::Wait);

    NEED_AUTO_TRIG = false;
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
    CAN_READ_DATA = true;
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


void FPGA::DataRead()
{
    Panel::EnableLEDTrig(false);

    IN_PROCESS_READ = true;

    ReadPoints();

    if (!TBase::InModeRandomizer())
    {
        InverseDataIsNecessary(Chan::A, dataReadA);
        InverseDataIsNecessary(Chan::B, dataReadB);
    }

    Storage::AddData(ds, dataReadA.Data(), dataReadB.Data());

    if (TRIG_MODE_FIND_IS_AUTO && TRIG_AUTO_FIND)
    {
        FPGA::FindAndSetTrigLevel();

        TRIG_AUTO_FIND = false;
    }

    IN_PROCESS_READ = false;
}


void FPGA::ReadPoints()
{
    HAL_FMC::Write(WR_PRED, Reader::CalculateAddressRead());
    HAL_FMC::Write(WR_ADDR_READ, 0xffff);

    uint8 *a = dataReadA.Data();
    uint8 *b = dataReadB.Data();
    const uint8 *const endA = dataReadA.Last();

    if (SET_PEAKDET_IS_ENABLE)
    {
        uint8 *p_minA = a;
        uint8 *p_maxA = p_minA + ENUM_POINTS_FPGA::ToNumPoints();
        uint8 *p_minB = b;
        uint8 *p_maxB = p_minB + ENUM_POINTS_FPGA::ToNumPoints();

        BitSet16 data;

        while (p_maxA < endA && IN_PROCESS_READ)
        {
            data.half_word = *RD_ADC_A;
            *p_maxA++ = data.byte0;
            *p_minA++ = data.byte1;

            data.half_word = *RD_ADC_B;
            *p_maxB++ = data.byte0;
            *p_minB++ = data.byte1;
        }
    }
    else
    {
        const int shift_rand = ShiftRandomizerADC();
        a += shift_rand;
        b += shift_rand;

        BitSet16 data;

        const int stretch = TBase::StretchRand();

        while (a < endA && IN_PROCESS_READ)
        {
            data.half_word = *RD_ADC_A;
            *a = data.byte0;
            a += stretch;
            *a = data.byte1;
            a += stretch;

            data.half_word = *RD_ADC_B;
            *b = data.byte0;
            b += stretch;
            *b = data.byte1;
            b += stretch;
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


int FPGA::ShiftRandomizerADC()
{
    if (TBase::InModeRandomizer())
    {
        uint16 rand = HAL_ADC1::GetValue();

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

        if (TBase::InModeRandomizer())
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

    return 0;
}


bool FPGA::CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax)
{
    if (FIRST_AFTER_WRITE)
    {
        FIRST_AFTER_WRITE = false;
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

    dataReadA.Fill(ValueFPGA::NONE);
    dataReadB.Fill(ValueFPGA::NONE);
}


void FPGA::ReadPoint()
{
    if (_GET_BIT(ReadFlag(), FL_POINT))
    {
        BitSet16 dataA(*RD_ADC_A);
        BitSet16 dataB(*RD_ADC_B);

        Storage::P2P::AddPoints(dataA, dataB);
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
