// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Hardware/Timer.h"


struct FrameImitation
{
    DataStruct    data;

    // ����������� � ������ ����� ������.
    // � ������ ������������� - ���������� ��������� ������ �� Storage.
    // � ������ ����������� ������ - �������� �����
    void PrepareForNewCycle();

    void Inverse(Chan);

    void AppendPoints(BitSet16, BitSet16);

    // ������� ��� �������� ������ ��� ������� ����������
    void CreateForCurrent();

private:
    void AppendByte(uint8, uint8);
};


namespace Storage
{
    extern FrameImitation current;     // ���� ��������� ��������������� �� FPGA. ������������, ����� ���. ����� �����
                                    // ��������� � ���������
    extern TimeMeterMS time_meter;  // ��� ��������� ������� ���������� ���������� ������

    // �������� ���� ���������� ���������
    void Clear();

    // �������� ��������� ������. ��� ���� ����������� ��������� ������� ��������� �������.
    void Append(DataStruct &);

    // ������� ����� ��������� ��������� � ������.
    int NumFrames();

    // �������� ��������� �� ������
    const DataStruct &GetData(int fromEnd);

    // ������, ������� ����� �������� � ������� ������. ��� ������ �������� � ����� Storage (GetData(0))
    // ���� �������� ���������� (��� ����� �������������, � ������� ������ ����������), �� ������� ������ �� Averager
    const DataStruct &GetLatest();

    // ���������� ��������� �� ������, ��������� �� fromEnd o� ���������� �����������
    DataSettings GetDataSettings(int fromEnd);

    int NumberAvailableEntries();

    namespace SameSettings
    {
        // ���������� ����� ����������� ���������, ������� � ����������, � ������ �� �����������, ��� � ����������.
        int GetCount();
    }
};
