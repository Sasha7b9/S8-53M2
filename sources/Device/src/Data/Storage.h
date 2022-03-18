// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Containers/Buffer.h"


namespace Storage
{
    // �������� ���� ���������� ���������
    void Clear();

    // �������� ��������� ������. ��� ���� ����������� ��������� ������� ��������� �������.
//    void AddData(DataStruct &);

    // �������� ������ ������. �� ��������� � ��������� � � ���� ����� �������� ������
    void OpenFrame();

    // �������� ����� ��������� �������� OpenFrame() ������
    void CloseFrame();

    // ������� ����� ����� ����������� ������.
    void CreateFrameP2P(const DataSettings &);

    // ������� ����� ��������� ��������� � ������.
    int NumFrames();

    // ���������� ����� ����������� ���������, ������� � ����������, � ������ �� �����������, ��� � ����������.
    int NumFramesWithSameSettings();

    // ���������� ����� ����������� ���������, ������� � ����������, � �������� ����������� �������
    int NumFramesWithCurrentSettings();

    // �������� ��������� �� ������
    bool GetData(int fromEnd, DataStruct &);

    // ���������� ��������� �� ������, ��������� �� fromEnd o� ���������� �����������. "����������" ����� �� ����������
    DataSettings *GetDataSettings(int fromEnd);

    // ���������� "����������" �����, ���� �� �������
    DataSettings *GetOpenedDataSettings();

    uint8* GetData(Chan, int fromEnd);

    // �������� �������������� ����� ������� 0 - �����, 1 - ������.
    uint8* GetLimitation(Chan, int direction);

    int NumberAvailableEntries();
};
