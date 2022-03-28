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
#define MODE_ACCUM_IS_RESET         (MODE_ACCUM == ModeAccumulation::Reset)
#define MODE_ACCUM_IS_NORESET       (MODE_ACCUM == ModeAccumulation::NoReset)

#define ALT_MARKERS                 (set.display.altMarkers)
#define ALT_MARKERS_HIDE            (ALT_MARKERS == AltMarkers::Hide)

#define MENU_AUTO_HIDE              (set.display.menuAutoHide)

#define SHOW_STRING_NAVIGATION      (set.display.showStringNavigation)

#define LINKING_RSHIFT              (set.display.linkingRShift)
#define LINKING_RSHIFT_IS_VOLTAGE   (LINKING_RSHIFT == LinkingRShift::Voltage)



 // ����� ��������� �������.
struct ModeDrawSignal
{
    enum E
    {
        Lines = 0,   // ������ �������� �������.
        Points = 1   // ������ �������� �������.
    };
};

// ��� ����� �� ������.
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

// ������������ ����������.
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

    static int number_drawing;      // ����� ��� ������������ ��������
};

// ���������� ���������� �� ����������.
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
        _512,
        Count
    };

    static int ToNumber();
};

// ��� ���������� �� ����������.
struct ModeAveraging
{
    enum E
    {
        Accurately,   // ��������� �����.
        Around,       // ��������� ��������������.
        Count
    };
};

// ���������� ��������� ��� ������� ������������ � ������������� ��������.
struct ENumMinMax
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
        Count
    };
};

// ���������� ����� ��� ������� ����������� �������.
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

    // ���������� ����� ����� ������������� ������� (����� ������� - �����������).
    static int ToPoints();
};

// ����������� FPS.
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

// ����� ����������.
struct ModeAccumulation
{
    enum E
    {
        NoReset,   // � ���� ������ ������������ ������ N ��������� ���������.
        Reset      // � ���� ������ ���������� N ��������� ��������� � ����� ������������.
    };
};

// ����� ����������� �������������� ������� �������� �������� �� ����������.
struct AltMarkers
{
    enum E
    {
        Hide,        // ������ �� ��������.
        Show,        // ������ ��������.
        AutoHide     // �������� � ������� ����� timeShowLevels.
    };
};

// ����� ����� ����� ����� ���������� ������� ������ �������� ����.
struct MenuAutoHide
{
    enum E
    {
        None = 0,  // �������.
        _5 = 5,     // ����� 5 ������.
        _10 = 10,   // ����� 10 ������.
        _15 = 15,   // ����� 15 ������.
        _30 = 30,   // ����� 30 ������.
        _60 = 60    // ����� 60 ������.
    };
};

// ��� �������� � �������� �� ���������
struct LinkingRShift
{
    enum E
    {
        Voltage,      // �������� ��������� � ����������
        Position      // �������� ���������� � ������� �� ������
    };
};



// ��������� �����������
struct SettingsDisplay
{
    int16               timeShowLevels;         // �����, � ������� �������� ����� ���������� ������ ��������.
    int16               shiftInMemory;          // ���������� �������� ������ ���� ����� ������������ ��������
                                                // ����� ������. ����� ��� ����������� ����������� ������� � ����.
    int16               timeMessages;           // ����� � ��������, �� ������� ��������� ������� �� ������.
    int16               brightness;             // ������� �������.
    int16               brightnessGrid;         // ������� ����� �� 0 �� 100.
    uint                colors[16];             // �����.
    ModeDrawSignal::E   modeDrawSignal;         // ����� ��������� �������.
    TypeGrid::E         typeGrid;               // ��� �����.
    ENumAccumulation::E enumAccumulation;       // ������������ ���������� ������� �� ������.
    ENumAveraging::E    enumAve;                // ������������ ���������� �������.
    ModeAveraging::E    modeAve;                // ��� ���������� �� ����������.
    ENumMinMax::E       enumMinMax;             // ����� ��������� ��� ����������� ��������� � ����������.
    Smoothing::E        smoothing;              // ����� ����� ��� ����������� �������.
    ENumSignalsInSec::E enumSignalsInSec;       // ������������ ���������� ������� � �������.
    Chan                lastAffectedChannel;    // ����� �������� ����� ���������� ������, ������� ��������� �������.
                                                // ����� ��� ����, ����� �����, ����� ������ �������� �������.
    ModeAccumulation::E modeAccumulation;       // ����� ����� ���������� ��������.
    AltMarkers::E       altMarkers;             // ����� ����������� �������������� ������� �������� ��������.
    MenuAutoHide::E     menuAutoHide;           // ����� ������� ������� ����� ���������� ������� ������� ������� ����.
    bool                showFullMemoryWindow;   // ���������� �� ���� ������ ������ ������. \todo �� ������������.
    bool                showStringNavigation;   // ���������� �� ������ �������� ��������� ����..
    LinkingRShift::E    linkingRShift;          // ��� �������� �������� �� ���������.

    // ���������� index ������ � ��������� ����� �� ������.
    static BitSet32 PointsOnDisplay();

    // ���������� �����, ����� ������� ���� ������������� ����������, ���� �� ���� ������ �������.
    static int TimeMenuAutoHide();

    // ���� ����� ������� �� ��� ����� � �������� ������ ��������� ������ - ��������, � ������ ������ �������
    static bool IsSeparate();
};
