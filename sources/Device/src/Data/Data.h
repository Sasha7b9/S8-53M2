// 2022/03/16 08:24:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "FPGA/SettingsFPGA.h"
#include "Utils/Containers/Buffer.h"


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
    uint                valid               : 1;
    PackedTime          time;
    // ���������� �����
    int16               rec_point;                  // ������ ����� �������� ��� �����. ���� -1 - �� ����� �� ����������. �� ������ ���������

    DataSettings() { valid = 0; };

    void Set(DataSettings &ds) { *this = ds; valid = 1; }

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

    bool Valid() const { return (valid == 1); };
};


struct BufferU8 : public Buffer<uint8>
{
    BufferU8(int size = 0) : Buffer<uint8>(size) { }
};


struct DataStruct
{
    BufferU8     A;
    BufferU8     B;
    DataSettings ds;

    DataStruct() { }

    bool Valid() const { return ds.Valid(); }
};


namespace Data
{
    extern DataStruct dir;             // �������� ������
    extern DataStruct last;
    extern DataStruct ins;              // ����� ������ �� ����, ������� ������ ���������� �� �����
}
