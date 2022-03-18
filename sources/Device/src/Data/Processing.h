// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA.h"


namespace Processing
{
    // ���������� ������ ��� ���������. ��������� ����� ������� � Data::out
    void Process(DataFrame &in);

    // �������� ������� ������� ����������, �������������� �������� ������� ������� posCurT.
    float GetCursU(Chan, float posCurT);

    // �������� ������� ������� �������, ��������������� �������� ������� ������� ���������� posCurU.
    float GetCursT(Chan, float posCurU, int numCur);

    // ���������������� ��������� ��������� ������ ������������� �������� sinX/X.
    void InterpolationSinX_X(uint8 data[FPGA::MAX_POINTS * 2], TBase::E tBase);

    // ���������� ������ ��������������� ���������.
    char* GetStringMeasure(Measure::E measure, Chan, char buffer[20]);

    // ��������� ��� ���������.
    void CalculateMeasures();

    // ���������� �������� ��������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
    int GetMarkerHorizontal(Chan, int numMarker);

    // ���������� �������� ������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
    int GetMarkerVertical(Chan, int numMarker);
};
