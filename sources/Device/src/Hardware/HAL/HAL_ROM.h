#pragma once
#include "Data/Data.h"


#define MAX_NUM_SAVED_WAVES 23  // ���� ���������� ����������� �����������, ������� ����� ������� � ���� ����� ����� �����

namespace HAL_ROM
{
    void EraseData();

    bool GetData(int num, DataFrame &);

    bool LoadSettings();

    void SaveSettings();

    void DeleteData(int num);

    void SaveData(int num, DataStruct &);

    void SaveData(int num, const DataFrame &);

    // ���� ������� ����, ��������������� ������� ������� ����� true/.
    void GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES]);
}


namespace OTP
{
    bool SaveSerialNumber(char string[20]);

    int GetSerialNumber(char buffer[20]);
}