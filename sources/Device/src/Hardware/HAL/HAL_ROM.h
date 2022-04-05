#pragma once
#include "Data/Data.h"
#include "Settings/SettingsNRST.h"


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
        void Save(bool verifyLoaded = false);

        // ��������� �������������� ���������
        void SaveNRST(SettingsNRST *);

        bool Load();

        bool LoadNRST(SettingsNRST *);
    }
}


namespace OTP
{
    bool SaveSerialNumber(char string[20]);

    int GetSerialNumber(char buffer[20]);
}
