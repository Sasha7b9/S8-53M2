// 2022/03/16 11:20:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Data/DataExtensions.h"
#include "FPGA/TypesFPGA.h"
#include "Data/Storage.h"
#include "Utils/Math.h"
#include "Utils/Strings.h"


namespace Averager
{
    Buffer<float> ave_a;        // ����� ����������� ��������, ������������
    Buffer<float> ave_b;        // �� ���������������� ���������

    void GetDataAround(DataStruct &);
    void GetDataAccurately(DataStruct &);

    int added_datas = 0;        // ������� ������ ������ � ����������
    int number_averaging = 0;   // ������� ����� ����������
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


void Averager::Append(DataStruct &data)
{
    if (ModeAveraging::Current() == ModeAveraging::Accurately)
    {
        return;
    }

    int size = data.ds.BytesInChannel();

    if (ave_a.Size() != size || ModeAveraging::GetNumber() != number_averaging)
    {
        Reset();
    }

    if (added_datas == 0)
    {
        for (int i = 0; i < size; i++)
        {
            ave_a[i] = data.A[i];
            ave_b[i] = data.B[i];
        }
    }
    else
    {
        int num_datas = Storage::NumElementsWithCurrentSettings();

        Math::Limitation<int>(num_datas, 0, ModeAveraging::GetNumber());

        float ave_fless = (float)num_datas - 1.0f;
        float ave_inv = 1.0f / (float)num_datas;

        float *d_a = ave_a.Pointer(0);
        float *d_b = ave_b.Pointer(0);

        uint8 *d0 = data.A.Pointer(0);
        uint8 *d1 = data.B.Pointer(0);

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
    int num_bytes = data.ds.BytesInChannel();

    uint8 *a = data.Data(ChA).Data();
    uint8 *b = data.Data(ChB).Data();

    for (int i = 0; i < num_bytes; i++)
    {
        a[i] = (uint8)ave_a[i];
        b[i] = (uint8)ave_b[i];
    }
}