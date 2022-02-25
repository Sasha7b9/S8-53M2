// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


#define LIMIT_BELOW(x, min)         if(x < min) { x = min; }

#define LIMIT_ABOVE(x, max)         if(x > max) { x = max; }

#define SET_MIN_IF_LESS(x, min)     if((x) < (min)) { (min) = (x); }

#define SET_MAX_IF_LARGER(x, max)   if((x) > (max)) { (max) = (x); }

#define LIMITATION(var, value, min, max)    var = (value); if(var < (min)) { var = (min); } else if(var > (max)) { var = (max); };

#define LIMITATION_BOUND(var, min, max)     if((var) < (min)) { (min) = (var); }; if((var) > (max)) { (max) = (var); };

#define MAX_VOLTAGE_ON_SCREEN(range) (tableScalesRange[(range)] * 5.0f)

#define RSHIFT_2_ABS(rShift, range) (-((float)RShift::ZERO - (float)(rShift)) * absStepRShift[(uint)(range)])

#define TSHIFT_2_ABS(tShift, tBase)  (absStepTShift[(tBase)] * (tShift) * 2.0f)

#define TSHIFT_2_REL(tShiftAbs, tBase) ((tShiftAbs) / absStepTShift[(tBase)] / 2.0f)

#define ROUND(x) (x += 0.5f, x)

#define PI 3.141592653589793f

extern const float tableScalesRange[Range::Count];
extern const float absStepRShift[];
extern const float voltsInPixel[];
extern const float absStepTShift[];

namespace Math
{
    int   MinFrom2Int(int val0, int val1);
    int   Sign(int vlaue);
    // ¬ычисл€ет 10**pow.
    int   Pow10(int pow);

    float VoltageCursor(float shiftCurU, Range::E, int16 rShift);
    float TimeCursor(float shiftCurT, TBase::E);

    uint8 GetMaxFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint);

    template<class T> T Limitation(T value, T min, T max)
    {
        if (value < min)      { return min; }
        else if (value > max) { return max; }

        return value;
    }

    template<class T> void Swap(T *value0, T *value1)
    {
        T temp = *value0; *value0 = *value1; *value1 = temp;
    }

    template<class T> void Sort(T *value1, T *value2)
    {
        if (*value1 > *value2)
        {
            T temp = *value1;
            *value1 = *value2;
            *value2 = temp;
        }
    }

    template<class T> void Limitation(T *value, T min, T max)
    {
        if (*value < min) { *value = min; }
        else if (*value > max) { *value = max; }
    }
}

void    Math_DataExtrapolation(uint8 *data, uint8 *there, int size);
void    Math_PointsRelToVoltage(const uint8 *points, int numPoints, Range::E, int16 rShift, float *voltage);
uint8   Math_VoltageToPoint(float voltage, Range::E, int16 rShift);
void	Math_PointsVoltageToRel(const float *voltage, int numPoints, Range::E, int16 rShift, uint8 *points);
uint8   Math_GetMinFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint);
uint8   Math_GetMaxFromArray(const uint8 *data, int firstPoint, int lastPoint);
uint8   Math_GetMinFromArray(const uint8 *data, int firstPoint, int lastPoint);
// \brief ¬озвращает координату x пересечени€ линии, проход€щей через (x0, y0), (x1, y1), с горизонтальной линией, проход€щей через точку с 
// ординатой yHorLine.
float   Math_GetIntersectionWithHorizontalLine(int x0, int y0, int x1, int y1, int yHorLine);
// \brief —равнивает два числа. ¬озвращает true, если числа отличаютс€ друг от друга не более, чем на epsilonPart. ѕри этом дл€ расчЄта epsilonPart 
// используетс€ большее в смысле модул€ число.
bool    Math_FloatsIsEquals(float value0, float value1, float epsilonPart);
float   Math_MinFrom3float(float value1, float value2, float value3);
int     Math_MinInt(int val1, int val2);
void    Math_CalculateFFT(float *data, int numPoints, float *result, float *freq0, float *density0, float *freq1, float *density1, int *y0, int *y1);
void    Math_CalculateMathFunction(float *data0andResult, float *data1, int numPoints);
float   Math_RandFloat(float min, float max);
int8    Math_AddInt8WithLimitation(int8 value, int8 delta, int8 min, int8 max);
// ¬ычисл€ет число разр€дов в целом типа int.
int     Math_NumDigitsInNumber(int value);
// ¬озвращает модуль value.
int     Math_FabsInt(int value);

uint8   Math_CalculateFiltr(const uint8 *data, int x, int numPoints, int numSmoothing);
void    Math_CalculateFiltrArray(const uint8 *dataIn, uint8 *dataOut, int numPoints, int numSmoothing);
