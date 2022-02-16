// 2022/02/11 17:48:36 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "FPGA/FPGA.h"
#include "FPGA/Storage.h"
#include "FPGA/TypesFPGA_old.h"
#include "Menu/Pages/PageMemory.h"
#include "Display/Display.h"
#include "Panel/Panel.h"
#include "Panel/Controls.h"
#include "Settings/Settings.h"
#include "Settings/SettingsChannel.h"
#include "Utils/Math.h"
#include "Utils/ProcessingSignal.h"
#include "Utils/Generator.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Settings/Settings.h"
#include "Utils/Generator.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>
#include <string.h>


namespace FPGA
{
    namespace FreqMeter
    {
        float freq = 0.0f;           // Частота, намеренная альтерой.
        float prevFreq = 0.0f;
        bool readPeriod = false;     // Установленный в true флаг означает, что частоту нужно считать по счётчику периода

        void ReadFreq();

        void ReadPeriod();
    }

    volatile int numberMeasuresForGates = 1000;

    int additionShift = 0;

    #define n 200
    const int Kr[] = {n / 2, n / 5, n / 10, n / 20, n / 50};
    #undef n

    DataSettings ds;

    uint8 dataRel0[FPGA::MAX_POINTS] = {0};   // Буфер используется для чтения данных первого канала.
    uint8 dataRel1[FPGA::MAX_POINTS] = {0};   // Буфер используется для чтения данных второго канала.

    Settings storingSettings;                // Здесь нужно уменьшить необходимый размер памяти - сохранять настройки только альтеры
    uint timeStart = 0;

    void ReadPoint();

    // Функция вызывается, когда можно считывать очередной сигнал.
    void OnTimerCanReadData();

    void ReadPoint();

    uint8 ReadFlag();

    // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
    void LoadSettings();

    void WriteToAnalog(TypeWriteAnalog::E type, uint data);

    void WriteToDAC(TypeWriteDAC::E type, uint16 data);

    // Прочитать данные.
    // necessaryShift - Признак того, что сигнал нужно смещать.
    // saveToStorage - Нужно в режиме рандомизатора для указания, что пора сохранять измерение
    void DataRead(bool necessaryShift, bool saveToStorage);

    bool CalculateGate(uint16 rand, uint16 *min, uint16 *max);

    int CalculateShift();

    // Инвертирует данные.
    void InverseDataIsNecessary(Chan::E, uint8 *data);

    void AutoFind();

    uint8 CalculateMinWithout0(uint8 buffer[100]);

    uint8 CalculateMaxWithout255(uint8 buffer[100]);

    bool ProcessingData();

    void ReadRandomizeMode();

    void ReadRealMode(bool necessaryShift);

    Range::E AccurateFindRange(Chan::E);

    TBase::E FindTBase(Chan::E);

    TBase::E AccurateFindTBase(Chan::E);

    bool FindWave(Chan::E);
}


void FPGA::Init() 
{
    Storage::Clear();
    FPGA::LoadSettings();
    FPGA::SetNumSignalsInSec(sDisplay_NumSignalsInS());
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
} 


void FPGA::SetNumSignalsInSec(int numSigInSec) 
{
    Timer::Enable(TypeTimer::NumSignalsInSec, 1000.f / numSigInSec, OnTimerCanReadData);
}


void FPGA::OnTimerCanReadData()
{
    FPGA_CAN_READ_DATA = 1;
}


void FPGA::Start()
{
    if(SET_TBASE >= TBase::MIN_P2P)
    {
        Display::ResetP2Ppoints(false);
        Timer::Enable(TypeTimer::P2P, 1, ReadPoint);
    }
    else
    {
        Timer::Disable(TypeTimer::P2P);
        Display::ResetP2Ppoints(true);
    }
    HAL_FMC::Write(WR_START, 1);
    ds.FillDataPointer();
    timeStart = gTimerMS;
    StateWorkFPGA::SetCurrent(StateWorkFPGA::Wait);
    FPGA_CRITICAL_SITUATION = 0;
}


void FPGA::SwitchingTrig()
{
    HAL_FMC::Write(WR_TRIG_F, TRIG_POLARITY_IS_FRONT ? 0x00U : 0x01U);
    HAL_FMC::Write(WR_TRIG_F, TRIG_POLARITY_IS_FRONT ? 0x01U : 0x00U);
}


bool FPGA::ProcessingData()
{
    bool retValue = false;

    int num = (sTime_RandomizeModeEnabled() && (!START_MODE_IS_SINGLE) && SAMPLE_TYPE_IS_EQUAL) ? Kr[SET_TBASE] : 1;
    
   for (int i = 0; i < num; i++)
   {
        uint8 flag = ReadFlag();
        if (FPGA_CRITICAL_SITUATION)
        {
            if (gTimerMS - timeStart > 500)
            {
                SwitchingTrig();
                TRIG_AUTO_FIND = 1;
                FPGA_CRITICAL_SITUATION = 0;
            }
            else if (_GET_BIT(flag, BIT_TRIG))
            {
                FPGA_CRITICAL_SITUATION = 0;
            }
        }
        else if (_GET_BIT(flag, BIT_DATA_READY))
        {
            if (set.debug.showRegisters.flag)
            {
                char buffer[9];
                LOG_WRITE("флаг готовности %s", Bin2String(flag, buffer));
            }
            Panel::EnableLEDTrig(true);
            FPGA::Stop(true);
            DataRead(_GET_BIT(flag, BIT_SIGN_SHIFT_POINT), (num == 1) || (i == num - 1));
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
                timeStart = gTimerMS;
            }
        }
        Panel::EnableLEDTrig(_GET_BIT(flag, BIT_TRIG) ? true : false);
    }

    return retValue;
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

	if(AUTO_FIND_IN_PROGRESS)
    {
		AutoFind();
		return;
	}

    if((FPGA_CAN_READ_DATA == 0) || (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop))
    {
        return;
    }

    if(SET_SELFRECORDER)
    {
        ReadPoint();
    }

    ProcessingData();

    FPGA_CAN_READ_DATA = 0;
}


void FPGA::OnPressStartStop()
{
    if(StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop) 
    {
        FPGA::Start();
    } 
    else
    {
        FPGA::Stop(false);
    } 
}


void FPGA::Stop(bool pause) 
{
    Timer::Disable(TypeTimer::P2P);
    HAL_FMC::Write(WR_STOP, 1);
    StateWorkFPGA::SetCurrent(pause ? StateWorkFPGA::Pause : StateWorkFPGA::Stop);
}


bool FPGA::IsRunning()
{
    return (StateWorkFPGA::GetCurrent() != StateWorkFPGA::Stop);
}


#define WRITE_AND_OR_INVERSE(addr, data, ch)                                                      \
    if(SET_INVERSE(ch))                                                                   \
    {                                                                                               \
        data = (uint8)((int)(2 * ValueFPGA::AVE) - LimitationUInt8(data, ValueFPGA::MIN, ValueFPGA::MAX));    \
    }                                                                                               \
    *addr = data;


void FPGA::ReadRandomizeMode()
{
    int Tsm = CalculateShift();
    if (Tsm == TShift::EMPTY)
    {
        return;
    };

    int step = Kr[SET_TBASE];
    int index = Tsm - step - additionShift;

    if (index < 0)
    {
        index += step;        // WARN
    }

    uint8 *pData0 = &dataRel0[index];
    const uint8 * const pData0Last = &dataRel0[FPGA::MAX_POINTS - 1];
    uint8 *pData1 = &dataRel1[index];
    const uint8 * const pData1Last = &dataRel1[FPGA::MAX_POINTS - 1];

    const uint8 * const first0 = &dataRel0[0];
    const uint8 * const last0 = pData0Last;
    const uint8 * const first1 = &dataRel1[0];
    const uint8 * const last1 = pData1Last;

    int numAve = NUM_AVE_FOR_RAND;

    if (sDisplay_NumAverage() > numAve)
    {
        numAve = sDisplay_NumAverage();
    }

//    int addShiftMem = step / 2;

    if (START_MODE_IS_SINGLE || SAMPLE_TYPE_IS_REAL)
    {
        FPGA::ClearData();
    }

    while (pData0 < &dataRel0[FPGA::MAX_POINTS])
    {
        uint8 data10 = *RD_ADC_B2; //-V566
        uint8 data00 = *RD_ADC_A2; //-V566
        if (pData0 >= first0 && pData0 <= last0)
        {
            WRITE_AND_OR_INVERSE(pData0, data00, Chan::A);
        }

//        uint8 *addr = pData0 + addShiftMem;

        if (pData1 >= first1 && pData1 <= last1)
        {
            WRITE_AND_OR_INVERSE(pData1, data10, Chan::B);
        }

//        addr = pData1 + addShiftMem;

        pData0 += step;
        pData1 += step;
    }

    if (START_MODE_IS_SINGLE || SAMPLE_TYPE_IS_REAL)
    {
        Processing::InterpolationSinX_X(dataRel0, SET_TBASE);
        Processing::InterpolationSinX_X(dataRel1, SET_TBASE);
    }
}


void FPGA::ReadRealMode(bool necessaryShift)
{
    uint8 *p0 = &dataRel0[0];
    uint8 *p1 = &dataRel1[0];
    uint8 *endP = &dataRel0[FPGA::MAX_POINTS];

    if (ds.peakDet != PeackDetMode::Disable)
    {
        uint8 *p0min = p0;
        uint8 *p0max = p0min + 512;
        uint8 *p1min = p1;
        uint8 *p1max = p1min + 512;
        while (p0max < endP && FPGA_IN_PROCESS_READ)
        {
            uint8 data = *RD_ADC_B2; //-V566
            *p1max++ = data;
            data = *RD_ADC_B1;
            *p1min++ = data;
            data = *RD_ADC_A2;
            *p0min++ = data;
            data = *RD_ADC_A1;
            *p0max++ = data;
        }
    }
    else
    {
        while (p0 < endP && FPGA_IN_PROCESS_READ)
        {
            *p1++ = *RD_ADC_B2;
            *p1++ = *RD_ADC_B1;
            uint8 data = *RD_ADC_A2; //-V566
            *p0++ = data;
            data = *RD_ADC_A1;
            *p0++ = data;
        }

        int shift = 0;
        if (SET_TBASE == TBase::_100ns || SET_TBASE == TBase::_200ns)
        {
            shift = CalculateShift();
        }
        else if (necessaryShift)
        {
            //shift = set.debug.altShift;       WARN Остановились на жёстком задании дополнительного смещения. На PageDebug выбор закомментирован, можно включить при необходимости
            shift = -1;
        }
        if (shift != 0)
        {
            if (shift < 0)
            {
                shift = -shift;
                for (int i = FPGA::MAX_POINTS - shift - 1; i >= 0; i--)
                {
                    dataRel0[i + shift] = dataRel0[i];
                    dataRel1[i + shift] = dataRel1[i];
                }
            }
            else
            {
                for (int i = shift; i < FPGA::MAX_POINTS; i++)
                {
                    dataRel0[i - shift] = dataRel0[i];
                    dataRel1[i - shift] = dataRel1[i];
                }
            }
        }
    }
}


void FPGA::DataRead(bool necessaryShift, bool saveToStorage) 
{
    Panel::EnableLEDTrig(false);
    FPGA_IN_PROCESS_READ = 1;
    if((TBase::E)ds.tBase < TBase::_100ns)
    {
        ReadRandomizeMode();
    } 
    else 
    {
        ReadRealMode(necessaryShift);
    }

    static uint prevTime = 0;

    if (saveToStorage || (gTimerMS - prevTime > 500))
    {
        prevTime = gTimerMS;
        if (!sTime_RandomizeModeEnabled())
        {
            InverseDataIsNecessary(Chan::A, dataRel0);
            InverseDataIsNecessary(Chan::B, dataRel1);
        }

        Storage::AddData(dataRel0, dataRel1, ds);

        if (TRIG_MODE_FIND_IS_AUTO && TRIG_AUTO_FIND)
        {
            FPGA::FindAndSetTrigLevel();
            TRIG_AUTO_FIND = 0;
        }
    }
    FPGA_IN_PROCESS_READ = 0;
}


void FPGA::SetAdditionShift(int shift)
{
    additionShift = shift;
}


bool FPGA::CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax)
{   
    if(FPGA_FIRST_AFTER_WRITE)
    {
        FPGA_FIRST_AFTER_WRITE = 0;
        return false;
    }
    
    if(rand < 500 || rand > 3500)
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
    if(rand < min)
    {
        min = rand;
    }
    if(rand > max)
    {
        max = rand;
    }

    if(minGate == 0)
    {
        *eMin = min;
        *eMax = max;
        if(numElements < numValues)
        {
            return true;
        }
        minGate = min;
        maxGate = max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    if(numElements == numValues)
    {
        minGate = 0.9f * minGate + 0.1f * min;
        maxGate = 0.9f * maxGate + 0.1f * max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }
    *eMin = minGate; //-V519
    *eMax = maxGate; //-V519

    return true;
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
    
    if (sTime_RandomizeModeEnabled())
    {
        float tin = (float)(rand - min) / (max - min) * 10e-9f;
        int retValue = tin / 10e-9f * Kr[SET_TBASE];
        return retValue;
    }

    if (SET_TBASE == TBase::_100ns && rand < (min + max) / 2)
    {
        return 0;
    }

    return -1;  // set.debug.altShift;      \todo Остановились на жёстком задании дополнительного смещения. На PageDebug выбор закомментирован, 
                                            //можно раскомментировать при необходимости
}


void FPGA::WriteToHardware(uint8 *address, uint8 value, bool restart)
{
    FPGA_FIRST_AFTER_WRITE = 1;
    if(restart)
    {
        if(FPGA_IN_PROCESS_READ)
        {
            FPGA::Stop(true);
            FPGA_IN_PROCESS_READ = 0;
            HAL_FMC::Write(address, value);
            FPGA::Start();
        }
        else
        {
            if(StateWorkFPGA::GetCurrent() != StateWorkFPGA::Stop)
            {
                FPGA::Stop(true);
                HAL_FMC::Write(address, value);
                FPGA::Start();
            }
            else
            {
                HAL_FMC::Write(address, value);
            }
        }
    }
    else
    {
        HAL_FMC::Write(address, value);
    }
}


void FPGA::WriteToHardware(uint16 *address, uint16 value, bool restart)
{
    LOG_ERROR("Не реализована функция %s %d", __FUNCTION__, __LINE__);
}


void FPGA::ReadPoint()
{
    if(_GET_BIT(ReadFlag(), BIT_POINT_READY))
    {
        uint8 dataB1 = *RD_ADC_B1; //-V566
        uint8 dataB2 = *RD_ADC_B2; //-V566
        uint8 dataA1 = *RD_ADC_A1; //-V566
        uint8 dataA2 = *RD_ADC_A2; //-V566
        Display::AddPoints(dataA2, dataA1, dataB2, dataB1);
    }
}


void FPGA::SaveState()
{
    gStateFPGA.stateWorkBeforeCalibration.value = StateWorkFPGA::GetCurrent();
    storingSettings = set;
}


void FPGA::RestoreState()
{
    int16 rShiftAdd[2][Range::Count][2];
    for (int ch = 0; ch < 2; ch++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < Range::Count; range++)
            {
                rShiftAdd[ch][range][mode] = RSHIFT_ADD(ch, range, mode);
            }
        }
    }
    set = storingSettings;
    for (int ch = 0; ch < 2; ch++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < Range::Count; range++)
            {
                 RSHIFT_ADD(ch, range, mode) = rShiftAdd[ch][range][mode];
            }
        }
    }
    FPGA::LoadSettings();
    if(gStateFPGA.stateWorkBeforeCalibration.value != StateWorkFPGA::Stop)
    {
        gStateFPGA.stateWorkBeforeCalibration = StateWorkFPGA::Stop;
        FPGA::OnPressStartStop();
    }
}


static BitSet32 ReadRegFreq()
{
    BitSet32 fr;
    fr.half_word[0] = HAL_FMC::Read(RD_FREQ_LOW);
    fr.half_word[1] = HAL_FMC::Read(RD_FREQ_HI);
    return fr;
}


static BitSet32 ReadRegPeriod()
{
    BitSet32 period;
    period.half_word[0] = HAL_FMC::Read(RD_PERIOD_LOW);
    period.half_word[1] = HAL_FMC::Read(RD_PERIOD_HI);
    return period;
}


static float FreqCounterToValue(BitSet32 *fr)
{
    return fr->word * 10.0f;
}


static float PeriodCounterToValue(BitSet32 *period)
{
    if (period->word == 0)
    {
        return 0.0f;
    }
    return 10e6f / (float)period->word;
}


void FPGA::FreqMeter::ReadFreq()            // Чтение счётчика частоты производится после того, как бит 4 флага RD_FL установится в едицину
{                                           // После чтения автоматически запускается новый цикл счёта
    BitSet32 freqFPGA = ReadRegFreq();

    if(freqFPGA.word < 1000) 
    {
        readPeriod = true;
    } 
    else 
    {
        float fr = FreqCounterToValue(&freqFPGA);
        if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
        {
            freq = ERROR_VALUE_FLOAT;
        }
        else
        {
            freq = fr;
        }
        prevFreq = fr;
    }
}


void FPGA::FreqMeter::ReadPeriod()
{
    BitSet32 periodFPGA = ReadRegPeriod();
    float fr = PeriodCounterToValue(&periodFPGA);
    if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
    {
        freq = ERROR_VALUE_FLOAT;
    }
    else
    {
        freq = fr;
    }
    prevFreq = fr;
    readPeriod = false;
}


static uint8 FPGA::ReadFlag()
{
    uint8 flag = HAL_FMC::Read(RD_FL);
    if(!FreqMeter::readPeriod) 
    {
        if(_GET_BIT(flag, BIT_FREQ_READY)) 
        {
            FreqMeter::ReadFreq();
        }
    }
    if(FreqMeter::readPeriod && _GET_BIT(flag, BIT_PERIOD_READY)) 
    {
        FreqMeter::ReadPeriod();
    }

    return flag;
}


static float CalculateFreqFromCounterFreq()
{
    while (_GET_BIT(HAL_FMC::Read(RD_FL), BIT_FREQ_READY) == 0) {};
    ReadRegFreq();
    while (_GET_BIT(HAL_FMC::Read(RD_FL), BIT_FREQ_READY) == 0) {};
    BitSet32 fr = ReadRegFreq();
    if (fr.word >= 5)
    {
        return FreqCounterToValue(&fr);
    }
    return 0.0f;
}


static float CalculateFreqFromCounterPeriod()
{
    uint time = gTimerMS;
    while (gTimerMS - time < 1000 && _GET_BIT(HAL_FMC::Read(RD_FL), BIT_PERIOD_READY) == 0) {};
    ReadRegPeriod();
    time = gTimerMS;
    while (gTimerMS - time < 1000 && _GET_BIT(HAL_FMC::Read(RD_FL), BIT_PERIOD_READY) == 0) {};
    BitSet32 period = ReadRegPeriod();
    if (period.word > 0 && (gTimerMS - time < 1000))
    {
        return PeriodCounterToValue(&period);
    }
    return 0.0f;
}


float FPGA::FreqMeter::GetFreq() 
{
    return freq;
}


void FPGA::ClearData()
{
    memset(dataRel0, 0, FPGA::MAX_POINTS);
    memset(dataRel1, 0, FPGA::MAX_POINTS);
}


bool FPGA::AllPointsRandomizer()
{
    if(SET_TBASE < TBase::_100ns) 
    {
        for(int i = 0; i < 281; i++) 
        {
            if(dataRel0[i] == 0) 
            {
                return false;   
            }
        }
    }
    return true;
}


void FPGA::InverseDataIsNecessary(Chan::E ch, uint8 *data)
{
    if(SET_INVERSE(ch))
    {
        for (int i = 0; i < FPGA::MAX_POINTS; i++)
        {
            data[i] = (uint8)((int)(2 * ValueFPGA::AVE) - LimitationUInt8(data[i], ValueFPGA::MIN, ValueFPGA::MAX));
        }
    }
}


void FPGA::SetNumberMeasuresForGates(int number)
{
    numberMeasuresForGates = number;
}


void FPGA::StartAutoFind()
{
	AUTO_FIND_IN_PROGRESS = 1;
}


uint8 FPGA::CalculateMinWithout0(uint8 buffer[100])
{
    // \todo На одном экземпляре был страшенныый глюк, когда без сигнала выбивались значения 0 и 255 в рандомных местах
    // Вот такой кастыиль на скорую ногу, чтобы нули выкинуть.
    uint8 min = 255;
    
    for (int i = 1; i < 100; i++)
    {
        if (buffer[i] > 0 && buffer[i] < min)
        {
            min = buffer[i];
        }
    }
    return min;
}


uint8 FPGA::CalculateMaxWithout255(uint8 buffer[100])
{
    // \todo На одном экземпляре был страшенныый глюк, когда без сигнала выбивались значения 0 и 255 в рандомных местах
    // Вот такой кастыиль на скорую ногу, чтобы нули выкинуть.
    uint8 max = 0;
    for(int i = 1; i < 100; i++)
    {
        if(buffer[i] < 255 && buffer[i] > max)
        {
            max = buffer[i];
        }
    }
    return max;
}


TBase::E CalculateTBase(float freq_)
{
    if     (freq_ >= 100e6f)  { return TBase::_2ns;   }
    else if(freq_ >= 40e6f)   { return TBase::_5ns;   }
    else if(freq_ >= 20e6f)   { return TBase::_10ns;  }
    else if(freq_ >= 10e6f)   { return TBase::_20ns;  }
    else if(freq_ >= 3e6f)    { return TBase::_50ns;  }
    else if(freq_ >= 2e6f)    { return TBase::_100ns; }
    else if(freq_ >= 900e3f)  { return TBase::_200ns; }
    else if(freq_ >= 400e3f)  { return TBase::_500ns; }
    else if(freq_ >= 200e3f)  { return TBase::_1us;   }
    else if(freq_ >= 90e3f)   { return TBase::_2us;   }
    else if(freq_ >= 30e3f)   { return TBase::_5us;   }
    else if(freq_ >= 20e3f)   { return TBase::_10us;  }
    else if(freq_ >= 10e3f)   { return TBase::_20us;  }
    else if(freq_ >= 4e3f)    { return TBase::_50us;  }
    else if(freq_ >= 2e3f)    { return TBase::_100us; }
    else if(freq_ >= 1e3f)    { return TBase::_200us; }
    else if(freq_ >= 350.0f)  { return TBase::_500us; }
    else if(freq_ >= 200.0f)  { return TBase::_1ms;   }
    else if(freq_ >= 100.0f)  { return TBase::_2ms;   }
    else if(freq_ >= 40.0f)   { return TBase::_5ms;   }
    else if(freq_ >= 20.0f)   { return TBase::_10ms;  }
    else if(freq_ >= 10.0f)   { return TBase::_20ms;  }
    else if(freq_ >= 4.0f)    { return TBase::_50ms;  }
    else if(freq_ >= 2.0f)    { return TBase::_100ms; }
    return TBase::_200ms;
}


void FPGA::AutoFind()
{
    //LOG_WRITE(" ");
    //Timer::StartLogging();

    //LOG_WRITE("Канал 1");
    if (!FindWave(Chan::A))
    {
        //LOG_WRITE("Канал 2");
        if(!FindWave(Chan::B))
        {
            Display::ShowWarningBad(SignalNotFound);
        }
    }

    Init();
    Start();
    
    AUTO_FIND_IN_PROGRESS = 0;
}


bool FPGA::FindWave(Chan::E ch)
{
    Settings settings = set;    // Сохраняем предыдущие настройки

    Stop(false);
    SET_ENABLED(ch) = true;
    TrigSource::Set((TrigSource::E)ch);
    TrigLev::Set((TrigSource::E)ch, TrigLev::ZERO);
    RShift::Set(ch, RShift::ZERO);
    ModeCouple::Set(ch, ModeCouple::AC);
    Range::E range = AccurateFindRange(ch);
    //LOG_WRITE("Range %s", RangeName(range));
    if(range != Range::Count)
    {
        SET_RANGE(ch) = range;
        TBase::E tBase = AccurateFindTBase(ch);
        if (tBase != TBase::Count)
        {
            SET_TBASE = tBase;
            TRIG_SOURCE = (TrigSource::E)ch;
            return true;
        }
    }

    set = settings;
    FPGA::LoadSettings();
    return false;
}


Range::E FPGA::AccurateFindRange(Chan::E ch)
{
    /*
    Алгоритм поиска.
    Устанавливаем развёртку 50 мс.
    Устанавливаем закрытый вход.
    Включаем пиковый детектор.
    1. Устанавливаем растяжку 20В/дел.
    Не дожидаясь СИ, начинаем считывать точки в поточечном режиме и считываем их 50 штук - 2 клетки с небольшим. За это время
        на экране уложится чуть более одного периода частоты 10 Гц.
    Проверяем считанную информацию на выход за пределы экрана. Если вышла - берём предыдущую растяжку (например, 2В вместо 1В) и возвращаем
        её в качестве результата.
    Если не вышла - берём следующую растяжку и повторяем шаги с 1.
    */

    uint8 buffer[100];  // Сюда будем считывать точки

    TBase::Set(TBase::_50ms);
    ModeCouple::Set(ch, ModeCouple::AC);
    PeackDetMode::E peackDetMode = PEAKDET;
    PeackDetMode::Set(PeackDetMode::Enable);

    for (int range = Range::Count - 1; range >= 0; range--)
    {
        //Timer::LogPointMS("1");
        FPGA::Stop(false);
        Range::Set(ch, (Range::E)range);
        Timer::PauseOnTime(10);
        FPGA::Start();

        for (int i = 0; i < 50; i++)
        {
            while (_GET_BIT(HAL_FMC::Read(RD_FL), BIT_POINT_READY) == 0) {};
            HAL_FMC::Read(RD_ADC_B2);
            HAL_FMC::Read(RD_ADC_B1);
            HAL_FMC::Read(RD_ADC_A2);
            HAL_FMC::Read(RD_ADC_A1);
        }

        if (ch == Chan::A)
        {
            for (int i = 0; i < 100; i += 2)
            {
                while (_GET_BIT(HAL_FMC::Read(RD_FL), BIT_POINT_READY) == 0) {};
                HAL_FMC::Read(RD_ADC_B2);
                HAL_FMC::Read(RD_ADC_B1);
                buffer[i] = HAL_FMC::Read(RD_ADC_A2);
                buffer[i + 1] = HAL_FMC::Read(RD_ADC_A1);
            }
        }
        else
        {
            for (int i = 0; i < 100; i += 2)
            {
                while (_GET_BIT(HAL_FMC::Read(RD_FL), BIT_POINT_READY) == 0) {};
                buffer[i] = HAL_FMC::Read(RD_ADC_B2);
                buffer[i + 1] = HAL_FMC::Read(RD_ADC_B1);
                HAL_FMC::Read(RD_ADC_A2);
                HAL_FMC::Read(RD_ADC_A1);
            }
        }

        if (CalculateMinWithout0(buffer) < ValueFPGA::MIN || CalculateMaxWithout255(buffer) > ValueFPGA::MAX)
        {
            if (range < Range::_20V)
            {
                range++;
            }
            PeackDetMode::Set(peackDetMode);
            return (Range::E)range;
        }

        uint8 min = ValueFPGA::AVE - 30;
        uint8 max = ValueFPGA::AVE + 30;

        if(range == Range::_2mV && CalculateMinWithout0(buffer) > min && CalculateMaxWithout255(buffer) < max)
        {
            PeackDetMode::Set(peackDetMode);
            return Range::Count;
        }
    }

    PeackDetMode::Set(peackDetMode);
    return Range::Count;
}


TBase::E FPGA::AccurateFindTBase(Chan::E ch)
{
    for (int i = 0; i < 5; i++)
    {
        TBase::E tBase = FindTBase(ch);
        TBase::E secondTBase = FindTBase(ch); //-V656

        if (tBase == secondTBase && tBase != TBase::Count)
        {
            return tBase;
        }
    }
    return TBase::Count;
}


TBase::E FPGA::FindTBase(Chan::E ch)
{
    TrigInput::Set(TrigInput::Full);
    Timer::PauseOnTime(10);
    FPGA::Stop(false);
    float fr = CalculateFreqFromCounterFreq();

    TrigInput::Set(fr < 1e6f ? TrigInput::LPF : TrigInput::Full);

    fr = CalculateFreqFromCounterFreq();

    TBase::E tBase = TBase::Count;

    if (fr >= 50.0f)
    {
        tBase = CalculateTBase(fr);
        TBase::Set(tBase);
        FPGA::Start();
        TrigInput::Set(fr < 500e3f ? TrigInput::LPF : TrigInput::HPF);
        return tBase;
    }
    else
    {
        TrigInput::Set(TrigInput::LPF);
        FreqMeter::freq = CalculateFreqFromCounterPeriod();

        if (fr > 0.0f)
        {
            tBase = CalculateTBase(fr);
            TBase::Set(tBase);
            Timer::PauseOnTime(10);
            FPGA::Start();
            return tBase;
        }
    }

    return TBase::Count;
}


void StopTemporaryPause()
{
    FPGA_TEMPORARY_PAUSE = 0;
    Timer::Disable(TypeTimer::TemporaryPauseFPGA);
}


void FPGA::TemporaryPause()
{
    FPGA_TEMPORARY_PAUSE = 1;
    Timer::Enable(TypeTimer::TemporaryPauseFPGA, 500, StopTemporaryPause);
}


void DataSettings::FillDataPointer()
{
    enableCh0 = sChannel_Enabled(Chan::A) ? 1U : 0U;
    enableCh1 = sChannel_Enabled(Chan::B) ? 1U : 0U;
    inverseCh0 = SET_INVERSE_A ? 1U : 0U;
    inverseCh1 = SET_INVERSE_B ? 1U : 0U;
    range[0] = SET_RANGE_A;
    range[1] = SET_RANGE_B;
    rShiftCh0 = (uint)SET_RSHIFT_A;
    rShiftCh1 = (uint)SET_RSHIFT_B;
    tBase = SET_TBASE;
    tShift = TSHIFT;
    modeCouple0 = SET_COUPLE_A;
    modeCouple1 = SET_COUPLE_B;
    length1channel = (uint)sMemory_GetNumPoints(false);
    trigLevCh0 = (uint)TRIG_LEVEL_A;
    trigLevCh1 = (uint)TRIG_LEVEL_B;
    peakDet = (uint)PEAKDET;
    multiplier0 = SET_DIVIDER_A;
    multiplier1 = SET_DIVIDER_B;
}


void FPGA::FindAndSetTrigLevel()
{
    TrigSource::E trigSource = TRIG_SOURCE;
    if (Storage::AllDatas() == 0 || TRIG_SOURCE_IS_EXT)
    {
        return;
    }

    Chan::E chanTrig = (Chan::E)trigSource;
    uint8 *data0 = 0;
    uint8 *data1 = 0;
    DataSettings *ds_ = 0;

    Storage::GetDataFromEnd(0, &ds_, &data0, &data1);

    const uint8 *data = (chanTrig == Chan::A) ? data0 : data1;

    int lastPoint = (int)ds_->length1channel - 1;

    uint8 min = Math_GetMinFromArray(data, 0, lastPoint);
    uint8 max = Math_GetMaxFromArray(data, 0, lastPoint);

    uint8 aveValue = (uint8)(((int)min + (int)max) / 2);

    static const float scale = (float)(TrigLev::MAX - TrigLev::ZERO) / (float)(ValueFPGA::MAX - ValueFPGA::AVE) / 2.4f;

    int16 trigLev = TrigLev::ZERO + scale * ((int)aveValue - ValueFPGA::AVE) - (SET_RSHIFT(chanTrig) - RShift::ZERO);

    TrigLev::Set(trigSource, trigLev);
}


#define pinCLC      GPIO_PIN_2
#define pinData     GPIO_PIN_3
#define CHIP_SELECT_IN_LOW  HAL_GPIO_WritePin(GPIOG, pinSelect, GPIO_PIN_RESET);
#define CHIP_SELECT_IN_HI   HAL_GPIO_WritePin(GPIOG, pinSelect, GPIO_PIN_SET);
#define CLC_HI              HAL_GPIO_WritePin(GPIOG, pinCLC, GPIO_PIN_SET);
#define CLC_LOW             HAL_GPIO_WritePin(GPIOG, pinCLC, GPIO_PIN_RESET);
#define DATA_SET(x)         HAL_GPIO_WritePin(GPIOG, pinData, x);


void FPGA::WriteToAnalog(TypeWriteAnalog::E type, uint data)
{
#define pinSelect   GPIO_PIN_5

    CHIP_SELECT_IN_LOW
        for (int i = 23; i >= 0; i--)
        {
            DATA_SET((data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            CLC_HI
                CLC_LOW
        }
    CHIP_SELECT_IN_HI;
}


void FPGA::WriteToDAC(TypeWriteDAC::E type, uint16 data)
{
#undef pinSelect
#define pinSelect   GPIO_PIN_7
    char buffer[19];
    if (type == TypeWriteDAC::RShiftA && IS_SHOW_REG_RSHIFT_A)
    {
        LOG_WRITE("rShift 0 = %s", Bin2String16(data, buffer));
    }
    else if (type == TypeWriteDAC::RShiftB && IS_SHOW_REG_RSHIFT_B)
    {
        LOG_WRITE("rShfit 1 = %s", Bin2String16(data, buffer));
    }
    else if (type == TypeWriteDAC::TrigLev && IS_SHOW_REG_TRIGLEV)
    {
        LOG_WRITE("trigLev = %s", Bin2String16(data, buffer));
    }

    CHIP_SELECT_IN_LOW
        for (int i = 15; i >= 0; i--)
        {
            DATA_SET((data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            CLC_HI
                CLC_LOW
        }
    CHIP_SELECT_IN_HI;
}
