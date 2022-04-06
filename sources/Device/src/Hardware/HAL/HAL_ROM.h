#pragma once
#include "Data/Data.h"
#include "Settings/Settings.h"


namespace HAL_ROM
{
    namespace Data
    {
        static const int MAX_DATAS = 23;            // Пока ограничено количеством квадратиков, которые можно вывести в одну линию внизу сетки

        void Save(int num, const DataStruct &);

        bool Get(int num, DataStruct &);

        // Если даннные есть, соответствующий элемент массива равен true/.
        void GetInfo(bool existData[MAX_DATAS]);

        void Delete(int num);

        void EraseAll();
    }

    namespace Settings
    {
        // Сохранить настройки
        void Save(::Settings *, bool verifyLoaded = false);
        bool Load(::Settings *);

        // Сохранить отладочные настройки
        void SaveNRST(SettingsNRST *);
        bool LoadNRST(SettingsNRST *);
    }
}


namespace OTP
{
    bool SaveSerialNumber(char string[20]);

    int GetSerialNumber(char buffer[20]);
}
