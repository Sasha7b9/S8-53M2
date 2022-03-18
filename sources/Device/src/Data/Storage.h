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

    // Создать фрейм с текущими настройками для считывания в него данных целиком. Если находимся в режиме рандомизатора,
    // нужно заполнить его значениями по умолчанию
    void CreateFrame();

    // После записи данные в поля фрейма нужно вызвать эту функцию
    void CloseFrame();

    // Создать новый фрейм поточечного вывода.
    void CreateFrameP2P();

    // Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
    void AddData(DataStruct &);

    // Сколько всего измерений сохранено в памяти.
    int NumFrames();

    // Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
    int NumFramesWithSameSettings();

    // Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора
    int NumFramesWithCurrentSettings();

    // Получить указатель на данные
    bool GetData(int fromEnd, DataStruct &);

    // Возвращает указатель на данные, отстоящие на fromEnd oт последнего сохранённого
    DataSettings *GetDataSettings(int fromEnd);

    uint8* GetData(Chan, int fromEnd);

    // Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху.
    uint8* GetLimitation(Chan, int direction);

    int NumberAvailableEntries();
};
