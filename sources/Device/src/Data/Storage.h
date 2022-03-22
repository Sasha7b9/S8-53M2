// 2022/02/11 17:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Containers/Buffer.h"
#include "Hardware/Timer.h"


struct DataCurrent
{
    DataFrame     frame;
    Buffer<uint8> buffer;       // Здесь, как в Storage, будет храниться сначала DataSettings, а затем последовательно
                                // данные первого и второго каналов

    // Подготовить к новому циклу чтения.
    // В режиме рандомизатора - переписать последние данные из Storage.
    // В режиме поточечного вывода - сбросить точки
    void PrepareForNewCycle();

    void Inverse(Chan);

    void AppendPoints(BitSet16, BitSet16);
};


namespace Storage
{
    extern DataCurrent current;     // Сюда считываем непосредственно из FPGA. Рандомизатор, точки там. Чтобы потом
                                    // переложит в хранилище
    extern TimeMeterMS time_meter;  // Для засекания времени последнего сохранения данных

    // Удаление всех сохранённых измерений
    void Clear();

    // Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
    void AddData(DataFrame &);

    // Сколько всего измерений сохранено в памяти.
    int NumFrames();

    // Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
    int NumFramesWithSameSettings();

    // Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора
    int NumFramesWithCurrentSettings();

    // Получить указатель на данные
    DataFrame GetData(int fromEnd);

    // Возвращает указатель на данные, отстоящие на fromEnd oт последнего сохранённого
    DataSettings *GetDataSettings(int fromEnd);

    uint8* GetData(Chan, int fromEnd);

    int NumberAvailableEntries();
};
