#pragma once
#include "Display/DisplayTypes.h"


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
};


struct Filtr
{
    // ��������/��������� ������ �� ����� ������.
    static void Enable(Chan::E, bool);
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
};


// ����� ������ �� �����.
struct ModeCouple
{
    enum E
    {
        DC,      // �������� ����.
        AC,      // �������� ����.
        GND      // ���� �������.
    };

    // ���������� ����� ������ �� �����.
    static void Set(Chan::E, ModeCouple::E modeCoupe);
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

    static const E MIN_PEC_DEAT = _500ns;   // ����������� ������� �� �������, ��� ������� �������� ��������� ������ �������� ���������.
    static const E MIN_P2P      = _20ms;    // � ����� �������� tBase ������ ���������� ����� ����������� ������.
    static const E MAX_RAND     = _20ns;    // ������������ ��������, ��� ������� ��� ��������� ����� �������������
};


struct TShift
{
    static const int EMPTY = 1000000;
    static const int MAX = 16383;       //  ����������� �������� �������� �� ������� ������������ ����� �������������, ���������� � �������.

    // ���������� ������������� �������� �� �������.
    static void Set(int);

    // ���������� ���������� �������� �� ������� ��� ������ �������������. � ������ �������� ��� �������� ������ ���� ������.
    static void SetDelta(int);

    // ���������� ������������� �������� �� ������� � ��������� ����, ��������� ��� ������ �� �����.
    static pchar ToString(int tShiftRel, char buffer[20]);

    // �������� �� �������, ��������������� ������� TPos.
    static int Zero();

    // ����������� �������� �� �������, ������� ����� ���� �������� � ���������� �����.
    static int Min();

    static void Draw();

    static float ToAbs(int shift, TBase::E);

    static int ToRel(float shfit, TBase::E);

    static const float absStep[TBase::Count];
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
    static void Set(Chan::E, Range::E);

    // ��������� ������� �� ����������.
    static bool Increase(Chan::E);

    // ��������� ������� �� ����������.
    static bool Decrease(Chan::E);

    // �������� �������� �� ���������� � ���������� �����.
    static void Load(Chan::E);

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
    static void Set(Chan::E, int16);

    // �������� �������� �� ���������� � ���������� �����.
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

    static const int16 MIN = RShift::MIN;     // ����������� �������� ������ �������������, ���������� � ������.
    static const int16 ZERO = RShift::ZERO;   // ������� �������� ������ �������������, ���������� � ������. ������ ������������� ��� ���� ���������� �� ����� ������ � �������� �������� �� ����������.
    static const int16 MAX = RShift::MAX;     // ������������ ���������� ������ �������������, ���������� � ���������� �����.

    // ���������� ������������� ������� �������������.
    static void Set(TrigSource::E, int16);

    // �������� ������ ������������� � ���������� �����.
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
    void*               addrNext;                   // ����� ��������� ������.
    void*               addrPrev;                   // ����� ���������� ������.
    uint                rShiftA             : 10;   // �������� �� ����������
    uint                rShiftB             : 10;
    uint                trigLevA            : 10;   // ������� �������������
    int                 tShift;                     // �������� �� �������
    ModeCouple::E       coupleB             : 2;
    Range::E            range[2];                   // ������� �� ���������� ����� �������.

    uint                trigLevB            : 10;
    ENUM_POINTS_FPGA::E e_points_in_channel : 2;    // ����� � ������
    TBase::E            tBase               : 5;    // ������� �� �������
    ModeCouple::E       coupleA             : 2;    // ����� ������ �� �����
    uint                peakDet             : 2;    // ������� �� ������� ��������
    uint                en_a                : 1;    // ������� �� ����� 0
    uint                en_b                : 1;    // ������� �� ����� 1

    uint                inv_a               : 1;
    uint                inv_b               : 1;
    Divider::E          div_a               : 1;
    Divider::E          div_b               : 1;
    PackedTime          time;
    // ���������� �����
    int16               rec_point;                  // ������ ����� �������� ��� �����. ���� -1 - �� ����� �� ����������. �� ������ ���������

    bool Equal(const DataSettings &);

    void PrintElement();

    // ���������, ������� ������ ���������, ����� ��������� ��������� � ����������� dp
    int SizeElem();

    // ���������� ����� �������� �����������
    void Init();

    int BytesInChannel() const;

    int PointsInChannel() const;

    int16 GetRShift(Chan::E) const;

    bool InModeP2P() const;

    // �������� ����� � ���������� ������
    void AppendPoints(uint8 *a, uint8 *b, BitSet16 pointsA, BitSet16 pointsB);
};
