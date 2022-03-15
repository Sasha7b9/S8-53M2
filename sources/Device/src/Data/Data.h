#pragma once
#include "FPGA/SettingsFPGA.h"


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
    void                *addrNext;                   // ����� ��������� ������.
    void                *addrPrev;                   // ����� ���������� ������.
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

    int16 GetRShift(Chan) const;

    bool InModeP2P() const;

    // �������� ����� � ���������� ������
    void AppendPoints(uint8 *a, uint8 *b, BitSet16 pointsA, BitSet16 pointsB);
};


struct DataStruct
{
    uint8 *A;
    uint8 *B;
    DataSettings *ds;

    DataStruct() : A(nullptr), B(nullptr), ds(nullptr) { }
};


struct DataStructTemp
{
    uint8 *A;
    uint8 *B;
    DataSettings *ds;

    DataStructTemp() : A(nullptr), B(nullptr), ds(nullptr) { }
};


namespace Data
{
    extern DataStructTemp dir;             // �������� ������
    extern DataStructTemp last;
    extern DataStruct ins;              // ����� ������ �� ����, ������� ������ ���������� �� �����
}
