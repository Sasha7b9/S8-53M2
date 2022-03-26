#pragma once
#include "Data/Data.h"


namespace HAL_ROM
{
    namespace Data
    {
        static const int MAX_DATAS = 23;            // ���� ���������� ����������� �����������, ������� ����� ������� � ���� ����� ����� �����

        void EraseAll();

        bool Get(int num, DataStruct &);

        void Delete(int num);

        void Save(int num, DataStruct &);

        // ���� ������� ����, ��������������� ������� ������� ����� true/.
        void GetInfo(bool existData[MAX_DATAS]);
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
