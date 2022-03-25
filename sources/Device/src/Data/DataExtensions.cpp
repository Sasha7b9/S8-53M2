// 2022/03/16 11:20:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Data/DataExtensions.h"
#include "FPGA/TypesFPGA.h"
#include "Data/Storage.h"
#include "Utils/Math.h"
#include "Utils/Strings.h"
#include "Data/Processing.h"


namespace Averager
{
    Buffer<float> ave_a;        // Здесь усреднённые значения, рассчитанные
    Buffer<float> ave_b;        // по приблизетильному алгоритму

    void GetDataAround(DataStruct &);
    void GetDataAccurately(DataStruct &);

    int added_datas = 0;        // Столько данных учтено в измерениях
    int number_averaging = 0;   // Текущее число усреднений
}


namespace Limitator
{
    // Максимальные значения каналов
    uint8 lim_up[Chan::Count][FPGA::MAX_POINTS * 2];

    // Минимальные значения каналов
    uint8 lim_down[Chan::Count][FPGA::MAX_POINTS * 2];
}


void Averager::Reset()
{
    if (ModeAveraging::GetNumber() < 2)
    {
        ave_a.Free();
        ave_a.Free();
    }
    else
    {
        if (ModeAveraging::Current() == ModeAveraging::Around)
        {
            ave_a.Realloc(ENUM_POINTS_FPGA::ToNumBytes());
            ave_b.Realloc(ENUM_POINTS_FPGA::ToNumBytes());
        }
    }

    added_datas = 0;
    number_averaging = ModeAveraging::GetNumber();
}


void Averager::Append(DataFrame &frame)
{
    if (ModeAveraging::Current() == ModeAveraging::Accurately)
    {
        return;
    }

    int size = frame.ds->BytesInChanStored();

    if (ave_a.Size() != size || ModeAveraging::GetNumber() != number_averaging)
    {
        Reset();
    }

    const uint8 *a = frame.DataBegin(ChA);
    const uint8 *b = frame.DataBegin(ChB);

    if (added_datas == 0)
    {
        for (int i = 0; i < size; i++)
        {
            ave_a[i] = a[i];
            ave_b[i] = b[i];
        }
    }
    else
    {
        int num_datas = Storage::SameSettings::GetCount();

        Math::Limitation<int>(num_datas, 0, ModeAveraging::GetNumber());

        float ave_fless = (float)num_datas - 1.0f;
        float ave_inv = 1.0f / (float)num_datas;

        float *d_a = ave_a.Pointer(0);
        float *d_b = ave_b.Pointer(0);

        const uint8 *d0 = a;
        const uint8 *d1 = b;

        float *end = ave_a.Pointer(size);

        while(d_a < end)
        {
            *d_a = ((*d_a) * ave_fless + (float)(*d0++)) * ave_inv;
            d_a++;

            *d_b = ((*d_b) * ave_fless + (float)(*d1++)) * ave_inv;
            d_b++;

        };
    }

    added_datas++;
}


void Averager::GetData(DataStruct &data)
{
    if (ModeAveraging::Current() == ModeAveraging::Accurately)
    {
        GetDataAccurately(data);
    }

    if (ModeAveraging::Current() == ModeAveraging::Around)
    {
        GetDataAround(data);
    }
}


void Averager::GetDataAccurately(DataStruct &out)
{
    int num_datas = ModeAveraging::GetNumber();

    LIMIT_ABOVE(num_datas, Storage::SameSettings::GetCount());

    DataSettings ds = Storage::GetDataSettings(0);

    int num_bytes = ds.BytesInChanStored();

    Buffer<uint> sum_a(num_bytes);
    sum_a.Fill(0);

    Buffer<uint> sum_b(num_bytes);
    sum_b.Fill(0);

    for (int d = 0; d < num_datas; d++)
    {
        DataStruct &data = Storage::GetData(d);

        const uint8 *in_a = data.Data(ChA).Data();
        const uint8 *in_b = data.Data(ChB).Data();

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
    int num_bytes = data.ds.BytesInChanStored();

    uint8 *a = data.A.Data();
    uint8 *b = data.B.Data();

    for (int i = 0; i < num_bytes; i++)
    {
        a[i] = (uint8)ave_a[i];
        b[i] = (uint8)ave_b[i];
    }
}


DataStruct &Limitator::GetLimitation(Chan ch, int direction, DataStruct &data)
{
    uint8 *buffer = 0;

    if (direction == 0)
    {
        buffer = &(lim_down[ch][0]);
    }
    else if (direction == 1)
    {
        buffer = &(lim_up[ch][0]);
    }

    data.ds.Set(Processing::out.ds);

    data.Data(ch).ReallocFromBuffer(buffer, data.ds.BytesInChanStored());

    return data;
}


void Limitator::ClearLimits()
{
    std::memset(lim_up[0], 0, FPGA::MAX_POINTS * 2);
    std::memset(lim_up[1], 0, FPGA::MAX_POINTS * 2);
    std::memset(lim_down[0], 0xff, FPGA::MAX_POINTS * 2);
    std::memset(lim_down[1], 0xff, FPGA::MAX_POINTS * 2);
}


void Limitator::CalculateLimits(const DataSettings *dss, const uint8 *a, const uint8 *b)
{
    uint numElements = (uint)dss->PointsInChannel();

    if (Storage::NumFrames() == 0 || NUM_MIN_MAX == 1 || (!Storage::GetDataSettings(0).Equal(*dss)))
    {
        for (uint i = 0; i < numElements; i++)
        {
            lim_down[0][i] = lim_up[0][i] = a[i];
            lim_down[1][i] = lim_up[1][i] = b[i];
        }
    }
    else
    {
        int allDatas = Storage::SameSettings::GetCount();
        LIMITATION(allDatas, allDatas, 1, NUM_MIN_MAX);

        if (Storage::SameSettings::GetCount() >= NUM_MIN_MAX)
        {
            for (uint i = 0; i < numElements; i++)
            {
                lim_down[0][i] = lim_up[0][i] = a[i];
                lim_down[1][i] = lim_up[1][i] = b[i];
            }
            allDatas--;
        }

        for (int numData = 0; numData < allDatas; numData++)
        {
            DataStruct &data = Storage::GetData(numData);

            const uint8 *dA = data.A.Data();
            const uint8 *dB = data.B.Data();

            for (uint i = 0; i < numElements; i++)
            {
                if (dA[i] < lim_down[0][i]) lim_down[0][i] = dA[i];
                if (dA[i] > lim_up[0][i])   lim_up[0][i] = dA[i];
                if (dB[i] < lim_down[1][i]) lim_down[1][i] = dB[i];
                if (dB[i] > lim_up[1][i])   lim_up[1][i] = dB[i];
            }
        }
    }
}
