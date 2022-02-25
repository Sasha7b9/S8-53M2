// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "ProcessingSignal.h"
#include "Math.h"
#include "GlobalFunctions.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>


struct MeasureValue
{
    float value[2];
};

static uint8 dataOut0[FPGA::MAX_POINTS * 2];
static uint8 dataOut1[FPGA::MAX_POINTS * 2];
static DataSettings *dataSet = 0;
static uint8 dataIn[2][FPGA::MAX_POINTS * 2];

static int firstP = 0;
static int lastP = 0;
static int numP = 0;

static MeasureValue values[Measure_NumMeasures] = {{0.0f, 0.0f}};

static int markerHor[Chan::Count][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};
static int markerVert[Chan::Count][2] = {{ERROR_VALUE_INT}, {ERROR_VALUE_INT}};

static bool maxIsCalculating[2] = {false, false};
static bool minIsCalculating[2] = {false, false};
static bool maxSteadyIsCalculating[2] = {false, false};
static bool minSteadyIsCalculating[2] = {false, false};
static bool aveIsCalculating[2] = {false, false};
static bool periodIsCaclulating[2] = {false, false};
static bool periodAccurateIsCalculating[2];
static bool picIsCalculating[2] = {false, false};

#define EXIT_IF_ERROR_FLOAT(x)      if((x) == ERROR_VALUE_FLOAT)                                return ERROR_VALUE_FLOAT;
#define EXIT_IF_ERRORS_FLOAT(x, y)  if((x) == ERROR_VALUE_FLOAT || (y) == ERROR_VALUE_FLOAT)    return ERROR_VALUE_FLOAT;
#define EXIT_IF_ERROR_INT(x)        if((x) == ERROR_VALUE_INT)                                  return ERROR_VALUE_FLOAT;


namespace Processing
{
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

    void CountedToCurrentSettings();

    typedef float (*pFuncFCh)(Chan::E);

    struct MeasureCalculate
    {
        char *name;
        pFuncFCh    FuncCalculate;
        pFuncCFBC   FucnConvertate;
        // ≈сли true, нужно показывать знак.
        bool        showSign;
    };

    const MeasureCalculate measures[Measure_NumMeasures] =
    {
        {0, 0},
        {"CalculateVoltageMax",         CalculateVoltageMax,           Voltage2String, true},
        {"CalculateVoltageMin",         CalculateVoltageMin,           Voltage2String, true},
        {"CalculateVoltagePic",         CalculateVoltagePic,           Voltage2String, false},
        {"CalculateVoltageMaxSteady",   CalculateVoltageMaxSteady,     Voltage2String, true},
        {"CalculateVoltageMinSteady",   CalculateVoltageMinSteady,     Voltage2String, true},
        {"CalculateVoltageAmpl",        CalculateVoltageAmpl,          Voltage2String, false},
        {"CalculateVoltageAverage",     CalculateVoltageAverage,       Voltage2String, true},
        {"CalculateVoltageRMS",         CalculateVoltageRMS,           Voltage2String, false},
        {"CalculateVoltageVybrosPlus",  CalculateVoltageVybrosPlus,    Voltage2String, false},
        {"CalculateVoltageVybrosMinus", CalculateVoltageVybrosMinus,   Voltage2String, false},
        {"CalculatePeriod",             CalculatePeriod,               Time2String, false},
        {"CalculateFreq",               CalculateFreq,                 Freq2String, false},
        {"CalculateTimeNarastaniya",    CalculateTimeNarastaniya,      Time2String, false},
        {"CalculateTimeSpada",          CalculateTimeSpada,            Time2String, false},
        {"CalculateDurationPlus",       CalculateDurationPlus,         Time2String, false},
        {"CalculateDurationPlus",       CalculateDurationMinus,        Time2String, false},
        {"CalculateSkvaznostPlus",      CalculateSkvaznostPlus,        FloatFract2String, false},
        {"CalculateSkvaznostMinus",     CalculateSkvaznostMinus,       FloatFract2String, false},
        {"CalculateDelayPlus",          CalculateDelayPlus,            Time2String, false},
        {"CalculateDelayMinus",         CalculateDelayMinus,           Time2String, false},
        {"CalculatePhazaPlus",          CalculatePhazaPlus,            Phase2String, false},
        {"CalculatePhazaMinus",         CalculatePhazaMinus,           Phase2String, false}
    };
}


void Processing::CalculateMeasures()
{
    if(!SHOW_MEASURES || !dataSet)
    {
        return;
    }

    maxIsCalculating[0] = maxIsCalculating[1] = maxSteadyIsCalculating[0] = maxSteadyIsCalculating[1] = false;
    minIsCalculating[0] = minIsCalculating[1] = minSteadyIsCalculating[0] = minSteadyIsCalculating[1] = false;
    aveIsCalculating[0] = aveIsCalculating[1] = false;
    periodIsCaclulating[0] = periodIsCaclulating[1] = false;
    periodAccurateIsCalculating[0] = periodAccurateIsCalculating[1] = false;
    picIsCalculating[0] = picIsCalculating[1] = false;

    for(int str = 0; str < Measure_NumRows(); str++)
    {
        for(int elem = 0; elem < Measure_NumCols(); elem++)
        {
            Measure meas = Measure_Type(str, elem);
            pFuncFCh func = measures[meas].FuncCalculate;

            if(func)
            {
                if(meas == MEAS_MARKED || MEAS_MARKED_IS_NONE)
                {
                    markerVert[Chan::A][0] = markerVert[Chan::A][1] = markerVert[Chan::B][0] = markerVert[Chan::B][1] = ERROR_VALUE_INT;
                    markerHor[Chan::A][0] = markerHor[Chan::A][1] = markerHor[Chan::B][0] = markerHor[Chan::B][1] = ERROR_VALUE_INT;
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
    if(MEAS_MARKED == Measure_VoltageMax)
    {
        markerHor[ch][0] = max;                           // «десь не округл€ем, потому что max может быть только целым
    }

    return POINT_2_VOLTAGE(max, dataSet->range[ch], ch == Chan::A ? dataSet->rShiftCh0 : dataSet->rShiftCh1) * VALUE_MULTIPLIER(ch);
}

float Processing::CalculateVoltageMin(Chan::E ch)
{
    float min = CalculateMinRel(ch);
    EXIT_IF_ERROR_FLOAT(min);
    if(MEAS_MARKED == Measure_VoltageMin)
    {
        markerHor[ch][0] = min;                           // «десь не округл€ем, потому что min может быть только целым
    }

    return POINT_2_VOLTAGE(min, dataSet->range[ch], ch == Chan::A ? dataSet->rShiftCh0 : dataSet->rShiftCh1) * VALUE_MULTIPLIER(ch);
}

float Processing::CalculateVoltagePic(Chan::E ch)
{
    float max = CalculateVoltageMax(ch);
    float min = CalculateVoltageMin(ch);

    EXIT_IF_ERRORS_FLOAT(min, max);

    if(MEAS_MARKED == Measure_VoltagePic)
    {
        markerHor[ch][0] = CalculateMaxRel(ch);
        markerHor[ch][1] = CalculateMinRel(ch);
    }
    return max - min;
}

float Processing::CalculateVoltageMinSteady(Chan::E ch)
{
    float min = CalculateMinSteadyRel(ch);
    EXIT_IF_ERROR_FLOAT(min);
    if(MEAS_MARKED == Measure_VoltageMinSteady)
    {
        markerHor[ch][0] = ROUND(min);
    }

    return (POINT_2_VOLTAGE(min, dataSet->range[ch], ch == Chan::A ? dataSet->rShiftCh0 : dataSet->rShiftCh1) * VALUE_MULTIPLIER(ch));
}

float Processing::CalculateVoltageMaxSteady(Chan::E ch)
{
    float max = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERROR_FLOAT(max);

    if(MEAS_MARKED == Measure_VoltageMaxSteady)
    {
        markerHor[ch][0] = max;
    }

    Range::E range = dataSet->range[ch];
    uint rShift = ch == Chan::A ? dataSet->rShiftCh0 : dataSet->rShiftCh1;

    return (POINT_2_VOLTAGE(max, range, rShift) * VALUE_MULTIPLIER(ch));
}

float Processing::CalculateVoltageVybrosPlus(Chan::E ch)
{
    float max = CalculateMaxRel(ch);
    float maxSteady = CalculateMaxSteadyRel(ch);

    EXIT_IF_ERRORS_FLOAT(max, maxSteady);

    if (MEAS_MARKED == Measure_VoltageVybrosPlus)
    {
        markerHor[ch][0] = max;
        markerHor[ch][1] = maxSteady;
    }

    int16 rShift = ch == Chan::A ? (int16)dataSet->rShiftCh0 : (int16)dataSet->rShiftCh1;
    return fabsf(POINT_2_VOLTAGE(maxSteady, dataSet->range[ch], rShift) - POINT_2_VOLTAGE(max, dataSet->range[ch], rShift)) * VALUE_MULTIPLIER(ch);
}

float Processing::CalculateVoltageVybrosMinus(Chan::E ch)
{
    float min = CalculateMinRel(ch);
    float minSteady = CalculateMinSteadyRel(ch);
    EXIT_IF_ERRORS_FLOAT(min, minSteady);

    if (MEAS_MARKED == Measure_VoltageVybrosMinus)
    {
        markerHor[ch][0] = min;
        markerHor[ch][1] = minSteady;
    }

    int16 rShift = ch == Chan::A ? (int16)dataSet->rShiftCh0 : (int16)dataSet->rShiftCh1;
    return fabsf(POINT_2_VOLTAGE(minSteady, dataSet->range[ch], rShift) - POINT_2_VOLTAGE(min, dataSet->range[ch], rShift)) * VALUE_MULTIPLIER(ch);
}

float Processing::CalculateVoltageAmpl(Chan::E ch)
{
    float max = CalculateVoltageMaxSteady(ch);
    float min = CalculateVoltageMinSteady(ch);

    EXIT_IF_ERRORS_FLOAT(min, max);

    if(MEAS_MARKED == Measure_VoltageAmpl)
    {
        markerHor[ch][0] = CalculateMaxSteadyRel(ch);
        markerHor[ch][1] = CalculateMinSteadyRel(ch);
    }
    return max - min;
}

float Processing::CalculateVoltageAverage(Chan::E ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    int sum = 0;
    uint8 *data = &dataIn[ch][firstP];
    for(int i = 0; i < period; i++)
    {
        sum += *data++;
    }

    uint8 aveRel = (float)sum / period;

    if(MEAS_MARKED == Measure_VoltageAverage)
    {
        markerHor[ch][0] = aveRel;
    }

    return (POINT_2_VOLTAGE(aveRel, dataSet->range[ch], ch == Chan::A ? dataSet->rShiftCh0 : dataSet->rShiftCh1) * VALUE_MULTIPLIER(ch));
}

float Processing::CalculateVoltageRMS(Chan::E ch)
{
    int period = CalculatePeriodAccurately(ch);

    EXIT_IF_ERROR_INT(period);

    float rms = 0.0f;
    int16 rShift = ch == Chan::A ? (int16)dataSet->rShiftCh0 : (int16)dataSet->rShiftCh1;
    for(int i = firstP; i < firstP + period; i++)
    {
        float volts = POINT_2_VOLTAGE(dataIn[ch][i], dataSet->range[ch], rShift);
        rms +=  volts * volts;
    }

    if(MEAS_MARKED == Measure_VoltageRMS)
    {
        markerHor[ch][0] = Math_VoltageToPoint(sqrt(rms / period), dataSet->range[ch], rShift);
    }

    return sqrt(rms / period) * VALUE_MULTIPLIER(ch);
}

float Processing::CalculatePeriod(Chan::E ch)
{
    static float period[2] = {0.0f, 0.0f};

    if(!periodIsCaclulating[ch])
    {
        float aveValue = CalculateAverageRel(ch);
        if(aveValue == ERROR_VALUE_UINT8)
        {
            period[ch] = ERROR_VALUE_FLOAT;
        }
        else
        {
            float intersectionDownToTop = FindIntersectionWithHorLine(ch, 1, true, aveValue);
            float intersectionTopToDown = FindIntersectionWithHorLine(ch, 1, false, aveValue);

            EXIT_IF_ERRORS_FLOAT(intersectionDownToTop, intersectionTopToDown);

            float firstIntersection = intersectionDownToTop < intersectionTopToDown ? intersectionDownToTop : intersectionTopToDown;
            float secondIntersection = FindIntersectionWithHorLine(ch, 2, intersectionDownToTop < intersectionTopToDown, aveValue);

            EXIT_IF_ERRORS_FLOAT(firstIntersection, secondIntersection);

            float per = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, dataSet->tBase);

            period[ch] = per;
            periodIsCaclulating[ch] = true;
        }
    }

    return period[ch];
}

#define EXIT_FROM_PERIOD_ACCURACY               \
    period[ch] = ERROR_VALUE_INT;             \
    periodAccurateIsCalculating[ch] = true;   \
    return period[ch];

int Processing::CalculatePeriodAccurately(Chan::E ch)
{
    static int period[2];

    int sums[FPGA::MAX_POINTS * 2];

    if(!periodAccurateIsCalculating[ch])
    {
        period[ch] = 0;
        float pic = CalculatePicRel(ch);

        if(pic == ERROR_VALUE_FLOAT)
        {
            EXIT_FROM_PERIOD_ACCURACY
        }
        int delta = pic * 5;
        sums[firstP] = dataIn[ch][firstP];

        int i = firstP + 1;
        int *sum = &sums[i];
        uint8 *data = &dataIn[ch][i];
        uint8 *end = &dataIn[ch][lastP];

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
        periodAccurateIsCalculating[ch] = true;
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

    uint8 *data = &dataIn[ch][0];

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
    return Math_GetIntersectionWithHorizontalLine(x, data[x], x + 1, data[x + 1], yLine);
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

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, dataSet->tBase);
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

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, dataSet->tBase);
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

    float retValue = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, dataSet->tBase);

    if (MEAS_MARKED == Measure_TimeNarastaniya)
    {
        markerHor[ch][0] = max09;
        markerHor[ch][1] = min01;
        markerVert[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
        markerVert[ch][1] = secondIntersection - SHIFT_IN_MEMORY;
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

    float retValue = TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, dataSet->tBase);

    if (MEAS_MARKED == Measure_TimeSpada)
    {
        markerHor[ch][0] = max09;
        markerHor[ch][1] = min01;
        markerVert[ch][0] = firstIntersection - SHIFT_IN_MEMORY;
        markerVert[ch][1] = secondIntersection - SHIFT_IN_MEMORY;
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

    if(!minSteadyIsCalculating[ch])
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
            uint8 *data = &dataIn[ch][firstP];
            const uint8 * const end = &dataIn[ch][lastP];
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

                data = &dataIn[ch][firstP];
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
        minSteadyIsCalculating[ch] = true;
    }

    return min[ch];
}

float Processing::CalculateMaxSteadyRel(Chan::E ch)
{
    static float max[2] = {255.0f, 255.0f};

    if(!maxSteadyIsCalculating[ch])
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
            uint8 *data = &dataIn[ch][firstP];
            const uint8 * const end = &dataIn[ch][lastP];
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

                data = &dataIn[ch][firstP];
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
        maxSteadyIsCalculating[ch] = true;
    }

    return max[ch];
}

float Processing::CalculateMaxRel(Chan::E ch)
{
    static float max[2] = {0.0f, 0.0f};

    if(!maxIsCalculating[ch])
    {
        uint8 val = Math_GetMaxFromArrayWithErrorCode(dataIn[ch], firstP, lastP);
        max[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        maxIsCalculating[ch] = true;
    }

    return max[ch];
}

float Processing::CalculateMinRel(Chan::E ch)
{
    static float min[2] = {255.0f, 255.0f};

    if (!minIsCalculating[ch])
    {
        uint8 val = Math_GetMinFromArrayWithErrorCode(dataIn[ch], firstP, lastP);
        min[ch] = val == ERROR_VALUE_UINT8 ? ERROR_VALUE_FLOAT : val;
        minIsCalculating[ch] = true;
    }

    return min[ch];
}

float Processing::CalculateAverageRel(Chan::E ch)
{
    static float ave[2] = {0.0f, 0.0f};

    if(!aveIsCalculating[ch])
    {
        float min = CalculateMinRel(ch);
        float max = CalculateMaxRel(ch);
        ave[ch] = (min == ERROR_VALUE_FLOAT || max == ERROR_VALUE_FLOAT) ? ERROR_VALUE_FLOAT : (min + max) / 2.0f;
        aveIsCalculating[ch] = true;
    }
    return ave[ch];
}

float Processing::CalculatePicRel(Chan::E ch)
{
    static float pic[2] = {0.0f, 0.0f};

    if(!picIsCalculating[ch])
    {
        float min = CalculateMinRel(ch);
        float max = CalculateMaxRel(ch);
        pic[ch] = (min == ERROR_VALUE_FLOAT || max == ERROR_VALUE_FLOAT) ? ERROR_VALUE_FLOAT : max - min;
        picIsCalculating[ch] = true;
    }
    return pic[ch];
}

float Processing::CalculateDelayPlus(Chan::E ch)
{
    float period0 = CalculatePeriod(Chan::A);
    float period1 = CalculatePeriod(Chan::B);

    EXIT_IF_ERRORS_FLOAT(period0, period1);
    if(!Math_FloatsIsEquals(period0, period1, 1.05f))
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

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, dataSet->tBase);
}

float Processing::CalculateDelayMinus(Chan::E ch)
{
    float period0 = CalculatePeriod(Chan::A);
    float period1 = CalculatePeriod(Chan::B);

    EXIT_IF_ERRORS_FLOAT(period0, period1);

    if(!Math_FloatsIsEquals(period0, period1, 1.05f))
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

    return TSHIFT_2_ABS((secondIntersection - firstIntersection) / 2.0f, dataSet->tBase);
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

void Processing::SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, const BitSet32 &points)
{
    firstP = points.half_iword[0];
    lastP = points.half_iword[1];
    numP = lastP - firstP;
    
    int numSmoothing = Smoothing::ToPoints();

    int length = ds->BytesInChannel();

    Math_CalculateFiltrArray(data0, &dataIn[Chan::A][0], length, numSmoothing);
    Math_CalculateFiltrArray(data1, &dataIn[Chan::B][0], length, numSmoothing);

    dataSet = ds;

    CountedToCurrentSettings();
}

void Processing::GetData(uint8 **data0, uint8 **data1, DataSettings **ds)
{
    if (data0)
    {
        *data0 = dataOut0;
    }
    if (data1)
    {
        *data1 = dataOut1;
    }
    *ds = dataSet;
}

float Processing::GetCursU(Chan::E ch, float posCurT)
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    float retValue = 0.0f;

    LIMITATION(retValue, 200 - (dataIn[ch])[points.half_iword[0] + (int)posCurT] + ValueFPGA::MIN, 0, 200);

    return retValue;
}

float Processing::GetCursT(Chan::E ch, float posCurU, int numCur)
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    int prevData = 200 - (dataIn[ch])[points.half_iword[0]] + ValueFPGA::MIN;

    int numIntersections = 0;

    for(int i = points.half_iword[0] + 1; i < points.half_iword[1]; i++)
    {
        int curData = 200 - (dataIn[ch])[i] + ValueFPGA::MIN;

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
            float sinX = (part < delta - 1) ? sin(PI / delta * (part + 1)) : sin(PI / delta * (part - (delta - 1) * 2));

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

char* Processing::GetStringMeasure(Measure measure, Chan::E ch, char buffer[20])
{
    if (!SET_ENABLED(ch))
    {
        return "";
    }
    buffer[0] = '\0';
    sprintf(buffer, ch == Chan::A ? "1: " : "2: ");
    if(dataSet == 0)
    {
        strcat(buffer, "-.-");
    }
    else if((ch == Chan::A && dataSet->enableCh0 == 0) || (ch == Chan::B && dataSet->enableCh1 == 0))
    {
    }
    else if(measures[measure].FuncCalculate)
    {
        char bufferForFunc[20];
        pFuncCFBC func = measures[measure].FucnConvertate;
        float value = values[measure].value[ch];
        char *text = func(value, measures[measure].showSign, bufferForFunc);
        strcat(buffer, text);
    }
    else
    {
        return buffer;
    }
    return buffer;
}

int Processing::GetMarkerHorizontal(Chan::E ch, int numMarker)
{
    return markerHor[ch][numMarker] - ValueFPGA::MIN;
}

int Processing::GetMarkerVertical(Chan::E ch, int numMarker)
{
    return markerVert[ch][numMarker];
}

void Processing::CountedToCurrentSettings()
{
    memset(dataOut0, 0, FPGA::MAX_POINTS);
    memset(dataOut1, 0, FPGA::MAX_POINTS);
    
    int numPoints = dataSet->BytesInChannel();

    int dataTShift = dataSet->tShift;
    int curTShift = TSHIFT;

    int16 dTShift = curTShift - dataTShift;
    for (int i = 0; i < numPoints; i++)
    {
        int index = i - dTShift;
        if (index >= 0 && index < numPoints)
        {
            dataOut0[index] = dataIn[0][i];
            dataOut1[index] = dataIn[1][i];
        }
    }
 
    if (dataSet->enableCh0 == 1U && (dataSet->range[0] != SET_RANGE_A || dataSet->rShiftCh0 != (uint)SET_RSHIFT_A))
    {
        Range::E range = SET_RANGE_A;
        int16 rShift = SET_RSHIFT_A;

        for (int i = 0; i < numPoints; i++)
        {
            float absValue = POINT_2_VOLTAGE(dataOut0[i], dataSet->range[0], dataSet->rShiftCh0);
            int relValue = (absValue + MAX_VOLTAGE_ON_SCREEN(range) + RSHIFT_2_ABS(rShift, range)) / voltsInPixel[range] + ValueFPGA::MIN;

            if (relValue < ValueFPGA::MIN)       { dataOut0[i] = ValueFPGA::MIN; }
            else if (relValue > ValueFPGA::MAX)  { dataOut0[i] = ValueFPGA::MAX; }
            else                            { dataOut0[i] = (uint8)relValue; }
        }
    }
    if (dataSet->enableCh1 == 1 && (dataSet->range[1] != SET_RANGE_B || dataSet->rShiftCh1 != (uint)SET_RSHIFT_B))
    {
        Range::E range = SET_RANGE_B;
        int16 rShift = SET_RSHIFT_B;

        for (int i = 0; i < numPoints; i++)
        {
            float absValue = POINT_2_VOLTAGE(dataOut1[i], dataSet->range[1], dataSet->rShiftCh1);
            int relValue = (absValue + MAX_VOLTAGE_ON_SCREEN(range) + RSHIFT_2_ABS(rShift, range)) / voltsInPixel[range] + ValueFPGA::MIN;

            if (relValue < ValueFPGA::MIN)       { dataOut1[i] = ValueFPGA::MIN; }
            else if (relValue > ValueFPGA::MAX)  { dataOut1[i] = ValueFPGA::MAX; }
            else                            { dataOut1[i] = (uint8)relValue; }
        }
    }
}
