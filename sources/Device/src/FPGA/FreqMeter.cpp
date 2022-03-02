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
        float frequency = 0.0f;
        bool readPeriod = false;     // Установленный в true флаг означает, что частоту нужно считать по счётчику периода
        float prevFreq = 0.0f;

        BitSet32 ReadRegFreq();

        BitSet32 ReadRegPeriod();

        BitSet32 ReadRegFrequency();

        void Update(uint16 flag);

        void ReadFrequency();

        void ReadPeriod();

        float CalculateFrequencyFromCounterFrequency();

        float CalculateFrequencyFromCounterPeriod();

        namespace Frequency
        {
            float FromFrequencyCounter(const BitSet32 &fr) { return fr.word * 10.0F; }

            float FromPeriodCounter(const BitSet32 &period)
            {
                if (period.word == 0) { return 0.0F; }

                return 10e6F / period.word;
            }

            float FromFrequencySet(const BitSet32 &fr) { return (fr.word * 10.0f); }

            float FromPeriodSet(const BitSet32 &period)
            {
                if (period.word == 0) { return 0.0f; }

                return (10e5f / (float)period.word);
            }
        }
    }
}


float FPGA::FreqMeter::GetFreq()
{
    return frequency;
}


BitSet32 FPGA::FreqMeter::ReadRegFreq()
{
    BitSet32 fr;
    fr.half_word[0] = HAL_FMC::Read(RD_FREQ_LOW);
    fr.half_word[1] = HAL_FMC::Read(RD_FREQ_HI);
    return fr;
}


BitSet32 FPGA::FreqMeter::ReadRegPeriod()
{
    BitSet32 period;
    period.half_word[0] = HAL_FMC::Read(RD_PERIOD_LOW);
    period.half_word[1] = HAL_FMC::Read(RD_PERIOD_HI);

    return period;
}


BitSet32 FPGA::FreqMeter::ReadRegFrequency()
{
    BitSet32 fr;

    fr.half_word[0] = (uint8)HAL_FMC::Read(RD_FREQ_LOW);
    fr.half_word[1] = (uint8)HAL_FMC::Read(RD_FREQ_HI);

    return fr;
}


void FPGA::FreqMeter::Update(uint16 flag)
{
    bool freqReady = _GET_BIT(flag, FL_FREQ) == 1;
    bool periodReady = _GET_BIT(flag, FL_PERIOD) == 1;

    if (freqReady)
    {
        if (!readPeriod)
        {
            ReadFrequency();
        }
    }

    if (periodReady)
    {
        if (readPeriod)
        {
            ReadPeriod();
        }
    }
}


void FPGA::FreqMeter::ReadFrequency()            // Чтение счётчика частоты производится после того, как бит 4 флага RD_FL установится в едицину
{                                           // После чтения автоматически запускается новый цикл счёта
    BitSet32 freqSet = ReadRegFreq();

    if (freqSet.word < 1000)
    {
        readPeriod = true;
    }
    else
    {
        float fr = Frequency::FromFrequencyCounter(freqSet);

        if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
        {
            frequency = ERROR_VALUE_FLOAT;
        }
        else
        {
            frequency = fr;
        }

        prevFreq = fr;
    }
}


void FPGA::FreqMeter::ReadPeriod()
{
    BitSet32 periodSet = ReadRegPeriod();
    float fr = Frequency::FromPeriodSet(periodSet);

    if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
    {
        frequency = ERROR_VALUE_FLOAT;
    }
    else
    {
        frequency = fr;
    }

    prevFreq = fr;
    readPeriod = false;
}


float FPGA::FreqMeter::CalculateFrequencyFromCounterFrequency()
{
    frequency = 0.0f;

    while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_FREQ) == 0)
    {
    };

    ReadRegFrequency();

    while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_FREQ) == 0)
    {
    };

    BitSet32 fr = ReadRegFrequency();

    if (fr.word >= 5)
    {
        frequency = Frequency::FromFrequencyCounter(fr);
    }

    return 0.0F;
}
