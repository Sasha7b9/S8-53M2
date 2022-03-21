// 2022/03/16 08:24:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Data/Data.h"
#include "Utils/Strings.h"
#include "FPGA/TypesFPGA.h"
#include "Settings/SettingsDisplay.h"


namespace Data
{
    DataFrame  last;
    DataFrame  ins;
}


void DataStruct::Log(pchar point)
{
    LOG_WRITE("%s - data valid = %d", point, Valid());
    SU::LogBufferU8(Data(ChA).Data(), 10);
}


const uint8 *DataFrame::DataBegin(Chan ch) const
{
    uint8 *address = (uint8 *)ds + sizeof(DataSettings);

    if (ch.IsB())
    {
        address += ds->BytesInChannel();
    }

    return address;
}


const uint8 *DataFrame::DataEnd(Chan ch)
{
    return DataBegin(ch) + ds->BytesInChannel();
}


void DataStruct::PrepareForNewCycle()
{
    ds.FillFromCurrentSettings();

    A.ReallocAndFill(ds.BytesInChannel(), ValueFPGA::NONE);
    B.ReallocAndFill(ds.BytesInChannel(), ValueFPGA::NONE);

    if (TBase::InModeRandomizer())
    {

    }
    else if (TBase::InModeP2P())
    {
        rec_points = 0;
        all_points = 0;
    }
}


void DataStruct::PrepareForNormalDrawP2P()
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    int first = points.half_iword[0];
    int last = points.half_iword[1];

    int num_points = last - first;
}


void DataStruct::AppendPoints(BitSet16 pointsA, BitSet16 pointsB)
{
    int max_bytes = ds.BytesInChannel();

    uint8 *a = A.Data();
    uint8 *b = B.Data();

    if (rec_points == max_bytes - 1)
    {
        std::memmove(a, a + 1, (uint)(max_bytes - 1));
        std::memmove(b, b + 1, (uint)(max_bytes - 1));
        rec_points = max_bytes - 2;
    }
    else if (rec_points == max_bytes)
    {
        std::memmove(a, a + 2, (uint)(max_bytes - 2));
        std::memmove(b, b + 2, (uint)(max_bytes - 2));
        rec_points = max_bytes - 2;
    }

    a[rec_points] = pointsA.byte0;
    a[rec_points + 1] = pointsA.byte1;

    b[rec_points] = pointsB.byte0;
    b[rec_points + 1] = pointsB.byte1;

    rec_points += 2;
    all_points += 2;
}


DataStruct::DataStruct(const DataFrame &frame) : rec_points(-1), all_points(0)
{
    ds.Set(*frame.ds);

    A.ReallocFromBuffer(frame.DataBegin(ChA), ds.BytesInChannel());

    B.ReallocFromBuffer(frame.DataBegin(ChB), ds.BytesInChannel());
}
