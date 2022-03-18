// 2022/03/16 08:24:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Data/Data.h"
#include "Utils/Strings.h"


namespace Data
{
    DataStruct dir;
    DataStruct last;
    DataStruct ins;
}


void DataStruct::Log(pchar point)
{
    LOG_WRITE("%s - data valid = %d", point, Valid());
    SU::LogBufferU8(Data(ChA).Data(), 10);
}


uint8 *Frame::DataBegin(Chan ch)
{
    uint8 *address = (uint8 *)ds + sizeof(DataSettings);

    if (ch.IsB())
    {
        address += ds->BytesInChannel();
    }

    return address;
}


uint8 *Frame::DataEnd(Chan ch)
{
    return DataBegin(ch) + ds->BytesInChannel();
}


void DataStruct::AppendPoints(BitSet16 pointsA, BitSet16 pointsB)
{
    int max_bytes = ds.BytesInChannel();

    uint8 *a = (uint8 *)this + max_bytes;
    uint8 *b = a + max_bytes;

    if (rec_point == max_bytes - 1)
    {
        std::memmove(a, a + 1, (uint)(max_bytes - 1));
        std::memmove(b, b + 1, (uint)(max_bytes - 1));
        rec_point = max_bytes - 2;
    }
    else if (rec_point == max_bytes)
    {
        std::memmove(a, a + 2, (uint)(max_bytes - 2));
        std::memmove(b, b + 2, (uint)(max_bytes - 2));
        rec_point = max_bytes - 2;
    }

    a[rec_point] = pointsA.byte0;
    a[rec_point + 1] = pointsA.byte1;

    b[rec_point] = pointsB.byte0;
    b[rec_point + 1] = pointsB.byte1;

    rec_point += 2;
    all_points += 2;
}
