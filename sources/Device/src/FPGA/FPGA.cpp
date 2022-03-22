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
#include <cstring>


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
    StateFPGA state =
    {
        false,
        StateWorkFPGA::Stop,
        StateCalibration::None
    };

    uint timeStart = 0;

    volatile static int numberMeasuresForGates = 1000;

    bool IN_PROCESS_READ = false;
    bool TRIG_AUTO_FIND = false;    // Установленное в 1 значение означает, что нужно производить автоматический поиск
                                    // синхронизации, если  выбрана соответствующая настройка.
    bool runningBeforeSmallButtons = false;
    bool TEMPORARY_PAUSE = false;
    bool CAN_READ_DATA = false;
    bool NEED_AUTO_TRIG = false;    // Если true, нужно делать автозапуск (автоматический режим запуска и отсутсвует
                                    // синхроимпульс
    bool FIRST_AFTER_WRITE = false; // Используется в режиме рандомизатора. После записи любого параметра в альтеру
                    // нужно не использовать первое считанное данное с АЦП, потому что оно завышено и портит ворота

    Flag flag;

    // Перед каждым циклом измерений нужно произвести некоторые действия по подготовке
    void PrepareForCycle();

    void ProcessingData();

    // Смещение с АЦП рандомизатора
    int ShiftRandomizerADC();

    bool CalculateGate(uint16 rand, uint16 *min, uint16 *max);

    // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
    void LoadSettings();

    // Эта функция вызывается по таймеру, когда можно считывать новыый сигнал
    void OnTimerCanReadData();

    void ProcessP2P();

    namespace FreqMeter
    {
        void Update(uint16 flag);
    }

    namespace Reader
    {
        // Возвращает адрес, с которого нужно читать первую точку
        extern uint16 CalculateAddressRead();

        // Чтение двух байт канала 1 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadA();

        // Чтение двух байт канала 2 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadB();
    }
}



void FPGA::Init()
{
    Storage::Clear();
    FPGA::LoadSettings();
    FPGA::SetNumSignalsInSec(ENumSignalsInSec::ToNum(ENUM_SIGNALS_IN_SEC));
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
}


void FPGA::Update()
{
    flag.Read();

    FPGA::Reader::P2P::SavePoints();

    if (state.needCalibration)              // Если вошли в режим калибровки -
    {
        FPGA::Calibrator::RunCalibrate();       // выполняем её.
        state.needCalibration = false;
    }

    if (TEMPORARY_PAUSE)
    {
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
        Reader::P2P::ReadPoints();
    }

    int num_cycles = TBase::StretchRand();

    for (int i = 0; i < num_cycles; i++)
    {
        ProcessingData();
    }

    CAN_READ_DATA = false;
}


void FPGA::ProcessingData()
{
    flag.Read();

    if (NEED_AUTO_TRIG)
    {
        if (TIME_MS - timeStart > 500)
        {
            SwitchingTrig();
            TRIG_AUTO_FIND = true;
            NEED_AUTO_TRIG = false;
        }
        else if (_GET_BIT(flag.value, FL_TRIG))
        {
            NEED_AUTO_TRIG = false;
        }
    }
    else if (_GET_BIT(flag.value, FL_DATA))
    {
        Panel::EnableLEDTrig(true);

        Stop(true);

        Reader::DataRead();

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
        if (flag.value & (1 << 2))
        {
            if (START_MODE_IS_AUTO)
            {
                NEED_AUTO_TRIG = true;
            }
            timeStart = TIME_MS;
        }
    }

    Panel::EnableLEDTrig(_GET_BIT(flag.value, FL_TRIG) ? true : false);
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


void FPGA::PrepareForCycle()
{
    Storage::current.PrepareForNewCycle();

    if (TBase::InModeP2P())
    {
        Timer::Enable(TypeTimer::P2P, 1, Reader::P2P::ReadPoints);
    }
    else
    {
        Timer::Disable(TypeTimer::P2P);
    }
}


void FPGA::Start()
{
    PrepareForCycle();

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


void BUS_FPGA::WriteDAC(TypeWriteDAC::E, uint16 data)
{
    bool running = FPGA::IsRunning();

    FPGA::Stop(false);

    Pin::SPI4_CS1.Reset();

    for (int i = 15; i >= 0; i--)
    {
        DATA_SET((data & (1 << i)) ? 1 : 0);
        CLC_HI
        CLC_LOW
    }

    Pin::SPI4_CS1.Set();

    if (running)
    {
        FPGA::Start();
    }
}


void BUS_FPGA::WriteAnalog(TypeWriteAnalog::E, uint data)
{
    bool running = FPGA::IsRunning();

    FPGA::Stop(false);

    Pin::SPI4_CS2.Reset();

    for (int i = 23; i >= 0; i--)
    {
        DATA_SET((data & (1 << i)) ? 1 : 0);
        CLC_HI
        CLC_LOW
    }

    Pin::SPI4_CS2.Set();

    if (running)
    {
        FPGA::Start();
    }
}


int FPGA::ShiftRandomizerADC()
{
    if (TBase::InModeRandomizer())
    {
        uint16 rand = HAL_ADC1::GetValue();

        uint16 min = 0;
        uint16 max = 0;

        if (Panel::TimePassedAfterLastEvent() < 200)
        {
            return TShift::ERROR;
        }

        if (!CalculateGate(rand, &min, &max))
        {
            return TShift::ERROR;
        }

        float tin = (float)(rand - min) / (float)(max - min) * 10e-9f;
        int shift = (int)(tin / 10e-9f * (float)TBase::StretchRand());

        return shift;
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
