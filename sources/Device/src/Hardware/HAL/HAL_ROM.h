#pragma once
#include "Data/Data.h"


namespace HAL_ROM
{
    namespace Data
    {
        static const int MAX_DATAS = 23;            // ���� ���������� ����������� �����������, ������� ����� ������� � ���� ����� ����� �����

        void Save(int num, const DataStruct &);

        bool Get(int num, DataStruct &);

        // ���� ������� ����, ��������������� ������� ������� ����� true/.
        void GetInfo(bool existData[MAX_DATAS]);

        void Delete(int num);

        void EraseAll();
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
