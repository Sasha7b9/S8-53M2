// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA_Types.h"


class Processing
{
public:
    // ���������� ������ ��� ���������.
    static void SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, int firstPoint, int lastPoint);
    // �������� ������ ����� �������������� �������.
    static void GetData(uint8 **data0, uint8 **data1, DataSettings **ds);
    // �������� ������� ������� ����������, �������������� �������� ������� ������� posCurT.
    static float GetCursU(Channel chan, float posCurT);
    // �������� ������� ������� �������, ��������������� �������� ������� ������� ���������� posCurU.
    static float GetCursT(Channel chan, float posCurU, int numCur);
    // ���������������� ��������� ��������� ������ ������������� �������� sinX/X.
    static void InterpolationSinX_X(uint8 data[FPGA_MAX_POINTS], TBase tBase);
    // ���������� ������ ��������������� ���������.
    static char* GetStringMeasure(Measure measure, Chan::E ch, char buffer[20]);
    // ��������� ��� ���������.
    static void CalculateMeasures();
    // ���������� �������� ��������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
    static int GetMarkerHorizontal(Channel chan, int numMarker);
    // ���������� �������� ������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
    static int GetMarkerVertical(Channel chan, int numMarker);
private:
    static float CalculateVoltageMax(Chan::E ch);
    static float CalculateVoltageMin(Chan::E ch);
    static float CalculateVoltagePic(Chan::E ch);
    static float CalculateVoltageMaxSteady(Chan::E ch);
    static float CalculateVoltageMinSteady(Chan::E ch);
    static float CalculateVoltageAmpl(Chan::E ch);
    static float CalculateVoltageAverage(Chan::E ch);
    static float CalculateVoltageRMS(Chan::E ch);
    static float CalculateVoltageVybrosPlus(Chan::E ch);
    static float CalculateVoltageVybrosMinus(Chan::E ch);
    static float CalculatePeriod(Chan::E ch);
    // ����� ��������� ������ ��� ����� ����� �������� � ������ �������.
    static int   CalculatePeriodAccurately(Chan::E ch);
    static float CalculateFreq(Chan::E ch);
    static float CalculateTimeNarastaniya(Chan::E ch);
    static float CalculateTimeSpada(Chan::E ch);
    static float CalculateDurationPlus(Chan::E ch);
    static float CalculateDurationMinus(Chan::E ch);
    static float CalculateSkvaznostPlus(Chan::E ch);
    static float CalculateSkvaznostMinus(Chan::E ch);
    // ���������� ����������� �������� �������������� �������    
    static float CalculateMinRel(Chan::E ch);
    // ���������� ����������� �������������� �������� �������������� �������
    static float CalculateMinSteadyRel(Chan::E ch);
    // ���������� ������������ �������� �������������� �������
    static float CalculateMaxRel(Chan::E ch);
    // ���������� ������������ �������������� �������� �������������� �������
    static float CalculateMaxSteadyRel(Chan::E ch);
    // ���������� �������� �������� �������������� �������
    static float CalculateAverageRel(Chan::E ch);
    static float CalculatePicRel(Chan::E ch);
    static float CalculateDelayPlus(Chan::E ch);
    static float CalculateDelayMinus(Chan::E ch);
    static float CalculatePhazaPlus(Chan::E ch);
    static float CalculatePhazaMinus(Chan::E ch);
    // ����� ����� ����������� ������� � �������������� ������, ���������� �� ������ yLine. numItersection - ���������� ����� �����������, 
    // ���������� � 1. downToTop - ���� true, ���� ����������� ������� �� ������� ������ ��� ����������� �� "-" � "+".
    static float FindIntersectionWithHorLine(Channel chan, int numIntersection, bool downToUp, uint8 yLine);

    static void CountedToCurrentSettings();

    typedef float (*pFuncFCh)(Channel);

    struct MeasureCalculate
    {
        char        *name;
        pFuncFCh    FuncCalculate;
        pFuncCFBC   FucnConvertate;
        // ���� true, ����� ���������� ����.
        bool        showSign;
    };

    static const MeasureCalculate measures[Measure_NumMeasures];
};
