// 2022/02/21 11:58:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <stm32f4xx_hal.h>


namespace FPGA
{
    namespace FreqMeter
    {
        float freq = 0.0f;           // Частота, намеренная альтерой.
        float prevFreq = 0.0f;

        volatile static BitSet32 freqActual;
        volatile static BitSet32 periodActual;

        bool readPeriod = false;     // Установленный в true флаг означает, что частоту нужно считать по счётчику периода

        float CalculateFreqFromCounterFreq();

        BitSet32 ReadRegFreq();

        float FreqCounterToValue(BitSet32 *fr);

        float CalculateFreqFromCounterPeriod();

        BitSet32 ReadRegPeriod();

        float PeriodCounterToValue(BitSet32 *period);

        void Update(uint16 flag);

        void ReadFreq();

        void ReadPeriod();
    }
}


float FPGA::FreqMeter::GetFreq()
{
    return 0.0f;
}


float FPGA::FreqMeter::CalculateFreqFromCounterFreq()
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


BitSet32 FPGA::FreqMeter::ReadRegFreq()
{
    BitSet32 fr;
    fr.half_word[0] = HAL_FMC::Read(RD_FREQ_LOW);
    fr.half_word[1] = HAL_FMC::Read(RD_FREQ_HI);
    return fr;
}


float FPGA::FreqMeter::FreqCounterToValue(BitSet32 *fr)
{
    return (float)fr->word * 10.0f;
}


float FPGA::FreqMeter::CalculateFreqFromCounterPeriod()
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


BitSet32 FPGA::FreqMeter::ReadRegPeriod()
{
    BitSet32 period;
    period.half_word[0] = HAL_FMC::Read(RD_PERIOD_LOW);
    period.half_word[1] = HAL_FMC::Read(RD_PERIOD_HI);
    return period;
}


float FPGA::FreqMeter::PeriodCounterToValue(BitSet32 *period)
{
    if (period->word == 0)
    {
        return 0.0f;
    }
    return 10e6f / (float)period->word;
}


void FPGA::FreqMeter::Update(uint16 flag)
{
    bool freqReady = _GET_BIT(flag, FL_FREQ) == 1;
    bool periodReady = _GET_BIT(flag, FL_PERIOD) == 1;

    if (freqReady)
    {
        freqActual.half_word[0] = *RD_FREQ_LOW;
        freqActual.half_word[1] = *RD_FREQ_HI;

        if (!readPeriod)
        {
            ReadFreq();
        }
    }

    if (periodReady)
    {
        periodActual.half_word[0] = *RD_PERIOD_LOW;
        periodActual.half_word[1] = *RD_PERIOD_HI;

        if (readPeriod)
        {
            ReadPeriod();
        }
    }
}


void FPGA::FreqMeter::ReadFreq()            // Чтение счётчика частоты производится после того, как бит 4 флага RD_FL установится в едицину
{                                           // После чтения автоматически запускается новый цикл счёта
    BitSet32 freqFPGA = ReadRegFreq();

    if (freqFPGA.word < 1000)
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
