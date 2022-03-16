// 2022/03/16 11:20:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Data/DataExtensions.h"
#include "FPGA/TypesFPGA.h"


namespace Averager
{
    DataStruct ave;

    // В этих массивах хранятся усреднённые значения, подсчитанные по приблизительному алгоритму.
    float ave_a[FPGA::MAX_POINTS * 2] = {0.0f};

    float ave_b[FPGA::MAX_POINTS * 2] = {0.0f};

    // Если true, то новые суммы рассчитаны, и нужно повторить расчёт среднего
    bool newSumCalculated[Chan::Count] = {true, true};
}


void Averager::Append(DataStruct &)
{

}


void Averager::GetData(DataStruct &)
{

}


void Averager::Reset(DataStruct &)
{
    ave.ds.Init();

    ave.A.Realloc(ave.ds.BytesInChannel());
    ave.A.Fill(ValueFPGA::NONE);

    ave.B.Realloc(ave.ds.BytesInChannel());
    ave.B.Fill(ValueFPGA::NONE);
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



void Storage::CalculateAroundAverage(const DataSettings *dss, uint8 *a, uint8 *b)
{
    int numAveData = NumElementsWithCurrentSettings();

    int size = dss->BytesInChannel();

    if (numAveData == 1)
    {
        for (int i = 0; i < size; i++)
        {
            ave_a[i] = a[i];
            ave_b[i] = b[i];
        }
    }
    else
    {
        Math::Limitation<int>(numAveData, 0, SettingsDisplay::NumAverages());

        float numAveDataF = numAveData;
        float numAveDataFless = numAveDataF - 1.0f;
        float numAveDataInv = 1.0f / numAveDataF;
        float *aData0 = &ave_a[0];
        float *aData1 = &ave_b[0];
        uint8 *d0 = &a[0];
        uint8 *d1 = &b[0];
        float *endData = &ave_a[size];

        do
        {
            *aData0 = ((*aData0) * numAveDataFless + (float)(*d0++)) * numAveDataInv;
            aData0++;
            *aData1 = ((*aData1) * numAveDataFless + (float)(*d1++)) * numAveDataInv;
            aData1++;
        } while (aData0 != endData);
    }
}
