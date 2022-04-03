// 2022/02/21 11:58:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"


namespace FPGA
{
    namespace FreqMeter
    {
        float frequency = ERROR_VALUE_FLOAT;
        float prevFreq = 0.0f;

        BitSet32 ReadRegPeriod();

        BitSet32 ReadRegFrequency();

        void Update(uint16 flag);

        float FromFrequencyCounter(const BitSet32 &fr)
        {
            return fr.word * 10.0F;
        }

        float FromPeriodCounter(const BitSet32 &period)
        {
            return (period.word == 0) ? 0.0f : 1e6f / period.word;
        }
    }
}


void FPGA::FreqMeter::Reset()
{
    frequency = ERROR_VALUE_FLOAT;
}


float FPGA::FreqMeter::GetFreq()
{
    return frequency;
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


void FPGA::FreqMeter::Update(uint16 fl)
{
    if (_GET_BIT(fl, FL_FREQ))
    {
        BitSet32 freqSet = ReadRegFrequency();

        if (freqSet.word >= 1000)
        {
//            LOG_WRITE("Вычисляем по частоте %d", freqSet.word);

            float fr = FromFrequencyCounter(freqSet);

            if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
            {
                frequency = ERROR_VALUE_FLOAT;
            }

            prevFreq = fr;
        }
        else
        {
            if (_GET_BIT(fl, FL_PERIOD))
            {
                BitSet32 periodSet = ReadRegPeriod();

//                LOG_WRITE("Вычисляем по периоду %d", periodSet.word);

                float fr = FromPeriodCounter(periodSet);

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
    }
}
