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


uint8 *DataSettings::DataBegin(Chan ch)
{
    uint8 *address = (uint8 *)this + sizeof(DataSettings);

    if (ch.IsB())
    {
        address += BytesInChannel();
    }

    return address;
}


uint8 *DataSettings::DataEnd(Chan ch)
{
    return DataBegin(ch) + BytesInChannel();
}
