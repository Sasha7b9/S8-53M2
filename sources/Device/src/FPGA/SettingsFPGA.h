#pragma once


struct TShift
{
    static const int EMPTY = 1000000;
};


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
    Range       range[2];               // ������� �� ���������� ����� �������.

    uint        trigLevCh1      : 10;
    uint        length1channel  : 11;   // ������� �������� � ������ ����� ��������� ������ ������
    TBase       tBase           : 5;    // ������� �� �������
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
};
