// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "SettingsTypes.h"
#include "Display/Colors.h"


#define TIME_SHOW_LEVELS            (set.display.timeShowLevels)

#define SHIFT_IN_MEMORY             (set.display.shiftInMemory)

#define TIME_MESSAGES               (set.display.timeMessages)

#define BRIGHTNESS                  (set.display.brightness)

#define BRIGHTNESS_GRID             (set.display.brightnessGrid)

#define DISPLAY_COLOR(num)          (set.display.colors[num])

#define MODE_DRAW_SIGNAL            (set.display.modeDrawSignal)
#define MODE_DRAW_IS_SIGNAL_LINES   (MODE_DRAW_SIGNAL == ModeDrawSignal::Lines)

#define TYPE_GRID                   (set.display.typeGrid)
#define TYPE_GRID_IS_1              (TYPE_GRID == TypeGrid::_1)
#define TYPE_GRID_IS_2              (TYPE_GRID == TypeGrid::_2)
#define TYPE_GRID_IS_3              (TYPE_GRID == TypeGrid::_3)

#define ENUM_ACCUM                  (set.display.enumAccumulation)
#define ENUM_ACCUM_IS_NONE          (ENUM_ACCUM == ENumAccumulation::_1)
#define ENUM_ACCUM_IS_INFINITY      (ENUM_ACCUM == ENumAccumulation::Infinity)
#define NUM_ACCUM                   (1 << (int)set.display.enumAccumulation)

#define ENUM_AVE                    (set.display.enumAve)
#define NUM_AVE                     (1 << ENUM_AVE)

#define MODE_AVE                    (set.display.modeAve)

#define ENUM_MIN_MAX                (set.display.enumMinMax)
#define NUM_MIN_MAX                 (1 << ENUM_MIN_MAX)

#define SMOOTHING                   (set.display.smoothing)
#define SMOOTHING_IS_DISABLE        (SMOOTHING == Smoothing::Disable)

#define ENUM_SIGNALS_IN_SEC         (set.display.enumSignalsInSec)

#define LAST_AFFECTED_CHANNEL       (set.display.lastAffectedChannel)
#define LAST_AFFECTED_CHANNEL_IS_A  (LAST_AFFECTED_CHANNEL == Chan::A)
#define LAST_AFFECTED_CHANNEL_IS_B  (LAST_AFFECTED_CHANNEL == Chan::B)

#define MODE_ACCUM                  (set.display.modeAccumulation)
#define MODE_ACCUM_IS_RESET         (MODE_ACCUM == ModeAccumulation_Reset)
#define MODE_ACCUM_IS_NORESET       (MODE_ACCUM == ModeAccumulation_NoReset)

#define ALT_MARKERS                 (set.display.altMarkers)
#define ALT_MARKERS_HIDE            (ALT_MARKERS == AM_Hide)

#define MENU_AUTO_HIDE              (set.display.menuAutoHide)

#define SHOW_STRING_NAVIGATION      (set.display.showStringNavigation)

#define LINKING_RSHIFT              (set.display.linkingRShift)
#define LINKING_RSHIFT_IS_VOLTAGE   (LINKING_RSHIFT == LinkingRShift_Voltage)



 // Режим отрисовки сигнала.
struct ModeDrawSignal
{
    enum E
    {
        Lines = 0,   // Сигнал рисуется линиями.
        Points = 1   // Сигнал рисуется точками.
    };
};

// Тип сетки на экране.
struct TypeGrid
{
    enum E
    {
        _1,
        _2,
        _3,
        _4,
        Count
    };
};

// Перечисление накоплений.
struct ENumAccumulation
{
    enum E
    {
        _1,
        _2,
        _4,
        _8,
        _16,
        _32,
        _64,
        _128,
        Infinity
    };
};

// Количество усреднений по измерениям.
struct ENumAveraging
{
    enum E
    {
        _1,
        _2,
        _4,
        _8,
        _16,
        _32,
        _64,
        _128,
        _256,
        _512
    };
};

// Тип усреднений по измерениям.
struct ModeAveraging
{
    enum E
    {
        Accurately,   // Усреднять точно.
        Around        // Усреднять приблизительно.
    };

    // Возвращает режим усреднения
    static ModeAveraging::E Current();

    // Возвращает количество усреднений
    static int GetNumber();
};

// Количество измерений для расчёта минимального и максимального значений.
enum ENumMinMax
{
    NumMinMax_1,
    NumMinMax_2,
    NumMinMax_4,
    NumMinMax_8,
    NumMinMax_16,
    NumMinMax_32,
    NumMinMax_64,
    NumMinMax_128
};

// Количество точек для расчёта скользящего фильтра.
struct Smoothing
{
    enum E
    {
        Disable,
        _2points = 1,
        _3points = 2,
        _4points = 3,
        _5points = 4,
        _6points = 5,
        _7points = 6,
        _8points = 7,
        _9points = 8,
        _10points = 9
    };

    // Возвращает число точек сглаживающего фильтра (режим ДИСПЛЕЙ - Сглаживание).
    static int ToPoints();
};

// Ограничение FPS.
struct ENumSignalsInSec
{
    enum E
    {
        _25,
        _10,
        _5,
        _2,
        _1
    };

    static ENumSignalsInSec::E FromNum(int num);
    static int ToNum(ENumSignalsInSec::E);
};

// Режим накопления.
enum ModeAccumulation
{
    ModeAccumulation_NoReset,   // В этом режиме показываются строго N последних измерений.
    ModeAccumulation_Reset      // В этом режиме набираются N последних измерений и потом сбрасываются.
};

// Режим отображения дополнительных боковых маркеров смещения по напряжению.
enum AltMarkers
{
    AM_Hide,        // Никода не выводить.
    AM_Show,        // Всегда выводить.
    AM_AutoHide     // Выводить и прятать через timeShowLevels.
};

// Через какое время после последнего нажатия кнопки скрывать меню.
enum MenuAutoHide
{
    MenuAutoHide_None = 0,  // Никогда.
    MenuAutoHide_5 = 5,     // Через 5 секунд.
    MenuAutoHide_10 = 10,   // Через 10 секунд.
    MenuAutoHide_15 = 15,   // Через 15 секунд.
    MenuAutoHide_30 = 30,   // Через 30 секунд.
    MenuAutoHide_60 = 60    // Через 60 секунд.
};

// Тип привязки к смещению по вертикали
enum LinkingRShift
{
    LinkingRShift_Voltage,      // Смещение привязано к напряжению
    LinkingRShift_Position      // Смещение привязаоно к позиции на экране
};



// Настройки изображения
struct SettingsDisplay
{
    int16               timeShowLevels;             // Время, в течение которого нужно показывать уровни смещения.
    int16               shiftInMemory;              // Показывает смещение левого края сетки относительно нулевого байта памяти. Нужно для правильного отображения сигнала в окне.
    int16               timeMessages;               // Время в секундах, на которое сообщение остаётся на экране.
    int16               brightness;                 // Яркость дисплея.
    int16               brightnessGrid;             // Яркость сетки от 0 до 100.
    uint                colors[16];                 // Цвета.
    ModeDrawSignal::E   modeDrawSignal;             // Режим отрисовки сигнала.
    TypeGrid::E         typeGrid;                   // Тип сетки.
    ENumAccumulation::E enumAccumulation;           // Перечисление накоплений сигнала на экране.
    ENumAveraging::E    enumAve;                    // Перечисление усреднений сигнала.
    ModeAveraging::E    modeAve;                    // Тип усреднений по измерениям.
    ENumMinMax          enumMinMax;                 // Число измерений для определения минимумов и максимумов.
    Smoothing::E        smoothing;                  // Число точек для скользящего фильтра.
    ENumSignalsInSec::E enumSignalsInSec;           // Перечисление считываний сигнала в секунду.
    Chan                lastAffectedChannel;        // Здесь хранится номер последнего канала, которым управляли ручками. Нужно для того, чтобы знать, какой сигнал рисовать наверху.
    ModeAccumulation    modeAccumulation;           // Задаёт режим накопления сигналов.
    AltMarkers          altMarkers;                 // Режим отображения дополнительных боковых маркеров смещений.
    MenuAutoHide        menuAutoHide;               // Через сколько времени после последнего нажатия клавиши прятать меню.
    bool                showFullMemoryWindow;       // Показывать ли окно памяти вверху экрана. \todo Не используется.
    bool                showStringNavigation;       // Показывать ли строку текущего состояния меню..
    LinkingRShift       linkingRShift;              // Тип привязки смещения по вертикали.

    // Возвращает index первой и последней точки на экране.
    static BitSet32 PointsOnDisplay();

    // Возвращает время, через которое меню автоматически скрывается, если не было больше нажатий.
    static int TimeMenuAutoHide();

    // Если экран разделён на две части и основной сигнал выводится сверху - например, в режиме вывода спектра
    static bool IsSeparate();
};
