// 2022/03/16 08:24:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Data/Data.h"
#include "FPGA/TypesFPGA.h"
#include "Settings/SettingsDisplay.h"
#include "Data/Storage.h"
#include "Utils/Math.h"
#include "Data/Processing.h"
#include "Utils/Text/Text.h"
#include <cstring>


int DataFrame::all_points = 0;
int DataFrame::rec_points = 0;


namespace Data
{
    DataFrame  last;
    DataFrame  ins;
}


void DataStruct::Log(pchar point)
{
    LOG_WRITE("%s - data valid = %d", point, ds.valid);
    SU::LogBufferU8(Data(ChA).Data(), 10);
}


uint8 *DataFrame::DataBegin(Chan ch) const
{
    uint8 *address = (uint8 *)ds + sizeof(DataSettings);

    if (ch.IsB())
    {
        address += ds->BytesInChanStored();
    }

    return address;
}


const uint8 *DataFrame::DataEnd(Chan ch)
{
    return DataBegin(ch) + ds->BytesInChanStored();
}


uint8 *DataFrame::BeginFrame()
{
    return (uint8 *)ds;
}


uint8 *DataFrame::EndFrame()
{
    return BeginFrame() + 2 * ds->BytesInChanStored();
}


void FrameImitation::CreateForCurrent()
{
    int bytes_for_channel = ENUM_POINTS_FPGA::ToNumBytes();

    data.ds.FillFromCurrentSettings();

    data.A.ReallocAndFill(bytes_for_channel, ValueFPGA::NONE);
    data.B.ReallocAndFill(bytes_for_channel, ValueFPGA::NONE);
}


void FrameImitation::PrepareForNewCycle()
{
    if (TBase::InModeRandomizer())
    {
        DataSettings last_ds = Storage::GetDataSettings(0);

        if (!last_ds.valid)
        {
            CreateForCurrent();
        }
        else if (!last_ds.Equal(DataSettings()))
        {
            const DataStruct &last = Storage::GetData(0);
            Processing::CountedToCurrentSettings(last.ds, last.A.DataConst(), last.B.DataConst(), data);
            data.ds.FillFromCurrentSettings();
        }
    }
    else
    {
        CreateForCurrent();
    }

    data.rec_points = 0;
    data.all_points = 0;
    data.ds.valid = 0;
    data.mode_p2p = TBase::InModeP2P();
}


int DataStruct::PrepareForNormalDrawP2P()
{
    if (!mode_p2p)
    {
        return 0;
    }

    if (!ds.valid)
    {
        return 0;
    }

    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    int first = points.half_iword[0];           // Позиция первой выводимой точки
    int last = points.half_iword[1];            // Позиция последней выводимой точки

    if (ds.peak_det)
    {
        first *= 2;
        last *= 2;
    }

    int bytes_on_screen = last - first ;             // Столько точек всего помещается на экран

    int drawing_points = all_points;                // Здесь будет храниться количество точек, которе нужно вывести на экране

    BufferFPGA screenA(bytes_on_screen, ValueFPGA::NONE);       // Здесь точки канала 1, выводимые на экран
    BufferFPGA screenB(bytes_on_screen, ValueFPGA::NONE);       // Здесь точки канала 2, выводимые на экран

    int pos = 0;

    if (SET_SELFRECORDER)
    {
        if (rec_points < bytes_on_screen)
        {
            for (int i = 0; i < rec_points; i++)
            {
                screenA[pos] = A[i];
                screenB[pos] = B[i];
                pos++;
            }
        }
        else
        {
            for (int i = rec_points - bytes_on_screen; i < rec_points; i++)
            {
                screenA[pos] = A[i];
                screenB[pos] = B[i];
                pos++;
            }
        }
    }
    else
    {
        while (drawing_points-- > rec_points)
        {
            pos++;

            if (pos == bytes_on_screen)
            {
                pos = 0;
            }
        }

        for (int i = 0; i < rec_points; i++)
        {
            screenA[pos] = A[i];
            screenB[pos] = B[i];
            pos++;

            if (pos == bytes_on_screen)
            {
                pos = 0;
            }
        }
    }

    uint8 *a = screenA.Data();
    uint8 *b = screenB.Data();

    for (int i = first; i <= last; i++)
    {
        A[i] = *a++;
        B[i] = *b++;
    }

    if (ds.peak_det)
    {
        pos /= 2;
    }

    return pos;
}


void FrameImitation::AppendPoints(BitSet16 pointsA, BitSet16 pointsB)
{
    DataSettings &ds = data.ds;

    int max_bytes = ds.BytesInChanReal();

    uint8 *a = data.A.Data();
    uint8 *b = data.B.Data();

    int &rec_points = data.rec_points;

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
    data.all_points += 2;
    ds.valid = 1;
}


DataStruct::DataStruct(const DataFrame &frame) :
    rec_points(frame.rec_points), all_points(frame.all_points), mode_p2p(false)
{
    ds.Set(*frame.ds);

    A.ReallocFromBuffer(frame.DataBegin(ChA), ds.BytesInChanStored());

    B.ReallocFromBuffer(frame.DataBegin(ChB), ds.BytesInChanStored());
}


void DataFrame::FillDataChannelsFromFrame(DataFrame &frame)
{
    int num_bytes = ds->BytesInChanStored();

    uint8 *address = (uint8 *)ds + sizeof(*ds);

    std::memcpy(address, frame.DataBegin(ChA), (uint)num_bytes);

    address += num_bytes;

    std::memcpy(address, frame.DataBegin(ChB), (uint)num_bytes);
}


void FrameImitation::Inverse(Chan ch)
{
    int num_bytes = data.ds.BytesInChanReal();

    uint8 *dat = data.Data(ch).Data();

    for (int i = 0; i < num_bytes; i++)
    {
        dat[i] = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(dat[i], ValueFPGA::MIN, ValueFPGA::MAX));
    }
}


void DataSettings::Set(const DataSettings &ds)
{
    *this = ds;
}
