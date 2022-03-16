// 2022/03/16 11:20:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Data/DataExtensions.h"
#include "FPGA/TypesFPGA.h"
#include "Data/Storage.h"
#include "Utils/Math.h"


namespace Averager
{
    Buffer<float> ave_a;        // Здесь усреднённые значения, рассчитанные
    Buffer<float> ave_b;        // по приблизетильному алгоритму

    void GetDataAround(DataStruct &);
    void GetDataAccurately(DataStruct &);
}


void Averager::Reset()
{
//    if (SettingsDisplay::NumAverages() < 2)
//    {
//        ave.A.Realloc(0);
//        ave.B.Realloc(0);
//        ave_a.Realloc(0);
//        ave_b.Realloc(0);
//    }
//    else
//    {
//        ave.ds.Init();
//
//        ave.A.Realloc(ave.ds.BytesInChannel());
//        ave.A.Fill(ValueFPGA::NONE);
//
//        ave.B.Realloc(ave.ds.BytesInChannel());
//        ave.B.Fill(ValueFPGA::NONE);
//
//        if (SettingsDisplay::GetModeAveraging() == ModeAveraging::Around)
//        {
//
//        }
//    }
}


void Averager::Append(const DataSettings *dss, uint8 *a, uint8 *b)
{
    if (SettingsDisplay::GetModeAveraging() == ModeAveraging::Accurately)
    {
        return;
    }

    int num_datas = Storage::NumElementsWithCurrentSettings();

    int size = dss->BytesInChannel();

    if (num_datas == 1)
    {
        for (int i = 0; i < size; i++)
        {
            ave_a[i] = a[i];
            ave_b[i] = b[i];
        }
    }
    else
    {
        Math::Limitation<int>(num_datas, 0, SettingsDisplay::GetNumAverages());

        float numAveDataF = num_datas;
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


void Averager::GetData(DataStruct &data)
{
    if (SettingsDisplay::GetModeAveraging() == ModeAveraging::Accurately)
    {
        GetDataAccurately(data);
    }

    if (SettingsDisplay::GetModeAveraging() == ModeAveraging::Around)
    {
        GetDataAround(data);
    }
}


void Averager::GetDataAccurately(DataStruct &out)
{
    int num_datas = SettingsDisplay::GetNumAverages();

    LIMIT_ABOVE(num_datas, Storage::NumElementsWithSameSettings());

    DataSettings &ds = *Storage::GetDataSettings(0);

    int num_bytes = ds.BytesInChannel();

    Buffer<uint> sum_a(num_bytes);
    sum_a.Fill(0);

    Buffer<uint> sum_b(num_bytes);
    sum_b.Fill(0);

    for (int d = 0; d < num_datas; d++)
    {
        DataStruct data;

        Storage::GetData(d, data);

        uint8 *in_a = data.A.Data();
        uint8 *in_b = data.B.Data();

        for (int i = 0; i < num_bytes; i++)
        {
            sum_a[i] += in_a[i];
            sum_b[i] += in_b[i];
        }
    }

    out.ds.Set(ds);
    out.A.Realloc(num_bytes);
    out.B.Realloc(num_bytes);

    uint8 *out_a = out.A.Data();
    uint8 *out_b = out.B.Data();

    for (int i = 0; i < num_bytes; i++)
    {
        out_a[i] = sum_a[i] / num_datas;
        out_b[i] = sum_b[i] / num_datas;
    }
}


void Averager::GetDataAround(DataStruct &data)
{

}


//    static uint8 data_out[Chan::Count][FPGA::MAX_POINTS * 2];
//
//    if (newSumCalculated[ch] == false)
//    {
//        _data.Fill(&data_out[ch][0], FPGA::MAX_POINTS * 2);
//
//        return;
//    }
//
//    newSumCalculated[ch] = false;
//
//    DataStruct data;
//
//    GetData(0, data);
//
//    if (!data.Valid())
//    {
//        return;
//    }
//
//    uint numPoints = (uint)data.ds.BytesInChannel();
//
//    if (SettingsDisplay::GetModeAveraging() == ModeAveraging::Around)
//    {
//        float *floatAveData = (ch == Chan::A) ? ave_a : ave_b;
//
//        for (uint i = 0; i < numPoints; i++)
//        {
//            data_out[ch][i] = (uint8)(floatAveData[i]);
//        }
//
//        _data.Fill(&data_out[ch][0], FPGA::MAX_POINTS * 2);
//
//        return;
//    }
//
//    int numAveraging = SettingsDisplay::NumAverages();
//
//    LIMIT_ABOVE(numAveraging, NumElementsWithSameSettings());
//
//    for (uint i = 0; i < numPoints; i++)
//    {
//        data_out[ch][i] = sum[ch][i] / numAveraging;
//    }
//
//    _data.Fill(&data_out[ch][0], FPGA::MAX_POINTS * 2);
//}
