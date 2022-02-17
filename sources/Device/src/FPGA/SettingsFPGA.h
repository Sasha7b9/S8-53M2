#pragma once
#include "Display/DisplayTypes.h"


// ����� ������ �������� ���������.
struct PeackDetMode
{
    enum E
    {
        Disable,
        Enable,
        Average
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
        Right      // ������������� ��������� � ����� ������.
    };

    // ������ �������� ���������� ������ ������������� � ������.
    static int InPoints(PeackDetMode::E peakDet, int numPoints, TPos::E tPos);
};


struct TShift
{
    static const int EMPTY = 1000000;
    static const int MAX = 16383;       //  ����������� �������� �������� �� ������� ������������ ����� �������������, ���������� � �������.

    // ���������� ������������� �������� �� �������.
    static void Set(int);

    // ���������� ���������� �������� �� ������� ��� ������ �������������. � ������ �������� ��� �������� ������ ���� ������.
    static void SetDelta(int16);

    // �������� �������� �� ������� � ���������� �����.
    static void Load();

    // ���������� ������������� �������� �� ������� � ��������� ����, ��������� ��� ������ �� �����.
    static pchar ToString(int16 tShiftRel, char buffer[20]);

    // �������� �� �������, ��������������� ������� TPos.
    static int16 Zero();

    // ����������� �������� �� �������, ������� ����� ���� �������� � ���������� �����.
    static int16 Min();

    // �������� �� ������� � ������ ������. �.�. �� ����� � ��� �������� ��� ��� ���������, ��� �������� ���������� ��
    // ����������� � ���������� ����� � ��� ����.
    static int InPoints(PeackDetMode::E peakDet);
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

    static bool InRandomizeMode();

    static pchar ToString(TBase::E);
    static pchar ToStringEN(TBase::E);

    static const E MIN_PEC_DEAT = _500ns;   // ����������� ������� �� �������, ��� ������� �������� ��������� ������ �������� ���������.
    static const E MIN_P2P      = _20ms;    // � ����� �������� tBase ������ ���������� ����� ����������� ������.
    static const E MAX_RAND     = _50ns;    // ������������ ��������, ��� ������� ��� ��������� ����� �������������
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
};

Range::E &operator++(Range::E &range);
Range::E &operator--(Range::E &range);


struct RShift
{
    static const int16 MIN = 20;    // ����������� �������� �������� ������ �� ����������, ���������� � ���������� �����. ������������� �������� 10 ������ ���� �� ����������� �����.
    static const int16 ZERO = 500;  // ������� �������� �������� ������ �� ����������, ���������� � ���������� �����. ������������� ������������ ������ �� ������ ������.
    static const int16 MAX = 980;   // ������������ �������� �������� ������ �� ����������, ���������� � ���������� �����. ������������� �������� 10 ������ ����� �� ����������� �����.

    static const int16 STEP = (((RShift::MAX - RShift::MIN) / 24) / GRID_DELTA);   // �� ������� ������ ����� �������� �������� ��������, ����� ������ �������� �� ���������� ������������ �� ���� �����.

    // ���������� ������������� �������� �� ����������.
    static void Set(Chan::E, int16);

    // �������� �������� �� ���������� � ���������� �����.
    static void Load(Chan::E);

    static pchar ToString(int16 rShiftRel, Range::E, Divider::E, char buffer[20]);
};


struct TrigLev
{
    static const int MIN = RShift::MIN;     // ����������� �������� ������ �������������, ���������� � ������.
    static const int ZERO = RShift::ZERO;   // ������� �������� ������ �������������, ���������� � ������. ������ ������������� ��� ���� ���������� �� ����� ������ � �������� �������� �� ����������.
    static const int MAX = RShift::MAX;     // ������������ ���������� ������ �������������, ���������� � ���������� �����.

    // ���������� ������������� ������� �������������.
    static void Set(TrigSource::E, int16);

    // �������� ������ ������������� � ���������� �����.
    static void Load();
};


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
    void*         addrNext;               // ����� ��������� ������.
    void*         addrPrev;               // ����� ���������� ������.
    uint          rShiftCh0       : 10;   // �������� �� ����������
    uint          rShiftCh1       : 10;
    uint          trigLevCh0      : 10;   // ������� �������������
    int16         tShift;                 // �������� �� �������
    ModeCouple::E modeCouple1     : 2;
    Range::E      range[2];               // ������� �� ���������� ����� �������.

    uint          trigLevCh1      : 10;
    uint          length1channel  : 11;   // ������� �������� � ������ ����� ��������� ������ ������
    TBase::E      tBase           : 5;    // ������� �� �������
    ModeCouple::E modeCouple0     : 2;    // ����� ������ �� �����
    uint          peakDet         : 2;    // ������� �� ������� ��������
    uint          enableCh0       : 1;    // ������� �� ����� 0
    uint          enableCh1       : 1;    // ������� �� ����� 1

    uint          inverseCh0      : 1;
    uint          inverseCh1      : 1;
    Divider::E    multiplier0     : 1;
    Divider::E    multiplier1     : 1;
    PackedTime    time;

    void PrintElement();

    // ���������, ������� ������ ���������, ����� ��������� ��������� � ����������� dp
    int SizeElem();

    void FillDataPointer();
};
