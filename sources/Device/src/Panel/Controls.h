// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// �������������� ������.
struct Key
{
    enum E
    {
        Empty = 0,        // ������ �� ������
        ChannelA = 1,        // ����� 1
        Service = 2,        // ������
        ChannelB = 3,        // ����� 2
        Display = 4,        // �������
        Time = 5,        // ����
        Memory = 6,        // ������
        Trig = 7,        // �����
        Start = 8,        // ����/�����
        Cursors = 9,        // �������
        Measures = 10,       // �����
        Power = 11,       // �������
        Help = 12,       // ������
        Menu = 13,       // ����
        F1 = 14,       // 1
        F2 = 15,       // 2
        F3 = 16,       // 3
        F4 = 17,       // 4
        F5 = 18,       // 5
        Count            // ����� ���������� ������
    };
};

Key::E& operator++(Key::E &);

// �������������� �����������.
enum Regulator
{
    R_Empty     = 0,    // ��������� �� ��������
    R_RangeA    = 20,   // �����/��� ���. 1
    R_RShiftA   = 21,   
    R_RangeB    = 22,   // �����/��� ���. 2
    R_RShiftB   = 23,
    R_TBase     = 24,   // �����/���
    R_TShift    = 25,
    R_TrigLev   = 26,   // �������
    R_Set       = 27    // ���������
};

// ����������� �������� �����������.
enum RegulatorDirection
{
    Direction_Left,     // �������� ���������� �����
    Direction_Right,    // �������� ���������� ������
    Direction_None      // �������� �����������
};


#define NUM_REGULATORS (R_Set - R_RangeA + 1)


pchar NameButton(Key::E);
