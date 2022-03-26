#pragma once
#include "Data/Data.h"


#define MAX_NUM_SAVED_WAVES 23  // Пока ограничено количеством квадратиков, которые можно вывести в одну линию внизу сетки

namespace HAL_ROM
{
    namespace Data
    {
        void EraseData();

        bool GetData(int num, DataFrame &);

        void DeleteData(int num);

        void SaveData(int num, DataStruct &);

        void SaveData(int num, const DataFrame &);

        // Если даннные есть, соответствующий элемент массива равен true/.
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