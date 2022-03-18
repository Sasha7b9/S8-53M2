// 2022/02/11 17:49:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Data/Storage.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Strings.h"
#include "Utils/Containers/Buffer.h"
#include "Data/DataExtensions.h"
#include <cstring>


namespace Storage
{
    WorkingFrame working;

    // Количество отведённой для измерений памяти.
    const int SIZE_POOL = (30 * 1024);

    // Здесь хранятся данные.
    uint8 pool[SIZE_POOL] = {0};

    // Адрес начала памяти для хранения
    uint8 *beginPool = &(pool[0]);

    // Адрес последнего байта памяти для хранения
    uint8 *endPool = &(pool[SIZE_POOL - 1]);

    // Максимальные значения каналов
    uint8 lim_up[Chan::Count][FPGA::MAX_POINTS * 2];

    // Минимальные значения каналов
    uint8 lim_down[Chan::Count][FPGA::MAX_POINTS * 2];

    // Указатель на первые сохранённые данные
    DataSettings *first_ds = nullptr;

    // Указатель на последние сохранённые данные
    DataSettings *last_ds = nullptr;

    // Всего данных сохранено
    int count_data = 0;

    // Возвращает количество свободной памяти в байтах
    int MemoryFree();

    // Удалить первое (самое старое) измерение
    void RemoveFirstFrame();

    // Удалить последнее (самое новое) измерение
    void RemoveLastFrame();

    // Подготовить новый фрейм для записи в него данных. Записывает в него данные из ds
    DataSettings *PrepareNewFrame(DataSettings &ds);

    // Возвращает true, если настройки измерений с индексами elemFromEnd0 и elemFromEnd1 совпадают, и false в ином случае.
    bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1);

    // Очистка значений мин, макс и сумм
    void ClearLimitsAndSums();

    void CalculateLimits(const DataSettings *, const uint8 *dataA, const uint8 *dataB);

    // Копирует данные канала chan из, определяемые ds, в одну из двух строк массива dataImportRel
    void CopyData(DataSettings *, Chan ch, BufferFPGA &);

    // тупо добавляет новый фрейм
    void AppendFrameP2P(DataSettings);
}


void DataSettings::PrintElement()
{
    LOG_WRITE("addr:%x, addrNext:%x, addrPrev:%x, size:%d", this, next, prev, SizeFrame());
}


void Storage::Clear()
{
    first_ds = nullptr;
    last_ds = (DataSettings *)beginPool;
    last_ds->next = last_ds->prev = nullptr;
    ClearLimitsAndSums();
}


void Storage::ClearLimitsAndSums()
{
    std::memset(lim_up[0], 0, FPGA::MAX_POINTS * 2);
    std::memset(lim_up[1], 0, FPGA::MAX_POINTS * 2);
    std::memset(lim_down[0], 0xff, FPGA::MAX_POINTS * 2);
    std::memset(lim_down[1], 0xff, FPGA::MAX_POINTS * 2);
}


void Storage::AddData(DataStruct &data)
{
    data.ds.time = HAL_RTC::GetPackedTime();

    CalculateLimits(&data.ds, data.A.Data(), data.B.Data());

    DataSettings *frame = PrepareNewFrame(data.ds);

    std::memcpy(frame->DataBegin(ChA), data.A.Data(), (uint)frame->BytesInChannel());

    std::memcpy(frame->DataBegin(ChB), data.B.Data(), (uint)frame->BytesInChannel());

    Averager::Append(frame);
}


void Storage::OpenFrame()
{
    DataSettings ds;
    ds.Init();

    PrepareNewFrame(ds);
}


void Storage::CloseFrame()
{
    DataSettings *ds = GetDataSettings(0);

    ds->time = HAL_RTC::GetPackedTime();

    CalculateLimits(ds, ds->DataBegin(ChA), ds->DataEnd(ChB));

    Averager::Append(ds);
}


int Storage::NumFrames()
{
    return count_data;
}


void Storage::CalculateLimits(const DataSettings *dss, const uint8 *a, const uint8 *b)
{
    uint numElements = (uint)dss->PointsInChannel();

    if (NumFrames() == 0 || NUM_MIN_MAX == 1 || (!GetDataSettings(0)->Equal(*dss)))
    {
        for (uint i = 0; i < numElements; i++)
        {
            lim_down[0][i] = lim_up[0][i] = a[i];
            lim_down[1][i] = lim_up[1][i] = b[i];
        }
    }
    else
    {
        int allDatas = NumFramesWithSameSettings();
        LIMITATION(allDatas, allDatas, 1, NUM_MIN_MAX);

        if (NumFramesWithSameSettings() >= NUM_MIN_MAX)
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
            const uint8 *dA = GetData(Chan::A, numData);
            const uint8 *dB = GetData(Chan::B, numData);

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


int Storage::NumFramesWithSameSettings()
{
    int retValue = 0;
    int numElements = NumFrames();
    for (retValue = 1; retValue < numElements; retValue++)
    {
        if (!SettingsIsIdentical(retValue, retValue - 1))
        {
            break;
        }
    }
    return retValue;
}


int Storage::NumFramesWithCurrentSettings()
{
    DataSettings dp;
    dp.Init();
    int retValue = 0;
    int numElements = NumFrames();

    for (retValue = 0; retValue < numElements; retValue++)
    {
        if (!GetDataSettings(retValue)->Equal(dp))
        {
            break;
        }
    }
    return retValue;
}


bool Storage::GetData(int fromEnd, DataStruct &data)
{
    DataSettings *dp = GetDataSettings(fromEnd);

    if (dp == nullptr)
    {
        data.ds.valid = 0;
        return false;
    }

    data.ds.Set(*dp);

    CopyData(dp, Chan::A, data.A);

    CopyData(dp, Chan::B, data.B);

    return true;
}


uint8 *Storage::GetData(Chan ch, int fromEnd)
{
    DataSettings *ds = GetDataSettings(fromEnd);

    if (ds == 0)
    {
        return nullptr;
    }

    uint8 *address = (uint8 *)ds + sizeof(DataSettings);

    return ch.IsA() ? address : (address + ds->BytesInChannel());
}


void Storage::CopyData(DataSettings *ds, Chan ch, BufferFPGA &data)
{
    uint8 *address = ((uint8 *)ds + sizeof(DataSettings));

    uint length = (uint)ds->BytesInChannel();

    if (ch.IsB())
    {
        address += length;
    }

    data.FromBuffer(address, (int)length);
}


uint8 *Storage::GetLimitation(Chan ch, int direction)
{
    uint8 *retValue = 0;

    if (direction == 0)
    {
        retValue = &(lim_down[ch][0]);
    }
    else if (direction == 1)
    {
        retValue = &(lim_up[ch][0]);
    }

    return retValue;
}


int Storage::NumberAvailableEntries()
{
    if (first_ds == nullptr)
    {
        return 0;
    }

    return SIZE_POOL / last_ds->SizeFrame();
}


DataSettings *Storage::PrepareNewFrame(DataSettings &ds)
{
    int required = ds.SizeFrame();

    while (MemoryFree() < required)
    {
        RemoveFirstFrame();
    }

    uint8 *addrRecord = nullptr;

    if (first_ds == nullptr)
    {
        first_ds = (DataSettings *)beginPool;
        addrRecord = beginPool;
        ds.prev = nullptr;
        ds.next = nullptr;
    }
    else
    {
        addrRecord = (uint8 *)last_ds + last_ds->SizeFrame();

        if (addrRecord + ds.SizeFrame() > endPool)
        {
            addrRecord = beginPool;
        }

        ds.prev = last_ds;
        last_ds->next = addrRecord;
        ds.next = nullptr;
    }

    last_ds = (DataSettings *)addrRecord;

    std::memcpy(addrRecord, &ds, sizeof(DataSettings));

    count_data++;

    return last_ds;
}


int Storage::MemoryFree()
{
    if (first_ds == nullptr)
    {
        return SIZE_POOL;
    }
    else if (first_ds == last_ds)
    {
        return (endPool - (uint8 *)first_ds - (int)first_ds->SizeFrame());
    }
    else if (first_ds < last_ds)
    {
        if ((uint8 *)first_ds == beginPool)
        {
            return (endPool - (uint8 *)last_ds - last_ds->SizeFrame());
        }
        else
        {
            return (uint8 *)first_ds - beginPool;
        }
    }
    else if (last_ds < first_ds)
    {
        return (uint8 *)first_ds - (uint8 *)last_ds - last_ds->SizeFrame();
    }
    return 0;
}


int DataSettings::SizeFrame()
{
    return (int)sizeof(DataSettings) + 2 * BytesInChannel();
}


void Storage::RemoveFirstFrame()
{
    if (first_ds)
    {
        first_ds = (DataSettings *)first_ds->next;
        first_ds->prev = nullptr;
        count_data--;
    }
}


void Storage::RemoveLastFrame()
{
    if (last_ds)
    {
        if (last_ds->prev)
        {
            DataSettings *ds = (DataSettings *)last_ds->prev;
            ds->next = nullptr;
        }
        else
        {
            last_ds = nullptr;
            first_ds = nullptr;
        }

        count_data--;
    }
}


DataSettings *Storage::GetDataSettings(int indexFromEnd)
{
    if (first_ds == nullptr)
    {
        return nullptr;
    }

    int index = indexFromEnd;
    DataSettings *ds = last_ds;

    while (index != 0 && ((ds = (DataSettings *)ds->prev) != 0))
    {
        index--;
    }

    if (index != 0)
    {
//        LOG_ERROR("Неправильный индекс %d, всего данных %d", indexFromEnd, NumElements());      // \todo После сброса настроек здесь срабатывает
        return nullptr;
    }

    return ds;
}


bool Storage::SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1)
{
    DataSettings *dp0 = GetDataSettings(elemFromEnd0);
    DataSettings *dp1 = GetDataSettings(elemFromEnd1);

    return dp0->Equal(*dp1);
}


void Storage::AppendFrameP2P(DataSettings ds)
{
    int num_bytes = ds.BytesInChannel();

    DataStruct data;
    data.ds.Set(ds);
    data.A.Realloc(num_bytes, ValueFPGA::NONE);
    data.A.Realloc(num_bytes, ValueFPGA::NONE);

    AddData(data);
}


void WorkingFrame::AppendPoints(BitSet16 pointsA, BitSet16 pointsB)
{
    int max_bytes = BytesInChannel();

    uint8 *a = (uint8 *)this + max_bytes;
    uint8 *b = a + max_bytes;

    if (rec_point == max_bytes - 1)
    {
        std::memmove(a, a + 1, (uint)(max_bytes - 1));
        std::memmove(b, b + 1, (uint)(max_bytes - 1));
        rec_point = max_bytes - 2;
    }
    else if (rec_point == max_bytes)
    {
        std::memmove(a, a + 2, (uint)(max_bytes - 2));
        std::memmove(b, b + 2, (uint)(max_bytes - 2));
        rec_point = max_bytes - 2;
    }

    a[rec_point] = pointsA.byte0;
    a[rec_point + 1] = pointsA.byte1;

    b[rec_point] = pointsB.byte0;
    b[rec_point + 1] = pointsB.byte1;

    rec_point += 2;
    all_points += 2;
}
