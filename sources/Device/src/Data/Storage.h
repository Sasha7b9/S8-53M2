// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Containers/Buffer.h"


// ��� ������� ����� - ���� ����� ������ ������ ����� ���, ��� ��������� � Storage
struct WorkingFrame : public DataSettings
{
    // ���������� �����
    int16  rec_point;   // ������ ����� �������� ��� �����. ���� -1 - �� ����� �� ����������. �� ������ ���������
    int    all_points;  // ����� �����

    BufferFPGA A;
    BufferFPGA B;

    void AppendPoints(BitSet16 pointsA, BitSet16 pointsB);
};


namespace Storage
{
    extern WorkingFrame working;

    // �������� ���� ���������� ���������
    void Clear();

    // �������� ��������� ������. ��� ���� ����������� ��������� ������� ��������� �������.
    void AddData(DataStruct &);

    // ������� ����� ��������� ��������� � ������.
    int NumFrames();

    // ���������� ����� ����������� ���������, ������� � ����������, � ������ �� �����������, ��� � ����������.
    int NumFramesWithSameSettings();

    // ���������� ����� ����������� ���������, ������� � ����������, � �������� ����������� �������
    int NumFramesWithCurrentSettings();

    // �������� ��������� �� ������
    bool GetData(int fromEnd, DataStruct &);

    // ���������� ��������� �� ������, ��������� �� fromEnd o� ���������� �����������
    DataSettings *GetDataSettings(int fromEnd);

    uint8* GetData(Chan, int fromEnd);

    // �������� �������������� ����� ������� 0 - �����, 1 - ������.
    uint8* GetLimitation(Chan, int direction);

    int NumberAvailableEntries();
};
