#pragma once
#include "Display/DisplayTypes.h"


// Число точек сигнала, с которым идёт работа.
struct ENUM_POINTS_FPGA
{
    enum E
    {
        _281,
        _512,
        _1024,
        Count
    };

    static int ToNumPoints();
    static int ToNumBytes();
};


// Режим работы пикового детектора.
struct PeackDetMode
{
    enum E
    {
        Disable,
        Enable
    };

    // Включить/выключить режим пикового детектора.
    static void Set(PeackDetMode::E);
};


// Привязка синхронизации к памяти.
struct TPos
{
    enum E
    {
        Left,      // Синхронизация привязана к началу памяти.
        Center,    // Синхронизация привязана к центру памяти.
        Right,     // Синхронизация привязана к концу памяти.
        Count
    };

    // Узнать привязку отсительно уровня синхронизации в точках.
    static int InPoints(ENUM_POINTS_FPGA::E, TPos::E);
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


// Режим работы калибратора.
struct CalibratorMode
{
    enum E
    {
        Freq,            // На выходе калибратора 4В, 1кГц.
        DC,              // На выходе калибратора 4В постоянного напряжения.
        GND              // На выходе калибратора 0В.
    };

    // Включить/выключить калибратор.
    static void Set(CalibratorMode::E);
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

    static bool InModeRandomizer();

    static bool InModeP2P();

    // Коэффициент растяжки в рандомизаторе (шаг между точками при установленной развёртке)
    static int StretchRand();

    static pchar ToString(TBase::E);
    static pchar ToStringEN(TBase::E);

    static const E MIN_PEC_DEAT = _500ns;   // Минимальный масштаб по времени, при котором возможно включение режима пикового детектора.
    static const E MIN_P2P      = _20ms;    // С этого значения tBase должен включаться режим поточечного вывода.
    static const E MAX_RAND     = _20ns;    // Максимальная развёртка, при которой ещё действует режим рандомизатора
};


struct TShift
{
    static const int EMPTY = 1000000;
    static const int MAX = 16383;       //  Максимально значение задержки по времени относительно точки синхронизации, засылаемое в альтеру.

    // Установить относительное смещение по времени.
    static void Set(int);

    // Установить добавочное смещение по времени для режима рандомизатора. В каждой развёртке это смещение должно быть разное.
    static void SetDelta(int);

    // Возвращает установленное смещение по времени в текстовом виде, пригодном для вывода на экран.
    static pchar ToString(int tShiftRel, char buffer[20]);

    // Смещение по времени, соответствующее позиции TPos.
    static int Zero();

    // Минимальное смещение по времени, которое может быть записано в аппаратную часть.
    static int Min();

    static void Draw();

    static float ToAbs(int shift, TBase::E);

    static int ToRel(float shfit, TBase::E);

    static const float absStep[TBase::Count];
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

    static pchar ToName(Range::E);

    static pchar ToString(Range::E, Divider::E);

    // Напряжение соответствует верхней границе сетки
    static float MaxOnScreen(Range::E);

    // Столько вольт в 1/250 амлитуды сигнала, растянутого на полный экран
    static const float voltsInPoint[Range::Count];

    static const float scale[Range::Count];
};

Range::E &operator++(Range::E &range);
Range::E &operator--(Range::E &range);


struct RShift
{
    RShift(int16 v = ZERO) : value(v) {}

    static const int16 MIN = 20;    // Минимальное значение смещения канала по напряжению, засылаемое в аналоговую часть. Соответствует смещению 10 клеток вниз от центральной линии.
    static const int16 ZERO = 500;  // Среднее значение смещения канала по напряжению, засылаемое в аналоговую часть. Соответствует расположению марера по центру экрана.
    static const int16 MAX = 980;   // Максимальное значение смещения канала по напряжению, засылаемое в аналоговую часть. Соответствует смещению 10 клеток вверх от центральной лиини.

    static const int16 STEP = (((RShift::MAX - RShift::MIN) / 24) / GRID_DELTA);   // На столько единиц нужно изменить значение смещения, чтобы маркер смещения по напряжению передвинулся на одну точку.

    // Установить относительное смещение по напряжению.
    static void Set(Chan::E, int16);

    // Загрузка смещения по напряжению в аппаратную часть.
    static void Load(Chan::E);

    pchar ToString(Range::E, Divider::E, char buffer[20]);

    static int ToRel(float rShiftAbs, Range::E);

    float ToAbs(Range::E);

    operator int16() const { return value; }

    RShift &operator=(int16 rshift) { value = rshift; return *this; }

    int16 value;

    static const float absStep[Range::Count];
};


struct TrigLev
{
    TrigLev(int16 v = ZERO) : value(v) {}

    static const int16 MIN = RShift::MIN;     // Минимальное значение уровня синхронизации, засылаемое в прибор.
    static const int16 ZERO = RShift::ZERO;   // Нулевое значение уровня синхронизации, засылаемое в прибор. Маркер синхронизации при этом находитися на одном уровне с маркером смещения по напряжению.
    static const int16 MAX = RShift::MAX;     // Максимальное значечение уровня синхронизации, засылаемое в аналоговую часть.

    // Установить относительный уровень синхронизации.
    static void Set(TrigSource::E, int16);

    // Загрузка уровня синхронизации в аппаратную часть.
    static void Load();

    float ToAbs(Range::E);

    operator int16() const { return value; }

    TrigLev &operator=(int16 trigLev) { value = trigLev; return *this; }

    int16 value;
};


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
    void*               addrNext;                   // Адрес следующей записи.
    void*               addrPrev;                   // Адрес предыдущей записи.
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
    PackedTime          time;
    // Поточечный режим
    int16               rec_point;                  // Сейчас будет записана эта точка. Если -1 - то фрейм не поточечный. Он считан полностью

    bool Equal(const DataSettings &);

    void PrintElement();

    // Вычисляет, сколько памяти трубуется, чтобы сохранить измерения с настройками dp
    int SizeElem();

    // Заполнение полей текущими настройками
    void Init();

    int BytesInChannel() const;

    int PointsInChannel() const;

    int16 GetRShift(Chan::E) const;

    bool InModeP2P() const;

    // Добавить точки в поточечном режиме
    void AppendPoints(uint8 *a, uint8 *b, BitSet16 pointsA, BitSet16 pointsB);
};
