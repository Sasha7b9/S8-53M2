#pragma once


struct TShift
{
    static const int EMPTY = 1000000;

    // Установить относительное смещение по времени.
    static void Set(int);

    // Установить добавочное смещение по времени для режима рандомизатора. В каждой развёртке это смещение должно быть разное.
    static void SetDelta(int16);

    // Загрузка смещения по времени в аппаратную часть.
    static void Load();

    // Возвращает установленное смещение по времени в текстовом виде, пригодном для вывода на экран.
    static pchar ToString(int16 tShiftRel, char buffer[20]);
};


struct RShift
{
    // Установить относительное смещение по напряжению.
    static void Set(Chan::E, int16);

    // Загрузка смещения по напряжению в аппаратную часть.
    static void Load(Chan::E);
};


struct Filtr
{
    // Включить/выключить фильтр на входе канала.
    static void Enable(Chan::E, bool);
};


// Вход синхронизации.
struct TrigInput
{
    enum E
    {
        Full,             // Полный сиганл.
        AC,               // Переменный.
        LPF,              // ФНЧ.
        HPF               // ФВЧ.
    };

    // Установить режим входа синхронизации.
    static void Set(TrigInput::E trigInput);
};


// Тип синхронизацц
struct TrigPolarity
{
    enum E
    {
        Front,         // Синхронизация по фронту.
        Back           // Синхронизация по срезу.
    };

    // Установить полярность синхронизации.
    static void Set(TrigPolarity::E);

    // Загузка полярности синхронизации в аппаратную часть.
    static void Load();
};


// Источник синхронизации.
struct TrigSource
{
    enum E
    {
        ChannelA,        // Канал 1.
        ChannelB,        // Канал 2.
        Ext              // Внешняя.
    };

    // Установить источник синхронизации.
    static void Set(TrigSource::E);
};


struct TrigLev
{
    // Установить относительный уровень синхронизации.
    static void Set(TrigSource::E, int16);

    // Загрузка уровня синхронизации в аппаратную часть.
    static void Load();
};


// Режим канала по входу.
struct ModeCouple
{
    enum E
    {
        DC,      // Открытый вход.
        AC,      // Закрытый вход.
        GND      // Вход заземлён.
    };

    // Установить режим канала по входу.
    static void Set(Chan::E, ModeCouple::E modeCoupe);
};


struct TBase
{
    enum E
    {
        _2ns,
        _5ns,
        _10ns,
        _20ns,
        _50ns,
        _100ns,
        _200ns,
        _500ns,
        _1us,
        _2us,
        _5us,
        _10us,
        _20us,
        _50us,
        _100us,
        _200us,
        _500us,
        _1ms,
        _2ms,
        _5ms,
        _10ms,
        _20ms,
        _50ms,
        _100ms,
        _200ms,
        _500ms,
        _1s,
        _2s,
        _5s,
        _10s,
        Count
    };

    static void Set(TBase::E);

    // Уменьшить масштаб по времени.
    static void Decrease();

    // Увеличить масштаб по времени.
    static void Increase();

    // Загрузка коэффицента развёртки в аппаратную часть.
    static void Load();
};


// Масштаб по напряжению.
struct Range
{
    enum E
    {
        _2mV,
        _5mV,
        _10mV,
        _20mV,
        _50mV,
        _100mV,
        _200mV,
        _500mV,
        _1V,
        _2V,
        _5V,
        _10V,
        _20V,
        Count
    };

    // Установить масштаб по напряжению.
    static void Set(Chan::E, Range::E);

    // Увеличить масштаб по напряжению.
    static bool Increase(Chan::E);

    // Уменьшить масштаб по напряжению.
    static bool Decrease(Chan::E);

    // Загрузка масштаба по напряжению в аппаратную часть.
    static void Load(Chan::E);
};

Range::E &operator++(Range::E &range);
Range::E &operator--(Range::E &range);


struct PackedTime
{
    uint hours : 5;
    uint minutes : 6;
    uint seconds : 6;
    uint year : 7;
    uint month : 4;
    uint day : 5;
};


struct DataSettings
{
    void*         addrNext;               // Адрес следующей записи.
    void*         addrPrev;               // Адрес предыдущей записи.
    uint          rShiftCh0       : 10;   // Смещение по напряжению
    uint          rShiftCh1       : 10;
    uint          trigLevCh0      : 10;   // Уровень синхронизации
    int16         tShift;                 // Смещение по времени
    ModeCouple::E modeCouple1     : 2;
    Range::E      range[2];               // Масштаб по напряжению обоих каналов.

    uint          trigLevCh1      : 10;
    uint          length1channel  : 11;   // Сколько занимает в байтах длина измерения одного канала
    TBase::E      tBase           : 5;    // Масштаб по времени
    ModeCouple::E modeCouple0     : 2;    // Режим канала по входу
    uint          peakDet         : 2;    // Включен ли пиковый детектор
    uint          enableCh0       : 1;    // Включён ли канал 0
    uint          enableCh1       : 1;    // Включен ли канал 1

    uint          inverseCh0      : 1;
    uint          inverseCh1      : 1;
    Divider       multiplier0     : 1;
    Divider       multiplier1     : 1;
    PackedTime    time;

    void PrintElement();

    // Вычисляет, сколько памяти трубуется, чтобы сохранить измерения с настройками dp
    int SizeElem();

    void FillDataPointer();
};
