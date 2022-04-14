// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA.h"


namespace Processing
{
    extern DataStruct out;      // ����� �������� ������, ������� ��� ������ - ��������������� �� in

    // ���������� ������ ��� ��������� � ����������� � �������� ����������� � ����������
    // ���� for_windows_memory - ���������� ������ ��� ���� ������
    void SetDataForProcessing(ModeWork::E, bool for_window_memory);

    // ���������������� ��������� ��������� ������ ������������� �������� sinX/X.
    void InterpolationSinX_X(uint8 data[FPGA::MAX_POINTS * 2], TBase::E tbase);

    // ���������� ������ ��������������� ���������.
    String<> GetStringMeasure(Measure::E measure, Chan);

    // ��������� ��� ���������.
    void CalculateMeasures();

    // ���������� ������ �� in. ��������� ������� � Data::out
    // ���� mode_p2p == true, �� ����� ���������� ������� ����������� ������
    void SetData(const DataStruct &in, bool mode_p2p = false);

    // �������� � ������� ���������� ������, �� inA, inB. ������ ����������� � out
    void CountedToCurrentSettings(const DataSettings &, const uint8 *inA, const uint8 *inB, DataStruct &out);

    namespace Cursor
    {
        // �������� ������� ������� ����������, �������������� �������� ������� ������� posCurT.
        float GetU(Chan, float posCurT);

        // �������� ������� ������� �������, ��������������� �������� ������� ������� ���������� posCurU.
        float GetT(Chan, float posCurU, int numCur);
    }

    namespace Marker
    {
        // ���������� �������� ��������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
        int GetU(Chan, int numMarker);

        // ���������� �������� ������������� �������. ���� ERROR_VALUE_INT - ������ �������� �� �����.
        int GetT(Chan, int numMarker);
    }
};
