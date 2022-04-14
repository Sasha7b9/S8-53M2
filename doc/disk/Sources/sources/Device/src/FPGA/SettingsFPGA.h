#pragma once
#include "Display/DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Display/Colors.h"
#include "Utils/Text/String.h"


// ����� ����� �������, � ������� ��� ������.
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


// ����� ������ �������� ���������.
struct PeackDetMode
{
    enum E
    {
        Disable,
        Enable
    };

    // ��������/��������� ����� �������� ���������.
    static void Set(PeackDetMode::E);
};


// �������� ������������� � ������.
struct TPos
{
    enum E
    {
        Left,      // ������������� ��������� � ������ ������.
        Center,    // ������������� ��������� � ������ ������.
        Right,     // ������������� ��������� � ����� ������.
        Count
    };

    // ������ �������� ���������� ������ ������������� � ������.
    static int InPoints(ENUM_POINTS_FPGA::E, TPos::E);

    static void Set(E);
};


struct Filtr
{
    // ��������/��������� ������ �� ����� ������.
    static void Enable(Chan, bool);
};


// ���� �������������.
struct TrigInput
{
    enum E
    {
        Full,             // ������ ������.
        AC,               // ����������.
        LPF,              // ���.
        HPF               // ���.
    };

    // ���������� ����� ����� �������������.
    static void Set(TrigInput::E trigInput);
};


// ��� ������������
struct TrigPolarity
{
    enum E
    {
        Front,         // ������������� �� ������.
        Back           // ������������� �� �����.
    };

    // ���������� ���������� �������������.
    static void Set(TrigPolarity::E);

    // ������� ���������� ������������� � ���������� �����.
    static void Load();
};


// �������� �������������.
struct TrigSource
{
    enum E
    {
        ChannelA,        // ����� 1.
        ChannelB,        // ����� 2.
        Ext              // �������.
    };

    // ���������� �������� �������������.
    static void Set(TrigSource::E);
    static void Set(Chan);
};


// ����� ������ �� �����.
struct ModeCouple
{
    enum E
    {
        DC,      // �������� ����.
        AC,      // �������� ����.
        GND,     // ���� �������.
        Count
    };

    // ���������� ����� ������ �� �����.
    static void Set(Chan, ModeCouple::E modeCoupe);
};


// ����� ������ �����������.
struct CalibratorMode
{
    enum E
    {
        Freq,            // �� ������ ����������� 4�, 1���.
        DC,              // �� ������ ����������� 4� ����������� ����������.
        GND              // �� ������ ����������� 0�.
    };

    // ��������/��������� ����������.
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

    // ��������� ������� �� �������.
    static void Decrease();

    // ��������� ������� �� �������.
    static void Increase();

    // �������� ����������� �������� � ���������� �����.
    static void Load();

    static bool InModeRandomizer();

    static bool InModeP2P();

    // ����������� �������� � ������������� (��� ����� ������� ��� ������������� ��������)
    static int StretchRand();

    static pchar ToString(TBase::E);
    static pchar ToStringEN(TBase::E);

    static const E MIN_PEC_DEAT = _1us;     // ����������� ������� �� �������, ��� ������� �������� ��������� ������
                                            // �������� ���������.
    static const E MIN_P2P      = _50ms;    // � ����� �������� tbase ������ ���������� ����� ����������� ������.
    static const E MAX_RAND     = _50ns;    // ������������ ��������, ��� ������� ��� ��������� ����� �������������
};


struct TShift
{
    static const int ERROR = 1000000;
    static const int MAX = 16383;           // ����������� �������� �������� �� ������� ������������ ����� �������������,
                                            // ���������� � �������.

    // ���������� ������������� �������� �� �������.
    static void Set(int);

    // ���������� ������������� �������� �� ������� � ��������� ����, ��������� ��� ������ �� �����.
    static String<> ToString(int tShiftRel);

    // �������� �� �������, ��������������� ������� TPos.
    static int Zero();

    // ����������� �������� �� �������, ������� ����� ���� �������� � ���������� �����.
    static int Min();

    static void Draw();

    static float ToAbs(int shift, TBase::E);

    static int ToRel(float shfit, TBase::E);

    static const float absStep[TBase::Count];

    static int ShiftForRandomizer();
};


// ������� �� ����������.
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

    // ���������� ������� �� ����������.
    static void Set(Chan, Range::E);

    // ��������� ������� �� ����������.
    static bool Increase(Chan);

    // ��������� ������� �� ����������.
    static bool Decrease(Chan);

    // �������� �������� �� ���������� � ���������� �����.
    static void Load(Chan);

    static pchar ToName(Range::E);

    static pchar ToString(Range::E, Divider::E);

    // ���������� ������������� ������� ������� �����
    static float MaxOnScreen(Range::E);

    // ������� ����� � 1/250 �������� �������, ����������� �� ������ �����
    static const float voltsInPoint[Range::Count];

    static const float scale[Range::Count];
};

Range::E &operator++(Range::E &range);
Range::E &operator--(Range::E &range);


struct RShift
{
    RShift(int16 v = ZERO) : value(v) {}

    static const int16 MIN = 20;    // ����������� �������� �������� ������ �� ����������, ���������� � ���������� �����. ������������� �������� 10 ������ ���� �� ����������� �����.
    static const int16 ZERO = 500;  // ������� �������� �������� ������ �� ����������, ���������� � ���������� �����. ������������� ������������ ������ �� ������ ������.
    static const int16 MAX = 980;   // ������������ �������� �������� ������ �� ����������, ���������� � ���������� �����. ������������� �������� 10 ������ ����� �� ����������� �����.

    static const int16 STEP = (((RShift::MAX - RShift::MIN) / 24) / GRID_DELTA);   // �� ������� ������ ����� �������� �������� ��������, ����� ������ �������� �� ���������� ������������ �� ���� �����.

    // ���������� ������������� �������� �� ����������.
    static void Set(Chan, int16);

    // �������� �������� �� ���������� � ���������� �����.
    static void Load(Chan);

    static int ToRel(float rShiftAbs, Range::E);

    static void ChangedMarkers();

    // ��������� ��������������� ����� ������� �������� �� ����������
    static void DisableShowLevel(Chan);

    static void OnMarkersAutoHide();

    static void Draw();

    // ���������� ������� �������� �� ����������
    static void Draw(Chan);

    float ToAbs(Range::E);

    String<> ToString(Range::E, Divider::E);

    operator int16() const { return value; }

    RShift &operator=(int16 rshift) { value = rshift; return *this; }

    int16 value;

    static const float absStep[Range::Count];

    static bool showLevel[2];               // ����� �� �������� �������������� ����� ������ �������� ������� ������
    static bool drawMarkers;
};


struct TrigLev
{
    TrigLev(int16 v = ZERO) : value(v) {}

    static const int16 MIN = RShift::MIN;   // ����������� �������� ������ �������������, ���������� � ������.
    static const int16 ZERO = RShift::ZERO; // ������� �������� ������ �������������, ���������� � ������. ������
                                            // ������������� ��� ���� ���������� �� ����� ������ � �������� �������� �� ����������.
    static const int16 MAX = RShift::MAX;   // ������������ ���������� ������ �������������, ���������� � ���������� �����.

    // ���������� ������������� ������� �������������.
    static void Set(TrigSource::E, int16);
    static void Set(Chan, int16);

    // �������� ������ ������������� � ���������� �����.
    static void Load();

    static void DisableShowLevel();

    // ����� � ���������� ������� ������������� �� ���������� ���������� �������
    static void FindAndSet();

    // ���������� ������ ������ �������������.
    static void Draw();

    float ToAbs(Range::E);

    operator int16() const { return value; }

    TrigLev &operator=(int16 trigLev) { value = trigLev; return *this; }

    int16 value;

    static bool showLevel;      // ����� �� �������� �������������� ����� ������ �������� ������ �������������

    static bool fireLED;        // ���� ����� ��������
};
