// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "FPGA/SettingsFPGA.h"


#define LIMIT_BELOW(x, min)         if(x < min) { x = min; }

#define LIMIT_ABOVE(x, max)         if(x > max) { x = max; }

#define SET_MIN_IF_LESS(x, min)     if((x) < (min)) { (min) = (x); }

#define SET_MAX_IF_LARGER(x, max)   if((x) > (max)) { (max) = (x); }

#define LIMITATION(var, value, min, max)    var = (value); if(var < (min)) { var = (min); } else if(var > (max)) { var = (max); };

#define LIMITATION_BOUND(var, min, max)     if((var) < (min)) { (min) = (var); }; if((var) > (max)) { (max) = (var); };

#define ROUND(x) (x += 0.5f, x)

#define PI 3.141592653589793f


namespace Math
{
    void  CalculateFiltrArray(const uint8 *in, uint8 *out, int numPoints, int numSmoothing);
    int   MinFrom2Int(int val0, int val1);
    int   Sign(int vlaue);
    float VoltageCursor(float shiftCurU, Range::E, RShift);
    float TimeCursor(float shiftCurT, TBase::E);
    void  DataExtrapolation(uint8 *data, uint8 *there, int size);
    uint8 GetMinFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint);
    uint8 GetMaxFromArray(const uint8 *data, int firstPoint, int lastPoint);
    uint8 GetMinFromArray(const uint8 *data, int firstPoint, int lastPoint);
    float MinFrom3float(float value1, float value2, float value3);
    int   MinInt(int val1, int val2);
    void  CalculateFFT(float *data, int numPoints, float *result, float *freq0, float *density0, float *freq1,
        float *density1, int *y0, int *y1);
    void  CalculateMathFunction(float *data0andResult, float *data1, int numPoints);
    float RandFloat(float min, float max);
    int8  AddInt8WithLimitation(int8 value, int8 delta, int8 min, int8 max);
    uint8 GetMaxFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint);

    // ���������� ���������� x ����������� �����, ���������� ����� (x0, y0), (x1, y1), � �������������� ������,
    // ���������� ����� ����� � ��������� yHorLine.
    float GetIntersectionWithHorizontalLine(int x0, int y0, int x1, int y1, int yHorLine);

    // ���������� ��� �����. ���������� true, ���� ����� ���������� ���� �� ����� �� �����, ��� �� epsilonPart. ��� ����
    // ��� ������� epsilonPart ������������ ������� � ������ ������ �����.
    bool FloatsIsEquals(float value0, float value1, float epsilonPart);

    // ��������� ����� �������� � ����� ���� int.
    int  NumDigitsInNumber(int value);

    // ���������� ������ value.
    int  FabsInt(int value);

    // ��������� 10**pow.
    int  Pow10(int pow);

    // ���������� ��� �����. ���������� true, ���� ����� ���������� ���� �� ����� �� �����, ��� �� epsilonPart. ���
    // ���� ��� ������� epsilonPart ������������ ������� � ������ ������ �����.
    bool FloatsIsEquals(float value0, float value1, float epsilonPart);

    // ��������� � �������� �� ������ val delta. ���� ��������� �� ����������� � �������� [min; max], ����������.
    template<class T> void AddLimitation(T *val, T delta, T min, T max);

    // ����������� �������� �� ������ val �� 1. �����, ���� ��������� ��������� max, ������������ ��� � min.
    template<class T> T CircleIncrease(T *val, T min, T max);

    // �������� �������� �� ������ val �� 1. �����, ���� ��������� ������ min, ������������ ��� max.
    template<class T> T CircleDecrease(T *val, T min, T max);

    // ���������� ������������ �������� �� ���.
    template<class T> T Max(T val1, T val2, T val3);

    // ���������� true, ���� value ������ � �������� [min; max].
    template<class T>
    bool InRange(T value, T min, T max);

    template<class T> T Limitation(T value, T min, T max)
    {
        if (value < min)      { return min; }
        else if (value > max) { return max; }

        return value;
    }

    template<class T> void Limmitation(T &value, T min, T max)
    {
        if (value < min)      { value = min; }
        else if (value > max) { value = max; }
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

    template<class T> T MinFrom2(T val1, T val2)
    {
        if (val1 < val2) { return val1; }

        return val2;
    }
}
