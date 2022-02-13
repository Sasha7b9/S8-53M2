// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"


namespace Storage
{
    // Удаление всех сохранённых измерений
    void Clear();

    // Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
    void AddData(uint8 *data0, uint8 *data1, DataSettings dss);

    // Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
    int NumElementsWithSameSettings();

    // Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора
    int NumElementsWithCurrentSettings();

    // Возвращает количество сохранённых измерений
    int NumElementsInStorage();

    // Получить указатель на данные
    bool GetDataFromEnd(int fromEnd, DataSettings **ds, uint8 **data0, uint8 **data1);

    uint8* GetData(Chan::E, int fromEnd);

    // Получить усреднённые данные по нескольким измерениям.
    uint8* GetAverageData(Chan::E);

    // Сколько всего измерений сохранено в памяти.
    int AllDatas();

    // Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху.
    uint8* GetLimitation(Chan::E, int direction);                                      

    int NumberAvailableEntries();
};
