#pragma once
#include "Display/DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Display/Colors.h"
#include "Utils/Text/String.h"


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

    static void Set(E);
};


struct Filtr
{
    // Включить/выключить фильтр на входе канала.
    static void Enable(Chan, bool);
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
    static void Set(Chan);
};


// Режим канала по входу.
struct ModeCouple
{
    enum E
    {
        DC,      // Открытый вход.
        AC,      // Закрытый вход.
        GND,     // Вход заземлён.
        Count
    };

    // Установить режим канала по входу.
    static void Set(Chan, ModeCouple::E modeCoupe);
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

    static const E MIN_PEC_DEAT = _1us;     // Минимальный масштаб по времени, при котором возможно включение режима
                                            // пикового детектора.
    static const E MIN_P2P      = _50ms;    // С этого значения tbase должен включаться режим поточечного вывода.
    static const E MAX_RAND     = _50ns;    // Максимальная развёртка, при которой ещё действует режим рандомизатора
};


struct TShift
{
    static const int ERROR = 1000000;
    static const int MAX = 16383;           // Максимально значение задержки по времени относительно точки синхронизации,
                                            // засылаемое в альтеру.

    // Установить относительное смещение по времени.
    static void Set(int);

    // Возвращает установленное смещение по времени в текстовом виде, пригодном для вывода на экран.
    static String<> ToString(int tShiftRel);

    // Смещение по времени, соответствующее позиции TPos.
    static int Zero();

    // Минимальное смещение по времени, которое может быть записано в аппаратную часть.
    static int Min();

    static void Draw();

    static float ToAbs(int shift, TBase::E);

    static int ToRel(float shfit, TBase::E);

    static const float absStep[TBase::Count];

    static int ShiftForRandomizer();
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
    static void Set(Chan, Range::E);

    // Увеличить масштаб по напряжению.
    static bool Increase(Chan);

    // Уменьшить масштаб по напряжению.
    static bool Decrease(Chan);

    // Загрузка масштаба по напряжению в аппаратную часть.
    static void Load(Chan);

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
    static void Set(Chan, int16);

    // Загрузка смещения по напряжению в аппаратную часть.
    static void Load(Chan);

    static int ToRel(float rShiftAbs, Range::E);

    static void ChangedMarkers();

    // Отключить вспомогательную линию маркера смещения по напряжению
    static void DisableShowLevel(Chan);

    static void OnMarkersAutoHide();

    static void Draw();

    // Нарисовать маркеры смещения по напряжению
    static void Draw(Chan);

    float ToAbs(Range::E);

    String<> ToString(Range::E, Divider::E);

    operator int16() const { return value; }

    RShift &operator=(int16 rshift) { value = rshift; return *this; }

    int16 value;

    static const float absStep[Range::Count];

    static bool showLevel[2];               // Нужно ли рисовать горизонтальную линию уровня смещения первого канала
    static bool drawMarkers;
};


struct TrigLev
{
    TrigLev(int16 v = ZERO) : value(v) {}

    static const int16 MIN = RShift::MIN;   // Минимальное значение уровня синхронизации, засылаемое в прибор.
    static const int16 ZERO = RShift::ZERO; // Нулевое значение уровня синхронизации, засылаемое в прибор. Маркер
                                            // синхронизации при этом находитися на одном уровне с маркером смещения по напряжению.
    static const int16 MAX = RShift::MAX;   // Максимальное значечение уровня синхронизации, засылаемое в аналоговую часть.

    // Установить относительный уровень синхронизации.
    static void Set(TrigSource::E, int16);
    static void Set(Chan, int16);

    // Загрузка уровня синхронизации в аппаратную часть.
    static void Load();

    static void DisableShowLevel();

    // Найти и установить уровень синхронизации по последнему считанному сигналу
    static void FindAndSet();

    // Нарисовать маркер уровня синхронизации.
    static void Draw();

    float ToAbs(Range::E);

    operator int16() const { return value; }

    TrigLev &operator=(int16 trigLev) { value = trigLev; return *this; }

    int16 value;

    static bool showLevel;      // Нужно ли рисовать горизонтальную линию уровня смещения уровня синхронизации

    static bool fireLED;        // Если горит лампочка
};
