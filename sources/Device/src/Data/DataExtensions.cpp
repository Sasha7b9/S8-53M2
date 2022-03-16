// 2022/03/16 11:20:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Data/DataExtensions.h"


void Averager::Append(DataStruct &)
{

}


void Averager::GetData(DataStruct &)
{

}


void Averager::Reset(DataStruct &)
{

}


void Storage::GetAverageData(Chan ch, BufferU8 &_data)
{
    static uint8 data_out[Chan::Count][FPGA::MAX_POINTS * 2];

    if (newSumCalculated[ch] == false)
    {
        _data.Fill(&data_out[ch][0], FPGA::MAX_POINTS * 2);

        return;
    }

    newSumCalculated[ch] = false;

    DataStruct data;

    GetData(0, data);

    if (!data.Valid())
    {
        return;
    }

    uint numPoints = (uint)data.ds.BytesInChannel();

    if (SettingsDisplay::GetModeAveraging() == ModeAveraging::Around)
    {
        float *floatAveData = (ch == Chan::A) ? ave_a : ave_b;

        for (uint i = 0; i < numPoints; i++)
        {
            data_out[ch][i] = (uint8)(floatAveData[i]);
        }

        _data.Fill(&data_out[ch][0], FPGA::MAX_POINTS * 2);

        return;
    }

    int numAveraging = SettingsDisplay::NumAverages();

    LIMIT_ABOVE(numAveraging, NumElementsWithSameSettings());

    for (uint i = 0; i < numPoints; i++)
    {
        data_out[ch][i] = sum[ch][i] / numAveraging;
    }

    _data.Fill(&data_out[ch][0], FPGA::MAX_POINTS * 2);
}

