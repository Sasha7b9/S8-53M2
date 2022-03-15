#include "defines.h"
#include "Data/DataController.h"
#include "Settings/Settings.h"
#include "Data/Storage.h"
#include "Menu/Pages/Definition.h"
#include <cstring>


namespace DataController
{
    static DataStruct dataDirect;
    static DataStruct dataLast;
    static DataStruct dataInternal;

    static void FillDataDirect();

    static void FillDataLast();

    static void FillDataInternal();
}


void DataSettings::PrintElement()
{
    LOG_WRITE("addr:%x, addrNext:%x, addrPrev:%x, size:%d", this, addrNext, addrPrev, SizeElem());
}


void DataSettings::Init()
{
    rec_point = TBase::InModeP2P() ? 0 : -1;

    en_a = SET_ENABLED_A ? 1U : 0U;
    en_b = SET_ENABLED_B ? 1U : 0U;
    inv_a = SET_INVERSE_A ? 1U : 0U;
    inv_b = SET_INVERSE_B ? 1U : 0U;
    range[0] = SET_RANGE_A;
    range[1] = SET_RANGE_B;
    rShiftA = (uint)SET_RSHIFT_A;
    rShiftB = (uint)SET_RSHIFT_B;
    tBase = SET_TBASE;
    tShift = SET_TSHIFT;
    coupleA = SET_COUPLE_A;
    coupleB = SET_COUPLE_B;
    e_points_in_channel = SET_ENUM_POINTS;
    trigLevA = (uint)TRIG_LEVEL_A;
    trigLevB = (uint)TRIG_LEVEL_B;
    peakDet = (uint)SET_PEAKDET;
    div_a = SET_DIVIDER_A;
    div_b = SET_DIVIDER_B;
}


int DataSettings::BytesInChannel() const
{
    int result = PointsInChannel();

    if (peakDet != 0)
    {
        result *= 2;
    }

    return result;
}


int DataSettings::PointsInChannel() const
{
    static const int num_points[ENUM_POINTS_FPGA::Count] =
    {
        281, 512, 1024
    };

    return (int)num_points[e_points_in_channel];
}


int16 DataSettings::GetRShift(Chan ch) const
{
    return (int16)((ch == ChA) ? rShiftA : rShiftB);
}


bool DataSettings::InModeP2P() const
{
    return (tBase >= TBase::MIN_P2P) && (rec_point != -1);
}


void DataSettings::AppendPoints(uint8 *a, uint8 *b, BitSet16 pointsA, BitSet16 pointsB)
{
    if (InModeP2P())
    {
        int max_bytes = BytesInChannel();

        if (rec_point == max_bytes - 1)
        {
            std::memmove(a, a + 1, (uint)(max_bytes - 1));
            rec_point = max_bytes - 2;
        }
        else if (rec_point == max_bytes)
        {
            std::memmove(a, a + 2, (uint)(max_bytes - 2));
            rec_point = max_bytes - 2;
        }

        if (en_a)
        {
            a[rec_point] = pointsA.byte0;
            a[rec_point + 1] = pointsA.byte1;
        }
        if (en_b)
        {
            b[rec_point] = pointsB.byte0;
            b[rec_point + 1] = pointsB.byte1;
        }

        rec_point += 2;
    }
}


bool DataSettings::Equal(const DataSettings &ds)
{
    return (en_a == ds.en_a) &&
           (en_b == ds.en_b) &&
           (inv_a == ds.inv_a) &&
           (inv_b == ds.inv_b) &&
           (range[0] == ds.range[0]) &&
           (range[1] == ds.range[1]) &&
           (rShiftA == ds.rShiftA) &&
           (rShiftB == ds.rShiftB) &&
           (tBase == ds.tBase) &&
           (tShift == ds.tShift) &&
           (coupleA == ds.coupleA) &&
           (coupleB == ds.coupleB) &&
           (trigLevA == ds.trigLevA) &&
           (trigLevB == ds.trigLevB) &&
           (div_a == ds.div_a) &&
           (div_b == ds.div_b) &&
           (peakDet == ds.peakDet);
}


void DataController::ResetFlags()
{
    dataDirect.need_calculate = true;
    dataLast.need_calculate = true;
    dataInternal.need_calculate = true;
}


DataStruct &DataController::GetData(TypeData::E type)
{
    if (type == TypeData::Direct)
    {
        FillDataDirect();

        return dataDirect;
    }
    else if (type == TypeData::Last)
    {
        FillDataLast();

        return dataLast;
    }
    else if (type == TypeData::Internal)
    {
        FillDataInternal();

        return dataInternal;
    }

    return dataDirect;
}


void DataController::FillDataDirect()
{
    if (!dataDirect.need_calculate)
    {
        return;
    }

    if (Storage::NumElements() == 0)
    {
        dataDirect.Reset();
    }
    else
    {
        dataDirect.GetFromStorage(0);

        if (SettingsDisplay::NumAverages() != 1 || TBase::InModeRandomizer())
        {
            dataDirect.GetAverage();
        }
    }

    dataDirect.need_calculate = false;
}


void DataController::FillDataLast()
{
    if (!dataLast.need_calculate)
    {
        return;
    }

    if (Storage::NumElements() == 0)
    {
        dataLast.Reset();
    }
    else
    {
        dataLast.GetFromStorage(PageMemory::Internal::currentSignal);

        if (SettingsDisplay::NumAverages() != 1)
        {
            dataLast.GetAverage();
        }
    }

    dataLast.need_calculate = false;
}


void DataController::FillDataInternal()
{
    if (!dataInternal.need_calculate)
    {
        return;
    }

    dataInternal.GetFromROM();

    dataInternal.need_calculate = false;
}
