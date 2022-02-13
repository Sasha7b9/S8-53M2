// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGA_Types.h"


class Processing
{
public:
    // ���������� ������ ��� ���������.
    static void SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, int firstPoint, int lastPoint);
    // �������� ������ ����� �������������� �������.
    static void GetData(uint8 **data0, uint8 **data1, DataSettings **ds);
    // �������� ������� ������� ����������, �������������� �������� ������� ������� posCurT.
    static float GetCursU(Chan::E, float posCurT);
    // �������� ������� ������� �������, ��������������� �������� ������� ������� ���������� posCurU.
    static float GetCursT(Chan::E, float posCurU, int numCur);
    // ���������������� ��������� ��������� ������ ������������� �������� sinX/X.
    static void InterpolationSinX_X(uint8 data[FPGA::MAX_POINTS], TBase tBase);
    // ���������� ������ ��������������� ���������.
    static char* GetStringMeasure(Measure measure, Chan::E ch, char buffer[20]);
    // ��������� ��� ���������.
    static void CalculateMeasures();
    // ���������� �������� ��������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
    static int GetMarkerHorizontal(Chan::E, int numMarker);
    // ���������� �������� ������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
    static int GetMarkerVertical(Chan::E, int numMarker);
private:
    static float CalculateVoltageMax(Chan::E);
    static float CalculateVoltageMin(Chan::E);
    static float CalculateVoltagePic(Chan::E);
    static float CalculateVoltageMaxSteady(Chan::E);
    static float CalculateVoltageMinSteady(Chan::E);
    static float CalculateVoltageAmpl(Chan::E);
    static float CalculateVoltageAverage(Chan::E);
    static float CalculateVoltageRMS(Chan::E);
    static float CalculateVoltageVybrosPlus(Chan::E);
    static float CalculateVoltageVybrosMinus(Chan::E);
    static float CalculatePeriod(Chan::E);
    // ����� ��������� ������ ��� ����� ����� �������� � ������ �������.
    static int   CalculatePeriodAccurately(Chan::E);
    static float CalculateFreq(Chan::E);
    static float CalculateTimeNarastaniya(Chan::E);
    static float CalculateTimeSpada(Chan::E);
    static float CalculateDurationPlus(Chan::E);
    static float CalculateDurationMinus(Chan::E);
    static float CalculateSkvaznostPlus(Chan::E);
    static float CalculateSkvaznostMinus(Chan::E);
    // ���������� ����������� �������� �������������� �������    
    static float CalculateMinRel(Chan::E);
    // ���������� ����������� �������������� �������� �������������� �������
    static float CalculateMinSteadyRel(Chan::E);
    // ���������� ������������ �������� �������������� �������
    static float CalculateMaxRel(Chan::E);
    // ���������� ������������ �������������� �������� �������������� �������
    static float CalculateMaxSteadyRel(Chan::E);
    // ���������� �������� �������� �������������� �������
    static float CalculateAverageRel(Chan::E);
    static float CalculatePicRel(Chan::E);
    static float CalculateDelayPlus(Chan::E);
    static float CalculateDelayMinus(Chan::E);
    static float CalculatePhazaPlus(Chan::E);
    static float CalculatePhazaMinus(Chan::E);
    // ����� ����� ����������� ������� � �������������� ������, ���������� �� ������ yLine. numItersection - ���������� ����� �����������, 
    // ���������� � 1. downToTop - ���� true, ���� ����������� ������� �� ������� ������ ��� ����������� �� "-" � "+".
    static float FindIntersectionWithHorLine(Chan::E, int numIntersection, bool downToUp, uint8 yLine);

    static void CountedToCurrentSettings();

    typedef float (*pFuncFCh)(Chan::E);

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
