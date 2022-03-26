#pragma once
#include "Data/Data.h"


#define MAX_NUM_SAVED_WAVES 23  // ���� ���������� ����������� �����������, ������� ����� ������� � ���� ����� ����� �����

namespace HAL_ROM
{
    namespace Data
    {
        void EraseAll();

        bool Get(int num, DataFrame &);

        void Delete(int num);

        void Save(int num, DataStruct &);

        void Save(int num, const DataFrame &);

        // ���� ������� ����, ��������������� ������� ������� ����� true/.
        void GetInfo(bool existData[MAX_NUM_SAVED_WAVES]);
    }

    namespace Settings
    {
        bool Load();

        void Save(bool verifyLoaded = false);
    }
}


namespace OTP
{
    bool SaveSerialNumber(char string[20]);

    int GetSerialNumber(char buffer[20]);
}
