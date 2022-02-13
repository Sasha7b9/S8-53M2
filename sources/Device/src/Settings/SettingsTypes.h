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
