// 2022/03/16 08:24:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Data/Data.h"
#include "Utils/Strings.h"
#include "FPGA/TypesFPGA.h"
#include "Settings/SettingsDisplay.h"
#include "Data/Storage.h"
#include "Utils/Math.h"


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
        address += ds->BytesInChan();
    }

    return address;
}


const uint8 *DataFrame::DataEnd(Chan ch)
{
    return DataBegin(ch) + ds->BytesInChan();
}


uint8 *DataFrame::BeginFrame()
{
    return (uint8 *)ds;
}


uint8 *DataFrame::EndFrame()
{
    return BeginFrame() + 2 * ds->BytesInChan();
}


void FrameImitation::CreateForCurrent()
{
    int bytes_for_channel = ENUM_POINTS_FPGA::ToNumBytes();

    int size_buffer = (int)sizeof(DataSettings) + 2 * bytes_for_channel;

    if (size_buffer != buffer.Size())
    {
        buffer.Realloc(size_buffer);
    }

    frame.ds = (DataSettings *)buffer.Data();

    frame.ds->FillFromCurrentSettings();

    std::memset((uint8 *)frame.DataBegin(ChA), ValueFPGA::NONE, (uint)bytes_for_channel);
    std::memset((uint8 *)frame.DataBegin(ChB), ValueFPGA::NONE, (uint)bytes_for_channel);
}


void FrameImitation::PrepareForNewCycle()
{
    if (TBase::InModeRandomizer())
    {
        DataSettings *last_ds = Storage::GetDataSettings(0);

        if (!last_ds || !last_ds->Equal(*frame.ds))
        {
            CreateForCurrent();
        }
    }
    else
    {
        CreateForCurrent();
    }

    frame.rec_points = 0;
    frame.all_points = 0;
    frame.ds->valid = 0;
}


int DataStruct::PrepareForNormalDrawP2P()
{
    if (!mode_p2p)
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

    int bytes_on_screen = last - first;        // Столько точек всего помещается на экран

    int drawing_points = all_points;            // Здесь будет храниться количество точек, которе нужно вывести на экране

    BufferFPGA screenA(bytes_on_screen);       // Здесь точки канала 1, выводимые на экран
    BufferFPGA screenB(bytes_on_screen);       // Здесь точки канала 2, выводимые на экран

    int pos = 0;

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

    return pos - 1;
}


void FrameImitation::AppendPoints(BitSet16 pointsA, BitSet16 pointsB)
{
    DataSettings &ds = *frame.ds;

    int max_bytes = ds.BytesInChan();

    uint8 *a = (uint8 *)frame.DataBegin(ChA);
    uint8 *b = (uint8 *)frame.DataBegin(ChB);

    int &rec_points = frame.rec_points;

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
    frame.all_points += 2;
    ds.valid = 1;
}


DataStruct::DataStruct(const DataFrame &frame) :
    rec_points(frame.rec_points), all_points(frame.all_points), mode_p2p(false)
{
    ds.Set(*frame.ds);

    A.ReallocFromBuffer(frame.DataBegin(ChA), ds.BytesInChan());

    B.ReallocFromBuffer(frame.DataBegin(ChB), ds.BytesInChan());
}


void DataFrame::FillDataChannelsFromFrame(DataFrame &frame)
{
    int num_bytes = ds->BytesInChan();

    uint8 *address = (uint8 *)ds + sizeof(*ds);

    std::memcpy(address, frame.DataBegin(ChA), (uint)num_bytes);

    address += num_bytes;

    std::memcpy(address, frame.DataBegin(ChB), (uint)num_bytes);
}


void FrameImitation::Inverse(Chan ch)
{
    int num_bytes = frame.ds->BytesInChan();

    uint8 *data = frame.DataBegin(ch);

    for (int i = 0; i < num_bytes; i++)
    {
        data[i] = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(data[i], ValueFPGA::MIN, ValueFPGA::MAX));
    }
}


void DataSettings::Set(const DataSettings &ds)
{
    *this = ds;
}
