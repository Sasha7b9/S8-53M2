// 2022/03/16 08:24:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Data/Data.h"
#include "Utils/Strings.h"
#include "FPGA/TypesFPGA.h"
#include "Utils/Math.h"


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


void DataSettings::InverseData(Chan ch)
{
    int num_bytes = BytesInChannel();

    uint8 *data = GetDataBegin(ch);

    for (int i = 0; i < num_bytes; i++)
    {
        data[i] = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(data[i], ValueFPGA::MIN, ValueFPGA::MAX));
    }
}


uint8 *DataSettings::GetDataBegin(Chan ch)
{
    uint8 *result = (uint8 *)this + sizeof(DataSettings);

    if (ch.IsB())
    {
        result += BytesInChannel();
    }

    return result;
}


uint8 *DataSettings::GetDataEnd(Chan ch)
{
    return GetDataBegin(ch) + BytesInChannel();
}
