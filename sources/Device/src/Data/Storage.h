// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Containers/Buffer.h"
#include "Hardware/Timer.h"


struct FrameImitation
{
    DataFrame     frame;
    Buffer<uint8> buffer;       // �����, ��� � Storage, ����� ��������� ������� DataSettings, � ����� ���������������
                                // ������ ������� � ������� �������

    // ����������� � ������ ����� ������.
    // � ������ ������������� - ���������� ��������� ������ �� Storage.
    // � ������ ����������� ������ - �������� �����
    void PrepareForNewCycle();

    void Inverse(Chan);

    void AppendPoints(BitSet16, BitSet16);

    // ������� ��� �������� ������ ��� ������� ����������
    void CreateForCurrent();
};


namespace Storage
{
    extern FrameImitation current;     // ���� ��������� ��������������� �� FPGA. ������������, ����� ���. ����� �����
                                    // ��������� � ���������
    extern TimeMeterMS time_meter;  // ��� ��������� ������� ���������� ���������� ������

    // �������� ���� ���������� ���������
    void Clear();

    // �������� ��������� ������. ��� ���� ����������� ��������� ������� ��������� �������.
    void AppendNewFrame(DataFrame &);

    // ������� ����� ��������� ��������� � ������.
    int NumFrames();

    // ���������� ����� ����������� ���������, ������� � ����������, � ������ �� �����������, ��� � ����������.
    int NumFramesWithSameSettings();

    // ���������� ����� ����������� ���������, ������� � ����������, � �������� ����������� �������
    int NumFramesWithCurrentSettings();

    // �������� ��������� �� ������
    DataFrame GetData(int fromEnd);

    // ������, ������� ����� �������� � ������� ������. ��� ������ �������� � ����� Storage
    DataFrame GetLatest();

    // ���������� ��������� �� ������, ��������� �� fromEnd o� ���������� �����������
    DataSettings *GetDataSettings(int fromEnd);

    uint8* GetData(Chan, int fromEnd);

    int NumberAvailableEntries();
};
