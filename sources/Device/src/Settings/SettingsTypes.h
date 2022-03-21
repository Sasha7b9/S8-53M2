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
struct Chan 
{ 
    enum E
    {
        A,
        B,
        A_B,
        Math,
        Count = 2
    };

    E value;

    Chan(E v = A) : value(v) {}

    bool Enabled();

    // ���������� ����� ������ �� 1 �� 2
    int ToNumber() { return (value == Chan::A) ? 1 : 2; }

    bool IsA() const { return value == A; }
    bool IsB() const { return value == B; }

    operator int() { return (int)value; }
};


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
struct CursCntrl
{
    enum E
    {
        _1,         // ������.
        _2,         // ������.
        _1_2,       // ������.
        Disable     // �������.
    };
};


// ������������ ����������� ��������.
struct CursMovement
{
    enum E
    {
        Points,    // �� ������.
        Percents   // �� ���������.
    };
};

// ����� ������� ������ �������. ����� �������, �� � ����� ������������ �� �������� ����� ���������.
struct CursActive
{
    enum E
    {
        U,
        T,
        None
    };
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


// ��� ��������������� ��������
enum RShiftADCtype
{
    RShiftADC_Disable,
    RShiftADC_Settings,
    RShiftADC_Hand
};
