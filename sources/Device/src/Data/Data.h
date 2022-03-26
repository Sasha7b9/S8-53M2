// 2022/03/16 08:24:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "FPGA/SettingsFPGA.h"
#include "Utils/Containers/Buffer.h"


struct DataStruct;


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
    uint                peak_det            : 2;    // Включен ли пиковый детектор
    uint                inv_a               : 1;
    uint                inv_b               : 1;
    Divider::E          div_a               : 1;
    Divider::E          div_b               : 1;
    uint                valid               : 1;
    PackedTime          time;

    DataSettings() { FillFromCurrentSettings(); valid = 0; };

    // Заполнение полей текущими настройками
    void FillFromCurrentSettings();

    void Set(const DataSettings &);

    bool Equal(const DataSettings &);

    void PrintElement();

    // Вычисляет, сколько памяти трубуется, чтобы сохранить измерения с настройками dp
    int SizeFrame() const;

    // Реальное количество байт на канал.
    int BytesInChanReal() const;

    // Количество байт, каторое занимает каждый канал при хранении в Storage. Оно должно быть кратным четырём.
    int BytesInChanStored() const;

    int PointsInChannel() const;

    int16 GetRShift(Chan) const;
};


struct BufferFPGA : public Buffer<uint8>
{
    BufferFPGA(int size = 0) : Buffer<uint8>(size) { }
};


// Структура хранит в себе ссылки на данные, хранящиеся в хранилище. Их нельзя изменять.
// Это такая обёртка
struct DataFrame
{
    DataSettings *ds;

    static int rec_points;
    static int all_points;

    DataFrame(DataSettings *_ds = nullptr) : ds(_ds) { };

    uint8 *BeginFrame();

    uint8 *EndFrame();

    // Начало данных канала
    uint8 *DataBegin(Chan) const;

    // Конец данных канала
    const uint8 *DataEnd(Chan);

    // Взять данные канала из BufferFPGA
    void FillDataChannelFromBuffer(Chan, BufferFPGA &);

    // Заполнить фрейм ds из DataStruct
    void FillDataChannelsFromStruct(DataStruct &);

    // Заполнить фрейм данными из фрейма
    void FillDataChannelsFromFrame(DataFrame &);
};


// Структура хранит в себе полные данные без привязки к хранилищу
struct DataStruct
{
    BufferFPGA   A;
    BufferFPGA   B;
    DataSettings ds;

    int rec_points;         // Эти точки сейчас будут записываться. Не может быть больше количества байт в канале
    int all_points;         // Всего считано точек
    bool mode_p2p;          // Выводить ли его как поточечный фрейм

    DataStruct() : rec_points(0), all_points(0), mode_p2p(false) { }

    DataStruct(const DataFrame &);

    BufferFPGA &Data(Chan ch) { return ch.IsA() ? A : B; }

    // Подготовить данные поточечного режима к выводу в поточечном режиме
    // Возвращает позицию точки (начиная с левой границы сетки), в которой нужно выводить вертикальную линию
    int PrepareForNormalDrawP2P();

    void Log(pchar point);
};


namespace Data
{
    extern DataFrame last;      // Здесь сигнал из ПАМЯТЬ-Последние, если нужно для отображения
    extern DataFrame ins;       // Здесь сигнал из ППЗУ, если нужно для отображения 
}
