// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Data/Processing.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Data/Storage.h"
#include "Utils/Strings.h"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <climits>


namespace Processing
{
    struct MeasureValue
    {
        float value[2];
    };

    int firstP = 0;
    int lastP = 0;
    int numP = 0;

    DataStruct out;         // «десь хран€тс€ данные после обработки, готовые к выводу и расчЄту измерений

    MeasureValue values[Measure::Count] = {{0.0f, 0.0f}};

    int markerVolt[Chan::Count][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};
    int markerTime[Chan::Count][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};

    bool max_ready[2] = {false, false};
    bool min_ready[2] = {false, false};
    bool maxSteady_ready[2] = {false, false};
    bool minSteady_ready[2] = {false, false};
    bool ave_ready[2] = {false, false};
    bool period_ready[2] = {false, false};
    bool periodAccurate_ready[2];
    bool pic_ready[2] = {false, false};

#define EXIT_IF_ERROR_FLOAT(x)     if((x) == ERROR_VALUE_FLOAT)                             return ERROR_VALUE_FLOAT;
#define EXIT_IF_ERRORS_FLOAT(x, y) if((x) == ERROR_VALUE_FLOAT || (y) == ERROR_VALUE_FLOAT) return ERROR_VALUE_FLOAT;
#define EXIT_IF_ERROR_INT(x)       if((x) == ERROR_VALUE_INT)                               return ERROR_VALUE_FLOAT;

    float CalculateVoltageMax(Chan::E);
    float CalculateVoltageMin(Chan::E);
    float CalculateVoltagePic(Chan::E);
    float CalculateVoltageMaxSteady(Chan::E);
    float CalculateVoltageMinSteady(Chan::E);
    float CalculateVoltageAmpl(Chan::E);
    float CalculateVoltageAverage(Chan::E);
    float CalculateVoltageRMS(Chan::E);
    float CalculateVoltageVybrosPlus(Chan::E);
    float CalculateVoltageVybrosMinus(Chan::E);
    float CalculatePeriod(Chan::E);
    // “очно вычисл€ет период или целое число периодов в точках сигнала.
    int   CalculatePeriodAccurately(Chan::E);
    float CalculateFreq(Chan::E);
    float CalculateTimeNarastaniya(Chan::E);
    float CalculateTimeSpada(Chan::E);
    float CalculateDurationPlus(Chan::E);
    float CalculateDurationMinus(Chan::E);
    float CalculateSkvaznostPlus(Chan::E);
    float CalculateSkvaznostMinus(Chan::E);
    // ¬озвращает минимальное значение относительного сигнала    
    float CalculateMinRel(Chan::E);
    // ¬озвращает минимальное установившеес€ значение относительного сигнала
    float CalculateMinSteadyRel(Chan::E);
    // ¬озвращает максимальное значение относительного сигнала
    float CalculateMaxRel(Chan::E);
    // ¬озвращает максимальное установившеес€ значение относительного сигнала
    float CalculateMaxSteadyRel(Chan::E);
    // ¬озвращает среденее значение относительного сигнала
    float CalculateAverageRel(Chan::E);
    float CalculatePicRel(Chan::E);
    float CalculateDelayPlus(Chan::E);
    float CalculateDelayMinus(Chan::E);
    float CalculatePhazaPlus(Chan::E);
    float CalculatePhazaMinus(Chan::E);
    // Ќайти точку пересечени€ сигнала с горизонтальной линией, проведЄнной на уровне yLine. numItersection - пор€дковый
    // номер пересечени€, начинаетс€ с 1. downToTop - если true, ищем пересечение сигнала со средней линией при
    // прохождении из "-" в "+".
    float FindIntersectionWithHorLine(Chan::E, int numIntersection, bool downToUp, uint8 yLine);

    // ѕривести к текущим настройкам данные, из inA, inB. ƒанные сохран€ютс€ в Processing::out
    void CountedToCurrentSettings(const DataSettings &, const uint8 *inA, const uint8 *inB);

    typedef float  (*pFuncFCh)(Chan::E);
    typedef String (*pFuncConvert)(float, bool);

    struct MeasureCalculate
    {
        char *name;
        pFuncFCh     FuncCalculate;
        pFuncConvert FucnConvertate;
        // ≈сли true, нужно показывать знак.
        bool        showSign;
    };

    const MeasureCalculate measures[Measure::Count] =
    {
        {0, 0},
        {"CalculateVoltageMax",         CalculateVoltageMax,           SU::Voltage2String,    true},
        {"CalculateVoltageMin",         CalculateVoltageMin,           SU::Voltage2String,    true},
        {"CalculateVoltagePic",         CalculateVoltagePic,           SU::Voltage2String,    false},
        {"CalculateVoltageMaxSteady",   CalculateVoltageMaxSteady,     SU::Voltage2String,    true},
        {"CalculateVoltageMinSteady",   CalculateVoltageMinSteady,     SU::Voltage2String,    true},
        {"CalculateVoltageAmpl",        CalculateVoltageAmpl,          SU::Voltage2String,    false},
        {"CalculateVoltageAverage",     CalculateVoltageAverage,       SU::Voltage2String,    true},
        {"CalculateVoltageRMS",         CalculateVoltageRMS,           SU::Voltage2String,    false},
        {"CalculateVoltageVybrosPlus",  CalculateVoltageVybrosPlus,    SU::Voltage2String,    false},
        {"CalculateVoltageVybrosMinus", CalculateVoltageVybrosMinus,   SU::Voltage2String,    false},
        {"CalculatePeriod",             CalculatePeriod,               SU::Time2String,       false},
        {"CalculateFreq",               CalculateFreq,                 SU::Freq2String,       false},
        {"CalculateTimeNarastaniya",    CalculateTimeNarastaniya,      SU::Time2String,       false},
        {"CalculateTimeSpada",          CalculateTimeSpada,            SU::Time2String,       false},
        {"CalculateDurationPlus",       CalculateDurationPlus,         SU::Time2String,       false},
        {"CalculateDurationPlus",       CalculateDurationMinus,        SU::Time2String,       false},
        {"CalculateSkvaznostPlus",      CalculateSkvaznostPlus,        SU::FloatFract2String, false},
        {"CalculateSkvaznostMinus",     CalculateSkvaznostMinus,       SU::FloatFract2String, false},
        {"CalculateDelayPlus",          CalculateDelayPlus,            SU::Time2String,       false},
        {"CalculateDelayMinus",         CalculateDelayMinus,           SU::Time2String,       false},
        {"CalculatePhazaPlus",          CalculatePhazaPlus,            SU::Phase2String,      false},
        {"CalculatePhazaMinus",         CalculatePhazaMinus,           SU::Phase2String,      false}
    };
}


void Processing::CalculateMeasures()
{
    if(!SHOW_MEASURES || !Processing::out.ds.valid)
    {
        return;
    }

    max_ready[0] = max_ready[1] = maxSteady_ready[0] = maxSteady_ready[1] = false;
    min_ready[0] = min_ready[1] = minSteady_ready[0] = minSteady_ready[1] = false;
    ave_ready[0] = ave_ready[1] = false;
    period_ready[0] = period_ready[1] = false;
    periodAccurate_ready[0] = periodAccurate_ready[1] = false;
    pic_ready[0] = pic_ready[1] = false;

    for(int str = 0; str < Measures::NumRows(); str++)
    {
        for(int elem = 0; elem < Measures::NumCols(); elem++)
        {
            Measure::E meas = Measures::Type(str, elem);
            pFuncFCh func = measures[meas].FuncCalculate;

            if(func)
            {
                if(meas == MEAS_MARKED || MEAS_MARKED_IS_NONE)
                {
                    markerTime[Chan::A][0] = markerTime[Chan::A][1] = markerTime[Chan::B][0] = markerTime[Chan::B][1] = ERROR_VALUE_INT;
                    markerVolt[Chan::A][0] = markerVolt[Chan::A][1] = markerVolt[Chan::B][0] = markerVolt[Chan::B][1] = ERROR_VALUE_INT;
                }
                if(MEAS_SOURCE_IS_A || MEAS_SOURCE_IS_A_B)
                {
                    values[meas].value[Chan::A] = func(Chan::A);
                }
                if(MEAS_SOURCE_IS_B || MEAS_SOURCE_IS_A_B)
                {
                    values[meas].value[Chan::B] = func(Chan::B);
                }
            }
        }
    }
}

float Processing::CalculateVoltageMax(Chan::E ch)
{
    float max = CalculateMaxRel(ch);
    
    EXIT_IF_ERROR_FLOAT(max);

    if(MEAS_MARKED == Measure::VoltageMax)
    {
        markerVolt[ch][0] = max;                           // «десь не округл€ем, потому что max может быть только целым
    }

    return ValueFPGA::ToVoltage(max, out.ds.range[ch], out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageMin(Chan::E ch)
{
    float min = CalculateMinRel(ch);
    EXIT_IF_ERROR_FLOAT(min);

    if(MEAS_MARKED == Measure::VoltageMin)
    {
        markerVolt[ch][0] = min;                           // «десь не округл€ем, потому что min может быть только целым
    }

    return ValueFPGA::ToVoltage(min, out.ds.range[ch], out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltagePic(Chan::E ch)
{
    float max = CalculateVoltageMax(ch);
    float min = CalculateVoltageMin(ch);

    EXIT_IF_ERRORS_FLOAT(min, max);

    if(MEAS_MARKED == Measure::VoltagePic)
    {
        markerVolt[ch][0] = CalculateMaxRel(ch);
        markerVolt[ch][1] = CalculateMinRel(ch);
    }
    return max - min;
}

float Processing::CalculateVoltageMinSteady(Chan::E ch)
{
    float min = CalculateMinSteadyRel(ch);
    EXIT_IF_ERROR_FLOAT(min);

    if(MEAS_MARKED == Measure::VoltageMinSteady)
    {
        markerVolt[ch][0] = ROUND(min);
    }

    return ValueFPGA::ToVoltage(min, out.ds.range[ch], out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageMaxSteady(Chan::E ch)
{
    float max = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERROR_FLOAT(max);

    if(MEAS_MARKED == Measure::VoltageMaxSteady)
    {
        markerVolt[ch][0] = max;
    }

    Range::E range = out.ds.range[ch];

    return ValueFPGA::ToVoltage(max, range, out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageVybrosPlus(Chan::E ch)
{
    float max = CalculateMaxRel(ch);
    float maxSteady = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(max, maxSteady);

    if (MEAS_MARKED == Measure::VoltageVybrosPlus)
    {
        markerVolt[ch][0] = max;
        markerVolt[ch][1] = maxSteady;
    }

    int16 rShift = out.ds.GetRShift(ch);

    return std::fabsf(ValueFPGA::ToVoltage(maxSteady, out.ds.range[ch], rShift) -
        ValueFPGA::ToVoltage(max, out.ds.range[ch], rShift)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageVybrosMinus(Chan::E ch)
{
    float min = CalculateMinRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);
    EXIT_IF_ERRORS_FLOAT(min, minSteady);

    if (MEAS_MARKED == Measure::VoltageVybrosMinus)
    {
        markerVolt[ch][0] = min;
        markerVolt[ch][1] = minSteady;
    }

    int16 rShift = out.ds.GetRShift(ch);

    return std::fabsf(ValueFPGA::ToVoltage(minSteady, out.ds.range[ch], rShift) -
        ValueFPGA::ToVoltage(min, out.ds.range[ch], rShift)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageAmpl(Chan::E ch)
{
    float max = CalculateVoltageMaxSteady(ch);
    float min = CalculateVoltageMinSteady(ch);

    EXIT_IF_ERRORS_FLOAT(min, max);

    if(MEAS_MARKED == Measure::VoltageAmpl)
    {
        markerVolt[ch][0] = CalculateMaxSteadyRel(ch);
        markerVolt[ch][1] = CalculateMinSteadyRel(ch);
    }
    return max - min;
}

float Processing::CalculateVoltageAverage(Chan::E ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    int sum = 0;
    uint8 *data = &out.Data(ch)[firstP];

    for(int i = 0; i < period; i++)
    {
        sum += *data++;
    }

    uint8 aveRel = (float)sum / period;

    if(MEAS_MARKED == Measure::VoltageAverage)
    {
        markerVolt[ch][0] = aveRel;
    }

    return ValueFPGA::ToVoltage(aveRel, out.ds.range[ch], out.ds.GetRShift(ch)) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculateVoltageRMS(Chan::E ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    float rms = 0.0f;
    int16 rShift = out.ds.GetRShift(ch);

    for(int i = firstP; i < firstP + period; i++)
    {
        float volts = ValueFPGA::ToVoltage(out.Data(ch)[i], out.ds.range[ch], rShift);
        rms +=  volts * volts;
    }

    if(MEAS_MARKED == Measure::VoltageRMS)
    {
        markerVolt[ch][0] = ValueFPGA::FromVoltage(std::sqrt(rms / period), out.ds.range[ch], rShift);
    }

    return std::sqrt(rms / period) * SET_DIVIDER_ABS(ch);
}

float Processing::CalculatePeriod(Chan::E ch)
{
    static float period[2] = {0.0f, 0.0f};

    if(!period_ready[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        if(aveValue == ERROR_VALUE_UINT8)
        {
            period[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            const float intersectionDownToTop = FindIntersectionWithHorLine(ch, 1, true, aveValue);
            const float intersectionTopToDown = FindIntersectionWithHorLine(ch, 1, false, aveValue);

            EXIT_IF_ERRORS_FLOAT(intersectionDownToTop, intersectionTopToDown);

            float firstIntersection = (intersectionDownToTop < intersectionTopToDown) ? intersectionDownToTop : intersectionTopToDown;
            float secondIntersection = FindIntersectionWithHorLine(ch, 2, (intersectionDownToTop < intersectionTopToDown), aveValue);

            EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

            float per = TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tBase);

            markerTime[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
            markerTime[ch][1] = secondIntersection - SHIFT_IN_MEMORY;

            period[ch] = per;
            period_ready[ch] = true;
        }
    }

    return period[ch];
}

#define EXIT_FROM_PERIOD_ACCURACY       \
    period[ch] = ERROR_VALUE_INT;       \
    periodAccurate_ready[ch] = true;    \
    return period[ch];

int Processing::CalculatePeriodAccurately(Chan::E ch)
{
    static int period[2];

    int sums[FPGA::MAX_POINTS * 2];

    if(!periodAccurate_ready[ch])
    {
        period[ch] = 0;
        float pic = CalculatePicRel(ch);

        if(pic == ERROR_VALUE_FLOAT)
        {
            EXIT_FROM_PERIOD_ACCURACY
        }
        int delta = pic * 5;
        sums[firstP] = out.Data(ch)[firstP];

        int i = firstP + 1;
        int *sum = &sums[i];
        uint8 *data = &out.Data(ch)[i];
        uint8 *end = &out.Data(ch)[lastP];

        while (data < end)
        {
            uint8 point = *data++;
            if(point < ValueFPGA::MIN || point >= ValueFPGA::MAX)
            {
                EXIT_FROM_PERIOD_ACCURACY
            }
            *sum = *(sum - 1) + point;
            sum++;
        }

        int addShift = firstP - 1;
        int maxPeriod = numP * 0.95f;

        for(int nextPeriod = 10; nextPeriod < maxPeriod; nextPeriod++)
        {
            int s = sums[addShift + nextPeriod];

            int maxDelta = 0;
            int maxStart = numP - nextPeriod;

            int *pSums = &sums[firstP + 1];
            for(int start = 1; start < maxStart; start++)
            {
                int nextSum = *(pSums + nextPeriod) - (*pSums);
                pSums++;

                int nextDelta = nextSum - s;
                if (nextSum < s)
                {
                    nextDelta = -nextDelta;
                }

                if(nextDelta > delta)
                {
                    maxDelta = delta + 1;
                    break;
                }
                else if(nextDelta > maxDelta)
                {
                    maxDelta = nextDelta;
                }
            }

            if(maxDelta < delta)
            {
                delta = maxDelta;
                period[ch] = nextPeriod;
            }
        }

        if(period[ch] == 0)
        {
            period[ch] = ERROR_VALUE_INT;
        }
        periodAccurate_ready[ch] = true;
    }

    return period[ch];
}

float Processing::CalculateFreq(Chan::E ch)
{
    float period = CalculatePeriod(ch);
    return period == ERROR_VALUE_FLOAT ? ERROR_VALUE_FLOAT : 1.0f / period;
}

float Processing::FindIntersectionWithHorLine(Chan::E ch, int numIntersection, bool downToUp, uint8 yLine)
{
    int num = 0;
    int x = firstP;
    int compValue = lastP - 1;

    uint8 *data = &out.Data(ch)[0];

    if(downToUp)
    {
        while((num < numIntersection) && (x < compValue))
        {
            if(data[x] < yLine && data[x + 1] >= yLine)
            {
                num++;
            }
            x++;
        }
    }
    else
    {
        while((num < numIntersection) && (x < compValue))
        {
            if(data[x] > yLine && data[x + 1] <= yLine)
            {
                num++;
            }
            x++;
        }
    }
    x--;

    if (num < numIntersection)
    {
        return ERROR_VALUE_FLOAT;
    }
    return Math::GetIntersectionWithHorizontalLine(x, data[x], x + 1, data[x + 1], yLine);
}

float Processing::CalculateDurationPlus(Chan::E ch)
{
    float aveValue = CalculateAverageRel(ch);
    EXIT_IF_ERROR_FLOAT(aveValue);

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, true, aveValue);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, false, aveValue);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, false, aveValue);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tBase);
}

float Processing::CalculateDurationMinus(Chan::E ch)
{
    float aveValue = CalculateAverageRel(ch);
    EXIT_IF_ERROR_FLOAT(aveValue);

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, false, aveValue);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, true, aveValue);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, true, aveValue);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tBase);
}

float Processing::CalculateTimeNarastaniya(Chan::E ch)                    // WARN «десь, возможно, нужно увеличить точность - брать не целые значени рассто€ний между отсчЄтами по времени, а рассчитывать пересечени€ линий
{
    float maxSteady = CalculateMaxSteadyRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(maxSteady, minSteady);

    float value01 = (maxSteady - minSteady) * 0.1f;
    float max09 = maxSteady - value01;
    float min01 = minSteady + value01;

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, true, min01);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, true, max09);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);
    
    if (secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, true, max09);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    float retValue = TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tBase);

    if (MEAS_MARKED == Measure::TimeNarastaniya)
    {
        markerVolt[ch][0] = max09;
        markerVolt[ch][1] = min01;
        markerTime[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
        markerTime[ch][1] = secondIntersection - SHIFT_IN_MEMORY;
    }

    return retValue;
}

float Processing::CalculateTimeSpada(Chan::E ch)                          // WARN јналогично времени нарастани€
{
    float maxSteady = CalculateMaxSteadyRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(maxSteady, minSteady);

    float value01 = (maxSteady - minSteady) * 0.1f;
    float max09 = maxSteady - value01;
    float min01 = minSteady + value01;

    float firstIntersection = FindIntersectionWithHorLine(ch, 1, false, max09);
    float secondIntersection = FindIntersectionWithHorLine(ch, 1, false, min01);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if (secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(ch, 2, false, min01);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    float retValue = TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tBase);

    if (MEAS_MARKED == Measure::TimeSpada)
    {
        markerVolt[ch][0] = max09;
        markerVolt[ch][1] = min01;
        markerTime[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
        markerTime[ch][1] = secondIntersection - SHIFT_IN_MEMORY;
    }

    return retValue;
}

float Processing::CalculateSkvaznostPlus(Chan::E ch)
{
    float period = CalculatePeriod(ch);
    float duration = CalculateDurationPlus(ch);

    EXIT_IF_ERRORS_FLOAT(period, duration);

    return period / duration;
}

float Processing::CalculateSkvaznostMinus(Chan::E ch)
{
    float period = CalculatePeriod(ch);
    float duration = CalculateDurationMinus(ch);

    EXIT_IF_ERRORS_FLOAT(period, duration);

    return period / duration;
}

float Processing::CalculateMinSteadyRel(Chan::E ch)
{
    static float min[2] = {255.0f, 255.0f};

    if(!minSteady_ready[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        if(aveValue == ERROR_VALUE_FLOAT)
        {
            min[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            int sum = 0;
            int numSums = 0;
            uint8 *data = &out.Data(ch)[firstP];
            const uint8 * const end = &out.Data(ch)[lastP];
            while(data <= end)
            {
                uint8 d = *data++;
                if(d < aveValue)
                {
                    sum += d;
                    numSums++;
                }
            }
            min[ch] = (float)sum / numSums;
            int numMin = numSums;

            int numDeleted = 0;

            float pic = CalculatePicRel(ch);
            if (pic == ERROR_VALUE_FLOAT)
            {
                min[ch] = ERROR_VALUE_FLOAT;
            }
            else
            {
                float value = pic / 9.0f;

                data = &out.Data(ch)[firstP];
                float _min = min[ch];
                while (data <= end)
                {
                    uint8 d = *data++;
                    if (d < aveValue)
                    {
                        if (d < _min)
                        {
                            if (_min - d > value)
                            {
                                sum -= d;
                                --numSums;
                                ++numDeleted;
                            }
                        }
                        else if (d - _min > value)
                        {
                            sum -= d;
                            --numSums;
                            ++numDeleted;
                        }
                    }
                }
                min[ch] = (numDeleted > numMin / 2.0f) ? CalculateMinRel(ch) : (float)sum / numSums;
            }
        }
        minSteady_ready[ch] = true;
    }

    return min[ch];
}

float Processing::CalculateMaxSteadyRel(Chan::E ch)
{
    static float max[2] = {255.0f, 255.0f};

    if(!maxSteady_ready[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        
        if(aveValue == ERROR_VALUE_FLOAT)
        {
            max[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            int sum = 0;
            int numSums = 0;
            uint8 *data = &out.Data(ch)[firstP];
            const uint8 * const end = &out.Data(ch)[lastP];

            while (data <= end)
            {
                uint8 d = *data++;
                if(d > aveValue)
                {
                    sum += d;
                    numSums++;
                }
            }

            max[ch] = (float)sum / numSums;
            int numMax = numSums;

            int numDeleted = 0;

            float pic = CalculatePicRel(ch);

            if (pic == ERROR_VALUE_FLOAT)
            {
                max[ch] = ERROR_VALUE_FLOAT;
            }
            else
            {
                float value = pic / 9.0f;

                data = &out.Data(ch)[firstP];
                uint8 _max = max[ch];

                while (data <= end)
                {
                    uint8 d = *data++;
                    if (d > aveValue)
                    {
                        if (d > _max)
                        {
                            if (d - _max > value)
                            {
                                sum -= d;
                                numSums--;
                                numDeleted++;
                            }
                        }
                        else if (_max - d > value)
                        {
                            sum -= d;
                            numSums--;
                            numDeleted++;
                        }
                    }
                }

                max[ch] = (numDeleted > numMax / 2) ? CalculateMaxRel(ch) : (float)sum / numSums;
            }
        }
        maxSteady_ready[ch] = true;
    }

    return max[ch];
}

float Processing::CalculateMaxRel(Chan::E ch)
{
    static float max[2] = {0.0f, 0.0f};

    if(!max_ready[ch])
    {
        uint8 val = Math::GetMaxFromArrayWithErrorCode(out.Data(ch).Data(), firstP, lastP);
        max[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        max_ready[ch] = true;
    }

    return max[ch];
}

float Processing::CalculateMinRel(Chan::E ch)
{
    static float min[2] = {255.0f, 255.0f};

    if (!min_ready[ch])
    {
        uint8 val = Math::GetMinFromArrayWithErrorCode(out.Data(ch).Data(), firstP, lastP);
        min[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        min_ready[ch] = true;
    }

    return min[ch];
}

float Processing::CalculateAverageRel(Chan::E ch)
{
    static float ave[2] = {0.0f, 0.0f};

    if(!ave_ready[ch])
    {
        float min = CalculateMinRel(ch);
        float max = CalculateMaxRel(ch);
        ave[ch] = (min == ERROR_VALUE_FLOAT || max == ERROR_VALUE_FLOAT) ? ERROR_VALUE_FLOAT : (min + max) / 2.0f;
        ave_ready[ch] = true;
    }
    return ave[ch];
}

float Processing::CalculatePicRel(Chan::E ch)
{
    static float pic[2] = {0.0f, 0.0f};

    if(!pic_ready[ch])
    {
        float min = CalculateMinRel(ch);
        float max = CalculateMaxRel(ch);
        pic[ch] = (min == ERROR_VALUE_FLOAT || max == ERROR_VALUE_FLOAT) ? ERROR_VALUE_FLOAT : max - min;
        pic_ready[ch] = true;
    }
    return pic[ch];
}

float Processing::CalculateDelayPlus(Chan::E ch)
{
    float period0 = CalculatePeriod(Chan::A);
    float period1 = CalculatePeriod(Chan::B);

    EXIT_IF_ERRORS_FLOAT(period0, period1);
    if(!Math::FloatsIsEquals(period0, period1, 1.05f))
    {
        return ERROR_VALUE_FLOAT;
    }

    float average0 = CalculateAverageRel(Chan::A);
    float average1 = CalculateAverageRel(Chan::B);

    EXIT_IF_ERRORS_FLOAT(average0, average1);

    float firstIntersection = 0.0f;
    float secondIntersection = 0.0f;
    float averageFirst = ch == Chan::A ? average0 : average1;
    float averageSecond = ch == Chan::A ? average1 : average0;
    Chan::E firstChannel = ch == Chan::A ? Chan::A : Chan::B;
    Chan::E secondChannel = ch == Chan::A ? Chan::B : Chan::A;

    firstIntersection = FindIntersectionWithHorLine(firstChannel, 1, true, averageFirst);
    secondIntersection = FindIntersectionWithHorLine(secondChannel, 1, true, averageSecond);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(secondChannel, 2, true, averageSecond);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tBase);
}

float Processing::CalculateDelayMinus(Chan::E ch)
{
    float period0 = CalculatePeriod(Chan::A);
    float period1 = CalculatePeriod(Chan::B);

    EXIT_IF_ERRORS_FLOAT(period0, period1);

    if(!Math::FloatsIsEquals(period0, period1, 1.05f))
    {
        return ERROR_VALUE_FLOAT;
    }

    float average0 = CalculateAverageRel(Chan::A);
    float average1 = CalculateAverageRel(Chan::B);

    EXIT_IF_ERRORS_FLOAT(average0, average1);

    float firstIntersection = 0.0f;
    float secondIntersection = 0.0f;
    float averageFirst = ch == Chan::A ? average0 : average1;
    float averageSecond = ch == Chan::A ? average1 : average0;
    Chan::E firstChannel = ch == Chan::A ? Chan::A : Chan::B;
    Chan::E secondChannel = ch == Chan::A ? Chan::B : Chan::A;

    firstIntersection = FindIntersectionWithHorLine(firstChannel, 1, false, averageFirst);
    secondIntersection = FindIntersectionWithHorLine(secondChannel, 1, false, averageSecond);

    EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

    if(secondIntersection < firstIntersection)
    {
        secondIntersection = FindIntersectionWithHorLine(secondChannel, 2, false, averageSecond);
    }

    EXIT_IF_ERROR_FLOAT(secondIntersection);

    return TShift::ToAbs((secondIntersection - firstIntersection) / 2.0f, out.ds.tBase);
}

float Processing::CalculatePhazaPlus(Chan::E ch)
{
    float delay = CalculateDelayPlus(ch);
    float period = CalculatePeriod(ch);
    if(delay == ERROR_VALUE_FLOAT || period == ERROR_VALUE_FLOAT)
    {
        return ERROR_VALUE_FLOAT;
    }
    return delay / period * 360.0f;
}

float Processing::CalculatePhazaMinus(Chan::E ch)
{
    float delay = CalculateDelayMinus(ch);
    float period = CalculatePeriod(ch);
    if(delay == ERROR_VALUE_FLOAT || period == ERROR_VALUE_FLOAT)
    {
        return ERROR_VALUE_FLOAT;
    }
    return delay / period * 360.0f; 
}


float Processing::GetCursU(Chan ch, float posCurT)
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    float retValue = 0.0f;

    LIMITATION(retValue, 200 - (out.Data(ch))[points.half_iword[0] + (int)posCurT] + ValueFPGA::MIN, 0, 200);

    return retValue;
}


float Processing::GetCursT(Chan ch, float posCurU, int numCur)
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    int prevData = 200 - (out.Data(ch))[points.half_iword[0]] + ValueFPGA::MIN;

    int numIntersections = 0;

    for(int i = points.half_iword[0] + 1; i < points.half_iword[1]; i++)
    {
        int curData = 200 - (out.Data(ch))[i] + ValueFPGA::MIN;

        if(curData <= posCurU && prevData > posCurU)
        {
            if(numCur == 0)
            {
                return i - points.half_iword[0];
            }
            else
            {
                if(numIntersections == 0)
                {
                    numIntersections++;
                }
                else
                {
                    return i - points.half_iword[0];
                }
            }
        }

        if(curData >= posCurU && prevData < posCurU)
        {
            if(numCur == 0)
            {
                return i - points.half_iword[0];
            }
            else
            {
                if(numIntersections == 0)
                {
                    numIntersections++;
                }
                else
                {
                    return i - points.half_iword[1];
                }
            }
        }
        prevData = curData;
    }
    return 0;
}

void Processing::InterpolationSinX_X(uint8 data[FPGA::MAX_POINTS * 2], TBase::E tBase)
{
/*
     ѕоследовательности x в sin(x)
2    1. 50нс : pi/2, -pi/2 ...
8    2. 20нс : pi/5, pi/5 * 2, pi/5 * 3, pi/5 * 4, -pi/5 * 4, -pi/5 * 3, -pi/5 * 2, -pi/5 ...
18   3. 10нс : pi/10, pi/10 * 2 ... pi/10 * 9, -pi/10 * 9 .... -pi/10 * 2, -pi/10 ...
38   4. 5нс  : pi/20, pi/20 * 2 ... pi/20 * 19, -pi/20 * 19 ... -pi/20 * 2, -pi/20 ...
98   5. 2нс  : pi/50, pi/50 * 2 ... pi/50 * 49, -pi/50 * 49 ... -pi/50 * 2, -pi/50 ...
*/

#define MUL_SIN 1e7f
#define MUL     1e6f
#define KOEFF   (MUL / MUL_SIN)

    int deltas[5] = {50, 20, 10, 5, 2};
    int delta = deltas[tBase];

    uint8 signedData[FPGA::MAX_POINTS / 2];
    int numSignedPoints = 0;
    
    for (int pos = 0; pos < FPGA::MAX_POINTS; pos++)
    {
        if (data[pos] > 0)
        {
            signedData[numSignedPoints] = data[pos];
            numSignedPoints++;
        }
    }

    // ЌайдЄм смещение первой значащей точки

    int shift = 0;
    for (int pos = 0; pos < FPGA::MAX_POINTS; pos++)
    {
        if (data[pos] > 0)
        {
            shift = pos;
            break;
        }
    }

    float deltaX = PI;
    float stepX0 = PI / (float)delta;
    float x0 = PI - stepX0;
    int num = 0;
    
    for(int i = 0; i < FPGA::MAX_POINTS; i++)
    {
        x0 += stepX0;
        if((i % delta) == 0)
        {
            data[i] = signedData[i / delta];
        }
        else
        {
            int part = num % ((delta - 1) * 2);
            num++;
            float sinX = (part < delta - 1) ? std::sin(PI / delta * (part + 1)) : std::sin(PI / delta * (part - (delta - 1) * 2));

            if (tBase > TBase::_5ns)                 // «десь используем более быструю, но более неправильную арифметику целвых чисел
            {
                int sinXint = (int)(sinX * MUL_SIN);
                int value = 0;
                int x = (int)((x0 - deltaX) * MUL);
                int deltaXint = (int)(deltaX * MUL);

                for (int n = 0; n < numSignedPoints; n++)
                {
                    value += signedData[n] * sinXint / (x - n * deltaXint);
                    sinXint = -sinXint;
                }
                data[i] = (uint8)(value * KOEFF);
            }
            else                                    // Ќа этих развЄртках арифметика с плавающей зап€той даЄт приемлемое быстродействие
            {
                float value = 0.0f;
                float x = x0;

                for (int n = 0; n < numSignedPoints; n++)
                {
                    x -= deltaX;
                    value += signedData[n] * sinX / x;
                    sinX = -sinX;
                }
                data[i] = (uint8)value;
            }
        }
    }
    
    int pos = FPGA::MAX_POINTS - 1;
    while (pos > shift)
    {
        data[pos] = data[pos - shift];
        pos--;
    }
}

String Processing::GetStringMeasure(Measure::E measure, Chan ch)
{
    if (!SET_ENABLED(ch))
    {
        return String("");
    }

    String result(ch.IsA() ? "1: " : "2: ");

    if(!out.ds.valid)
    {
        result.Append("-.-");
    }
    else if(!SET_ENABLED(ch))
    {
    }
    else if(measures[measure].FuncCalculate)
    {
        pFuncConvert func = measures[measure].FucnConvertate;
        float value = values[measure].value[ch];
        String text = func(value, measures[measure].showSign);
        result.Append(text.c_str());
    }
    else
    {
        return result;
    }

    return result;
}

int Processing::GetMarkerVoltage(Chan ch, int numMarker)
{
    return markerVolt[ch][numMarker] - ValueFPGA::MIN;
}

int Processing::GetMarkerTime(Chan ch, int numMarker)
{
    return markerTime[ch][numMarker];
}


void Processing::CountedToCurrentSettings(const DataSettings &ds, const uint8 *dA, const uint8 *dB)
{
    int num_bytes = ds.BytesInChanStored();

    out.ds = ds;
    out.A.Realloc(num_bytes);
    out.A.Fill(ValueFPGA::NONE);
    out.B.Realloc(num_bytes);
    out.B.Fill(ValueFPGA::NONE);

    int dataTShift = out.ds.tShift;
    int curTShift = SET_TSHIFT;

    int16 dTShift = curTShift - dataTShift;

    const uint8 *in_a = dA;
    const uint8 *in_b = dB;

    for (int i = 0; i < num_bytes; i++)
    {
        int index = i - dTShift;

        if (index >= 0 && index < num_bytes)
        {
            out.A[index] = in_a[i];
            out.B[index] = in_b[i];
        }
    }
 
    if ((out.ds.range[0] != SET_RANGE_A || out.ds.rShiftA != (uint)SET_RSHIFT_A))
    {
        Range::E range = SET_RANGE_A;
        RShift rShift = SET_RSHIFT_A;

        for (int i = 0; i < num_bytes; i++)
        {
            float absValue = ValueFPGA::ToVoltage(out.A[i], out.ds.range[0], (int16)out.ds.rShiftA);
            int relValue = (absValue + Range::MaxOnScreen(range) + rShift.ToAbs(range)) / Range::voltsInPoint[range] + ValueFPGA::MIN;

            if (relValue < ValueFPGA::MIN)       { out.A[i] = ValueFPGA::MIN; }
            else if (relValue > ValueFPGA::MAX)  { out.A[i] = ValueFPGA::MAX; }
            else                                 { out.A[i] = (uint8)relValue; }
        }
    }

    if ((out.ds.range[1] != SET_RANGE_B || out.ds.rShiftB != (uint)SET_RSHIFT_B))
    {
        Range::E range = SET_RANGE_B;
        RShift rShift = SET_RSHIFT_B;

        for (int i = 0; i < num_bytes; i++)
        {
            float absValue = ValueFPGA::ToVoltage(out.B[i], out.ds.range[1], (int16)out.ds.rShiftB);
            int relValue = (absValue + Range::MaxOnScreen(range) + rShift.ToAbs(range)) / Range::voltsInPoint[range] + ValueFPGA::MIN;

            if (relValue < ValueFPGA::MIN)       { out.B[i] = ValueFPGA::MIN; }
            else if (relValue > ValueFPGA::MAX)  { out.B[i] = ValueFPGA::MAX; }
            else                                 { out.B[i] = (uint8)relValue; }
        }
    }
}


void Processing::SetData(const DataStruct &in, bool mode_p2p)
{
    TimeMeterUS meter;

    out.ds.valid = 0;

    if (!in.ds.valid)
    {
        return;
    }

    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    firstP = points.half_iword[0];
    lastP = points.half_iword[1];
    numP = lastP - firstP;

    int length = in.ds.BytesInChanStored();

    BufferFPGA A(length, ValueFPGA::NONE);
    BufferFPGA B(length, ValueFPGA::NONE);

    Math::CalculateFiltrArray(in.A.DataConst(), A.Data(), length, Smoothing::ToPoints());
    Math::CalculateFiltrArray(in.B.DataConst(), B.Data(), length, Smoothing::ToPoints());

    CountedToCurrentSettings(in.ds, A.Data(), B.Data());

    out.ds.valid = 1;
    out.rec_points = in.rec_points;
    out.all_points = in.all_points;
    out.mode_p2p = mode_p2p;

    LOG_WRITE("%d ms", meter.ElapsedUS());
}


void Processing::SetDataForProcessing(bool for_window_memory)
{
    DataSettings last_ds = Storage::GetDataSettings(0);

    out.ds.valid = 0;

    if (TBase::InModeP2P())
    {
        if (START_MODE_IS_AUTO)
        {
            if (last_ds.valid && last_ds.Equal(Storage::current.data.ds) && Storage::time_meter.ElapsedTime() < 1000)
            {
                SetData(Storage::GetLatest());
            }
            else
            {
                SetData(Storage::current.data, true);
            }
        }
        else if (START_MODE_IS_WAIT)
        {
            if (last_ds.valid && last_ds.Equal(Storage::current.data.ds) && !for_window_memory)
            {
                SetData(Storage::GetLatest());
            }
            else
            {
                SetData(Storage::current.data, true);
            }
        }
        else
        {
            if (Storage::current.data.ds.valid || for_window_memory)
            {
                SetData(Storage::current.data, for_window_memory);
            }
            else
            {
                SetData(Storage::GetLatest());
            }
        }
    }
    else
    {
        if (Storage::NumFrames())
        {
            SetData(Storage::GetLatest());
        }
    }
}
