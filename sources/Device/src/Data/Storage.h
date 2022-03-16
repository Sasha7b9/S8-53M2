// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Containers/Buffer.h"


namespace Storage
{
    // Удаление всех сохранённых измерений
    void Clear();

    // Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
    void AddData(DataSettings, uint8 *dataA, uint8 *dataB);

    // Сколько всего измерений сохранено в памяти.
    int NumElements();

    // Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
    int NumElementsWithSameSettings();

    // Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора
    int NumElementsWithCurrentSettings();

    // Получить указатель на данные
    bool GetData(int fromEnd, DataStruct &);

    uint8* GetData(Chan, int fromEnd);

    // Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху.
    uint8* GetLimitation(Chan, int direction);                                      

    int NumberAvailableEntries();

    namespace P2P
    {
        // Создать новый фрейм поточечного вывода.
        void CreateFrame(const DataSettings &);

        // Сбросить точки текущего фрейма
        void Reset();

        // Добавить точки в текущий фрейм
        void AddPoints(BitSet16 dataA, BitSet16 dataB);
    }
};
