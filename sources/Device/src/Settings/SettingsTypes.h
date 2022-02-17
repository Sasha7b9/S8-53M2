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


#define ChA Chan::A
#define ChB Chan::B


// ��������.
struct Divider
{
    enum E
    {
        _1,
        _10
    };

    static int ToAbs(Divider::E);
};

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
