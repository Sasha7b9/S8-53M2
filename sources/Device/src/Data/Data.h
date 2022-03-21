// 2022/03/16 08:24:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "FPGA/SettingsFPGA.h"
#include "Utils/Containers/Buffer.h"


struct DataStruct;


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
    void                *next;                      // ����� ��������� ������.
    void                *prev;                      // ����� ���������� ������.
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
    uint                inv_a               : 1;
    uint                inv_b               : 1;
    Divider::E          div_a               : 1;
    Divider::E          div_b               : 1;
    uint                valid               : 1;
    PackedTime          time;

    DataSettings() { valid = 0; };

    void Set(const DataSettings &ds) { *this = ds; valid = 1; }

    bool Equal(const DataSettings &);

    void PrintElement();

    // ���������, ������� ������ ���������, ����� ��������� ��������� � ����������� dp
    int SizeFrame();

    // ���������� ����� �������� �����������
    void Init();

    int BytesInChannel() const;

    int PointsInChannel() const;

    int16 GetRShift(Chan) const;

    bool Valid() const { return (valid == 1); };
};


struct BufferFPGA : public Buffer<uint8>
{
    BufferFPGA(int size = 0) : Buffer<uint8>(size) { }

    // ����������� ������.
    void InverseData();
};


// ��������� ������ � ���� ������ �� ������, ���������� � ���������. �� ������ ��������.
// ��� ����� ������
struct DataFrame
{
    const DataSettings *ds;

    DataFrame(DataSettings *_ds = nullptr) : ds(_ds) { };

    // ������ ������ ������
    const uint8 *DataBegin(Chan) const;

    // ����� ������ ������
    const uint8 *DataEnd(Chan);

    // ��������� ����� ds �� DataStruct
    void GetDataChannelsFromStruct(DataStruct &);

    bool Valid() const { return (ds->valid == 1); }
};


// ��������� ������ � ���� ������ ������ ��� �������� � ���������
struct DataStruct
{
    BufferFPGA   A;
    BufferFPGA   B;
    DataSettings ds;

    int rec_point;
    int all_points;

    DataStruct() : rec_point(0), all_points(0) { }

    DataStruct(const DataFrame &);

    BufferFPGA &Data(Chan ch) { return ch.IsA() ? A : B; }

    bool Valid() const { return ds.Valid(); }

    void AppendPoints(BitSet16 pointsA, BitSet16 pointsB);

    void Log(pchar point);
};


namespace Data
{
    extern DataFrame in;        // ����� �������� �������� ������ ��� ��������� � ��������� - ��������� �� Storage,
                                //������������ �� Storage, �� ��� ��� ��������������

    extern DataStruct out;      // ����� �������� ������, ������� ��� ������ - ��������������� �� in

    extern DataFrame last;      // ����� ������ �� ������-���������, ���� ����� ��� �����������
    extern DataFrame ins;       // ����� ������ �� ����, ���� ����� ��� ����������� 
}
