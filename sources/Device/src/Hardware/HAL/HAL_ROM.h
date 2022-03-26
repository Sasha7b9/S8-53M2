#pragma once
#include "Data/Data.h"


#define MAX_NUM_SAVED_WAVES 23  // ���� ���������� ����������� �����������, ������� ����� ������� � ���� ����� ����� �����

namespace HAL_ROM
{
    namespace Data
    {
        void EraseData();

        bool GetData(int num, DataFrame &);

        void DeleteData(int num);

        void SaveData(int num, DataStruct &);

        void SaveData(int num, const DataFrame &);

        // ���� ������� ����, ��������������� ������� ������� ����� true/.
        void GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES]);
    }

    bool LoadSettings();

    void SaveSettings();
}


namespace OTP
{
    bool SaveSerialNumber(char string[20]);

    int GetSerialNumber(char buffer[20]);
}