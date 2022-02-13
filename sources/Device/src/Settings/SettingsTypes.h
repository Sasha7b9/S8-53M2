// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// SDisplay


// ����� ����������� ����������� �������� ��� ����������� ������� ������.
enum MissedSignals
{
    Missed_Hide,    // �� �������� �� �����
    Missed_Show,    // �������� �� �����
    Missed_Average  // ���������� � �������� �� �����
};



// ChannelX
struct Chan { enum E {
    A,
    B,
    A_B,
    Math,
    Count = 2
}; };


// ����� ������ �� �����.
enum ModeCouple
{
    ModeCouple_DC,      // �������� ����.
    ModeCouple_AC,      // �������� ����.
    ModeCouple_GND      // ���� �������.
};

// ��������.
enum Divider
{
    Divider_1,
    Divider_10
};

// ������� �� ����������.
enum Range
{
    Range_2mV,
    Range_5mV,
    Range_10mV,
    Range_20mV,
    Range_50mV,
    Range_100mV,
    Range_200mV,
    Range_500mV,
    Range_1V,
    Range_2V,
    Range_5V,
    Range_10V,
    Range_20V,
    RangeSize
};

Range& operator++(Range &range);
Range& operator--(Range &range);

const char *RangeName(Range range);

// Time

enum TBase
{
    TBase_2ns,
    TBase_5ns,
    TBase_10ns,
    TBase_20ns,
    TBase_50ns,
    TBase_100ns,
    TBase_200ns,
    TBase_500ns,
    TBase_1us,
    TBase_2us,
    TBase_5us,
    TBase_10us,
    TBase_20us,
    TBase_50us,
    TBase_100us,
    TBase_200us,
    TBase_500us,
    TBase_1ms,
    TBase_2ms,
    TBase_5ms,
    TBase_10ms,
    TBase_20ms,
    TBase_50ms,
    TBase_100ms,
    TBase_200ms,
    TBase_500ms,
    TBase_1s,
    TBase_2s,
    TBase_5s,
    TBase_10s,
    TBaseSize
};

const char *TBaseName(TBase tBase);



// Measures


// ������� �� ������� ��� ������ ���������.
enum ModeViewSignals
{
    ModeViewSignals_AsIs,       // ���������� ������� ��� ����
    ModeViewSignals_Compress    // ������� ����� � ���������
};


// Math


enum ScaleMath
{
    ScaleMath_Channel0,     // ������� ������ �� ������ 1
    ScaleMath_Channel1,     // ������� ������ �� ������ 2
    ScaleMath_Hand          // ������� ������� �������.
};


// �������

// ����� �������� ���������.
enum CursCntrl
{
    CursCntrl_1,            // ������.
    CursCntrl_2,            // ������.
    CursCntrl_1_2,          // ������.
    CursCntrl_Disable       // �������.
};


// ������������ ����������� ��������.
enum CursMovement
{
    CursMovement_Points,    // �� ������.
    CursMovement_Percents   // �� ���������.
};

// ����� ������� ������ �������. ����� �������, �� � ����� ������������ �� �������� ����� ���������.
enum CursActive
{
    CursActive_U,
    CursActive_T,
    CursActive_None
};

// ����� �������� ��������.
enum CursLookMode
{
    CursLookMode_None,      // ������� �� ������.
    CursLookMode_Voltage,   // ������� ������ �� ����������� �������������.
    CursLookMode_Time,      // ������� ������ �� �������� �������������.
    CursLookMode_Both       // ������� ������ �� �������� � �����������, � ����������� �� ����, ����� ������� ������� ���������.
};


// Service


// ���� ����
enum Language
{
    Russian,    // �������
    English     // ����������
};


// Debug


// ��� ������������ ��� �������.
// ���� � ���, ��� ������ ��� �� ��������� ��-�� ������� ������������� ( ? ), ������� �� ������ �������������� �������� ��� ������ �� ��� ������.
enum BalanceADCtype
{
    BalanceADC_Disable,     // ������������ ���������.
    BalanceADC_Settings,    // ������������ �������� ������������, ������� �������� �������������.
    BalanceADC_Hand         // ������������ �������� ������������, �������� �������.
};

// ��� �������� ���
enum StretchADCtype
{
    StretchADC_Disable,
    StretchADC_Settings,
    StretchADC_Hand
};

// ��� ��������������� ��������
enum RShiftADCtype
{
    RShiftADC_Disable,
    RShiftADC_Settings,
    RShiftADC_Hand
};
