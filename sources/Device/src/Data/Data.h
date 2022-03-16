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
    void                *addrNext;                   // Адрес следующей записи.
    void                *addrPrev;                   // Адрес предыдущей записи.
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
    uint                en_a                : 1;    // Включён ли канал 0
    uint                en_b                : 1;    // Включен ли канал 1

    uint                inv_a               : 1;
    uint                inv_b               : 1;
    Divider::E          div_a               : 1;
    Divider::E          div_b               : 1;
    uint                valid               : 1;
    PackedTime          time;
    // Поточечный режим
    int16               rec_point;                  // Сейчас будет записана эта точка. Если -1 - то фрейм не поточечный. Он считан полностью

    DataSettings() { valid = 0; };

    void Set(DataSettings &ds) { *this = ds; valid = 1; }

    bool Equal(const DataSettings &);

    void PrintElement();

    // Вычисляет, сколько памяти трубуется, чтобы сохранить измерения с настройками dp
    int SizeElem();

    // Заполнение полей текущими настройками
    void Init();

    int BytesInChannel() const;

    int PointsInChannel() const;

    int16 GetRShift(Chan) const;

    bool InModeP2P() const;

    // Добавить точки в поточечном режиме
    void AppendPoints(uint8 *a, uint8 *b, BitSet16 pointsA, BitSet16 pointsB);

    bool Valid() const { return (valid == 1); };
};


struct BufferU8 : public Buffer<uint8>
{
    BufferU8(int size = 0) : Buffer<uint8>(size) { }
};


struct DataStruct
{
    BufferU8     A;
    BufferU8     B;
    DataSettings ds;

    DataStruct() { }

    bool Valid() const { return ds.Valid(); }
};


namespace Data
{
    extern DataStruct dir;             // Рисуемый сигнал
    extern DataStruct last;
    extern DataStruct ins;              // Здесь данные из ППЗУ, которые должны выводиться на экран
}
