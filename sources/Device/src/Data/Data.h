// 2022/03/16 08:24:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "FPGA/SettingsFPGA.h"
#include "Utils/Containers/Buffer.h"


struct PackedTime
{
    uint hours   : 5;
    uint minutes : 6;
    uint seconds : 6;
    uint year    : 7;
    uint month   : 4;
    uint day     : 5;
};


struct DataSettings
{
    void                *next;                      // Адрес следующей записи.
    void                *prev;                      // Адрес предыдущей записи.
    uint                rShiftA             : 10;   // Смещение по напряжению
    uint                rShiftB             : 10;
    uint                trigLevA            : 10;   // Уровень синхронизации
    int                 tShift;                     // Смещение по времени
    ModeCouple::E       coupleB             : 2;
    Range::E            range[2];                   // Масштаб по напряжению обоих каналов.

    uint                trigLevB            : 10;
    ENUM_POINTS_FPGA::E e_points_in_channel : 2;    // Точек в канале
    TBase::E            tBase               : 5;    // Масштаб по времени
    ModeCouple::E       coupleA             : 2;    // Режим канала по входу
    uint                peakDet             : 2;    // Включен ли пиковый детектор
    uint                inv_a               : 1;
    uint                inv_b               : 1;
    Divider::E          div_a               : 1;
    Divider::E          div_b               : 1;
    uint                valid               : 1;
    PackedTime          time;

    DataSettings() { valid = 0; };

    void Set(DataSettings &ds) { *this = ds; valid = 1; }

    bool Equal(const DataSettings &);

    void PrintElement();

    // Вычисляет, сколько памяти трубуется, чтобы сохранить измерения с настройками dp
    int SizeFrame();

    // Заполнение полей текущими настройками
    void Init();

    int BytesInChannel() const;

    int PointsInChannel() const;

    int16 GetRShift(Chan) const;

    bool Valid() const { return (valid == 1); };
};


struct BufferFPGA : public Buffer<uint8>
{
    BufferFPGA(int size = 0) : Buffer<uint8>(size) { }

    // Инвертирует данные.
    void InverseData();
};


// Структура хранит в себе полные данные без привязки к хранилищу
struct DataStruct
{
    BufferFPGA   A;
    BufferFPGA   B;
    DataSettings ds;

    int rec_point;
    int all_points;

    DataStruct() : rec_point(0), all_points(0) { }

    BufferFPGA &Data(Chan ch) { return ch.IsA() ? A : B; }

    bool Valid() const { return ds.Valid(); }

    void AppendPoints(BitSet16 pointsA, BitSet16 pointsB);

    void Log(pchar point);
};


// Структура хранит в себе ссылки на данные, хранящиеся в хранилище. Их нельзя изменять.
// Это такая обёртка
struct DataFrame
{
    const DataSettings *ds;

    DataFrame(DataSettings *_ds) : ds(_ds) { };

    // Начало данных канала
    const uint8 *DataBegin(Chan);

    // Конец данных канала
    const uint8 *DataEnd(Chan);

    // Заполнить фрейм ds из DataStruct
    void FillDataFromStruct(DataStruct &);
};


namespace Data
{
    extern DataStruct dir;             // Рисуемый сигнал
    extern DataStruct last;
    extern DataStruct ins;              // Здесь данные из ППЗУ, которые должны выводиться на экран
}
