#pragma once


struct TShift
{
    static const int EMPTY = 1000000;

    // ���������� ������������� �������� �� �������.
    static void Set(int tShift);

    // ���������� ���������� �������� �� ������� ��� ������ �������������. � ������ �������� ��� �������� ������ ���� ������.
    static void SetDelta(int16 shift);

    // �������� �������� �� ������� � ���������� �����.
    static void Load();

    // ���������� ������������� �������� �� ������� � ��������� ����, ��������� ��� ������ �� �����.
    static pchar ToString(int16 tShiftRel, char buffer[20]);
};


struct RShift
{
    // ���������� ������������� �������� �� ����������.
    static void Set(Chan::E, int16 rShift);

    // �������� �������� �� ���������� � ���������� �����.
    static void Load(Chan::E);
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
};

Range::E &operator++(Range::E &range);
Range::E &operator--(Range::E &range);


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
    void*       addrNext;               // ����� ��������� ������.
    void*       addrPrev;               // ����� ���������� ������.
    uint        rShiftCh0       : 10;   // �������� �� ����������
    uint        rShiftCh1       : 10;
    uint        trigLevCh0      : 10;   // ������� �������������
    int16       tShift;                 // �������� �� �������
    ModeCouple  modeCouple1     : 2;
    Range::E    range[2];               // ������� �� ���������� ����� �������.

    uint        trigLevCh1      : 10;
    uint        length1channel  : 11;   // ������� �������� � ������ ����� ��������� ������ ������
    TBase::E    tBase           : 5;    // ������� �� �������
    ModeCouple  modeCouple0     : 2;    // ����� ������ �� �����
    uint        peakDet         : 2;    // ������� �� ������� ��������
    uint        enableCh0       : 1;    // ������� �� ����� 0
    uint        enableCh1       : 1;    // ������� �� ����� 1

    uint        inverseCh0      : 1;
    uint        inverseCh1      : 1;
    Divider     multiplier0     : 1;
    Divider     multiplier1     : 1;
    PackedTime  time;

    void PrintElement();

    // ���������, ������� ������ ���������, ����� ��������� ��������� � ����������� dp
    int SizeElem();

    void FillDataPointer();
};
