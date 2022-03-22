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

    DataSettings() { valid = 0; }; //-V730

    void Set(const DataSettings &ds) { *this = ds; valid = 1; }

    bool Equal(const DataSettings &);

    void PrintElement();

    // ���������, ������� ������ ���������, ����� ��������� ��������� � ����������� dp
    int SizeFrame();

    // ���������� ����� �������� �����������
    void FillFromCurrentSettings();

    int BytesInChannel() const;

    int PointsInChannel() const;

    int16 GetRShift(Chan) const;

    bool Valid() const { return (valid == 1); };
};


struct BufferFPGA : public Buffer<uint8>
{
    BufferFPGA(int size = 0) : Buffer<uint8>(size) { }
};


// ��������� ������ � ���� ������ �� ������, ���������� � ���������. �� ������ ��������.
// ��� ����� ������
struct DataFrame
{
    const DataSettings *ds;

    static int rec_points;
    static int all_points;

    DataFrame(DataSettings *_ds = nullptr) : ds(_ds) { };

    // ������ ������ ������
    uint8 *DataBegin(Chan) const;

    // ����� ������ ������
    const uint8 *DataEnd(Chan);

    // ����� ������ ������ �� BufferFPGA
    void GetDataChannelFromBuffer(Chan, BufferFPGA &);

    // ��������� ����� ds �� DataStruct
    void GetDataChannelsFromStruct(DataStruct &);

    // ��������� ����� ������� �� ������
    void GetDataChannelsFromFrame(DataFrame &);

    bool Valid() const { return (ds->valid == 1); }
};


// ��������� ������ � ���� ������ ������ ��� �������� � ���������
struct DataStruct
{
    BufferFPGA   A;
    BufferFPGA   B;
    DataSettings ds;

    int rec_points;         // ��� ����� ������ ����� ������������. �� ����� ���� ������ ���������� ���� � ������
    int all_points;         // ����� ������� �����
    bool mode_p2p;          // �������� �� ��� ��� ���������� �����

    DataStruct() : rec_points(0), all_points(0), mode_p2p(false) { }

    DataStruct(const DataFrame &);

    BufferFPGA &Data(Chan ch) { return ch.IsA() ? A : B; }

    bool Valid() const { return ds.Valid(); }

    // ����������� ������ ����������� ������ � ������ � ���������� ������
    // ���������� ������� ����� (������� � ����� ������� �����), � ������� ����� �������� ������������ �����
    int PrepareForNormalDrawP2P();

    void Log(pchar point);
};


namespace Data
{
    extern DataFrame last;      // ����� ������ �� ������-���������, ���� ����� ��� �����������
    extern DataFrame ins;       // ����� ������ �� ����, ���� ����� ��� ����������� 
}
