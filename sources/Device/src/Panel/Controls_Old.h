// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// �������������� ������.
struct KeyOld
{
    enum E
    {
        Empty    = 0,    // ������ �� ������
        ChannelA = 1,    // ����� 1
        Service  = 2,    // ������
        ChannelB = 3,    // ����� 2
        Display  = 4,    // �������
        Time     = 5,    // ����
        Memory   = 6,    // ������
        Trig     = 7,    // �����
        Start    = 8,    // ����/�����
        Cursors  = 9,    // �������
        Measures = 10,   // �����
        Power    = 11,   // �������
        Help     = 12,   // ������
        Menu     = 13,   // ����
        F1       = 14,   // 1
        F2       = 15,   // 2
        F3       = 16,   // 3
        F4       = 17,   // 4
        F5       = 18,   // 5
        Count            // ����� ���������� ������
    };

    static pchar Name(KeyOld::E);

    // ���������� true, ���� button - �������������� ������� [1..5].
    static bool IsFunctional(KeyOld::E);
};

KeyOld::E& operator++(KeyOld::E &);

// �������������� �����������.
struct RegulatorOld
{
    enum E
    {
        Empty   = 0,    // ��������� �� ��������
        RangeA  = 20,   // �����/��� ���. 1
        RShiftA = 21,
        RangeB  = 22,   // �����/��� ���. 2
        RShiftB = 23,
        TBase   = 24,   // �����/���
        TShift  = 25,
        TrigLev = 26,   // �������
        Set     = 27    // ���������
    };

    // ���������� true, ���� ��������� �������������� �����
    static bool IsLeft(uint16 data) { return (data >= RangeA && data <= Set); }

    // ���������� false, ���� ��������� �������������� ������
    static bool IsRight(uint16 data) { return ((data & 0x7f) >= 20) && ((data & 0x7f) <= 27); }
};
