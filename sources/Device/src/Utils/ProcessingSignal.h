// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA.h"


namespace Processing
{
    // ���������� ������ ��� ���������.
    void SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, const BitSet32 &points);

    // �������� ������ ����� �������������� �������.
    void GetData(uint8 **data0, uint8 **data1, DataSettings **ds);

    // �������� ������� ������� ����������, �������������� �������� ������� ������� posCurT.
    float GetCursU(Chan::E, float posCurT);

    // �������� ������� ������� �������, ��������������� �������� ������� ������� ���������� posCurU.
    float GetCursT(Chan::E, float posCurU, int numCur);

    // ���������������� ��������� ��������� ������ ������������� �������� sinX/X.
    void InterpolationSinX_X(uint8 data[FPGA::MAX_POINTS * 2], TBase::E tBase);

    // ���������� ������ ��������������� ���������.
    char* GetStringMeasure(Measure measure, Chan::E, char buffer[20]);

    // ��������� ��� ���������.
    void CalculateMeasures();

    // ���������� �������� ��������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
    int GetMarkerHorizontal(Chan::E, int numMarker);

    // ���������� �������� ������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
    int GetMarkerVertical(Chan::E, int numMarker);
};
