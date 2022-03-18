// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Containers/Buffer.h"


// Это рабочий фрейм - сюда будем читать даныне перед тем, как сохранить в Storage
struct WorkingFrame : public DataSettings
{
    // Поточечный режим
    int16  rec_point;   // Сейчас будет записана эта точка. Если -1 - то фрейм не поточечный. Он считан полностью
    int    all_points;  // Всего точек

    BufferFPGA A;
    BufferFPGA B;

    void AppendPoints(BitSet16 pointsA, BitSet16 pointsB);
};


namespace Storage
{
    extern WorkingFrame working;

    // Удаление всех сохранённых измерений
    void Clear();

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
