#pragma once
#include "Data/Data.h"
#include "Settings/Settings.h"


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
        // ��������� ���������
        bool Save(::Settings *);
        bool Load(::Settings *);

        // ��������� ���������� ���������
        bool SaveNRST(SettingsNRST *);
        bool LoadNRST(SettingsNRST *);
    }
}


namespace OTP
{
    bool SaveSerialNumber(int number, int year);

    // ���������� ���������� ��������� ���� ��� ����������
    int GetSerialNumber(int *number, int *year);
}
