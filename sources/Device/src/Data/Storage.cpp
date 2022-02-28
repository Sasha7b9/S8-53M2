// 2022/02/11 17:49:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
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
#include <string.h>


namespace Storage
{
    uint8 *dataA = nullptr;
    uint8 *dataB = nullptr;
    DataSettings *DS = nullptr;

    uint8 *dataLastA = nullptr;
    uint8 *dataLastB = nullptr;
    DataSettings *dsLast = nullptr;

    uint8 *dataIntA = nullptr;
    uint8 *dataIntB = nullptr;
    DataSettings *dsInt = nullptr;

    // Количество отведённой для измерений памяти.
    const int SIZE_POOL = (30 * 1024);

    // Здесь хранятся данные.
    uint8 pool[SIZE_POOL] = {0};

    // Адрес начала памяти для хранения
    uint8 *beginPool = &(pool[0]);

    // Адрес последнего байта памяти для хранения
    uint8 *endPool = &(pool[SIZE_POOL - 1]);

    // Здесь хранятся суммы измерений обоих каналов
    uint sum[Chan::Count][FPGA::MAX_POINTS * 2];

    // Максимальные значения каналов
    uint8 limitUp[Chan::Count][FPGA::MAX_POINTS * 2];

    // Минимальные значения каналов
    uint8 limitDown[Chan::Count][FPGA::MAX_POINTS * 2];

    // Указатель на первые сохранённые данные
    DataSettings *first_ds = nullptr;

    // Указатель на последние сохранённые данные
    DataSettings *last_ds = nullptr;

    // Всего данных сохранено
    int count_data = 0;

    // В этих массивах хранятся усреднённые значения, подсчитанные по приблизительному алгоритму.
    float aveData0[FPGA::MAX_POINTS * 2] = {0.0f};

    float aveData1[FPGA::MAX_POINTS * 2] = {0.0f};
    // Если true, то новые суммы рассчитаны, и нужно повторить расчёт среднего
    bool newSumCalculated[Chan::Count] = {true, true};

    void CalculateSums();

    // Возвращает количество свободной памяти в байтах
    int MemoryFree();

    // Удалить первое (самое старое) измерение
    void RemoveFirstElement();

    // Сохранить данные
    void PushData(DataSettings *dp, uint8 *data0, uint8 *data1);

    // Возвращает указатель на измерение, следующее за elem
    DataSettings *NextElem(DataSettings *elem);

    // Возвращает указатель на данные, отстоящие на indexFromEnd oт последнего сохранённого
    DataSettings *GetDataSettings(int fromEnd = 0);

    // Возвращает true, если настройки измерений с индексами elemFromEnd0 и elemFromEnd1 совпадают, и false в ином случае.
    bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1);

    // Очистка значений мин, макс и сумм
    void ClearLimitsAndSums();

    void CalculateLimits(uint8 *data0, uint8 *data1, DataSettings *dss);

    // Копирует данные канала chan из, определяемые ds, в одну из двух строк массива dataImportRel. Возвращаемое
    // значение false означает, что данный канал выключен.
    bool CopyData(DataSettings *ds, Chan::E ch, Buffer<uint8> &datatImportRel);

    void CalculateAroundAverage(uint8 *data0, uint8 *data1, DataSettings *dss);

    namespace P2P
    {
        // тупо добавляет новый фрейм
        void AppendFrame(DataSettings);
    }
}


void DataSettings::PrintElement()
{
    LOG_WRITE("addr:%x, addrNext:%x, addrPrev:%x, size:%d", this, addrNext, addrPrev, SizeElem());
}


void Storage::Clear()
{
    first_ds = 0;
    last_ds = (DataSettings *)beginPool;
    last_ds->addrNext = last_ds->addrPrev = 0;
    ClearLimitsAndSums();
}


void Storage::ClearLimitsAndSums()
{
    memset(limitUp[0], 0, FPGA::MAX_POINTS);
    memset(limitUp[1], 0, FPGA::MAX_POINTS);
    memset(limitDown[0], 0xff, FPGA::MAX_POINTS);
    memset(limitDown[1], 0xff, FPGA::MAX_POINTS);
    memset(&(sum[0][0]), 0, Chan::Count * FPGA::MAX_POINTS * sizeof(uint));
}


void Storage::CalculateAroundAverage(uint8 *data0, uint8 *data1, DataSettings *dss)
{
    int numAveData = NumElementsWithCurrentSettings();

    int size = dss->BytesInChannel();

    if (numAveData == 1)
    {
        for (int i = 0; i < size; i++)
        {
            aveData0[i] = data0[i];
            aveData1[i] = data1[i];
        }
    }
    else
    {
        if (numAveData > SettingsDisplay::NumAverages())
        {
            numAveData = SettingsDisplay::NumAverages();
        }

        float numAveDataF = numAveData;
        float numAveDataFless = numAveDataF - 1.0f;
        float numAveDataInv = 1.0f / numAveDataF;
        float *aData0 = &aveData0[0];
        float *aData1 = &aveData1[0];
        uint8 *d0 = &data0[0];
        uint8 *d1 = &data1[0];
        float *endData = &aveData0[size];

        do
        {
            *aData0 = ((*aData0) * numAveDataFless + (float)(*d0++)) * numAveDataInv;
            aData0++;
            *aData1 = ((*aData1) * numAveDataFless + (float)(*d1++)) * numAveDataInv;
            aData1++;
        } while (aData0 != endData);
    }
}


void Storage::AddData(DataSettings dss, uint8 *a, uint8 *b)
{
    dss.time = HAL_RTC::GetPackedTime();

    if (!dss.en_a && !dss.en_b)
    {
        return;
    }

    CalculateLimits(a, b, &dss);

    PushData(&dss, a, b);

    CalculateSums();

    CalculateAroundAverage(a, b, &dss);

    count_data++;
}


int Storage::NumElements()
{
    return count_data;
}


void Storage::CalculateLimits(uint8 *data0, uint8 *data1, DataSettings *dss)
{
    uint numElements = (uint)dss->PointsInChannel();

    if (NumElements() == 0 || NUM_MIN_MAX == 1 || (!GetDataSettings()->Equal(*dss)))
    {
        for (uint i = 0; i < numElements; i++)
        {
            limitDown[0][i] = limitUp[0][i] = data0[i];
            limitDown[1][i] = limitUp[1][i] = data1[i];
        }
    }
    else
    {
        int allDatas = NumElementsWithSameSettings();
        LIMITATION(allDatas, allDatas, 1, NUM_MIN_MAX);

        if (NumElementsWithSameSettings() >= NUM_MIN_MAX)
        {
            for (uint i = 0; i < numElements; i++)
            {
                limitDown[0][i] = limitUp[0][i] = data0[i];
                limitDown[1][i] = limitUp[1][i] = data1[i];
            }
            allDatas--;
        }

        for (int numData = 0; numData < allDatas; numData++)
        {
            const uint8 *dA = GetData(Chan::A, numData);
            const uint8 *dB = GetData(Chan::B, numData);

            for (uint i = 0; i < numElements; i++)
            {
                if (dA[i] < limitDown[0][i])  limitDown[0][i] = dA[i];
                if (dA[i] > limitUp[0][i])    limitUp[0][i] = dA[i];
                if (dB[i] < limitDown[1][i])  limitDown[1][i] = dB[i];
                if (dB[i] > limitUp[1][i])    limitUp[1][i] = dB[i];
            }
        }
    }
}


void Storage::CalculateSums()
{
    DataSettings *ds = 0;
    uint8 *data0 = 0;
    uint8 *data1 = 0;

    GetData(0, &ds, &data0, &data1);

    uint numPoints = (uint)ds->BytesInChannel();

    int numAveragings = 0;

    if (TBase::InModeRandomizer())
    {
        numAveragings = NUM_AVE_FOR_RAND;
    }

    if (SettingsDisplay::NumAverages() > numAveragings)
    {
        numAveragings = SettingsDisplay::NumAverages();
    }

    for (uint i = 0; i < numPoints; i++)
    {
        sum[0][i] = data0[i];
        sum[1][i] = data1[i];
    }
    if (numAveragings > 1)
    {
        int numSameSettins = NumElementsWithSameSettings();

        if (numSameSettins < numAveragings)
        {
            numAveragings = numSameSettins;
        }

        for (int i = 1; i < numAveragings; i++)
        {
            GetData(i, &ds, &data0, &data1);

            for (uint point = 0; point < numPoints; point++)
            {
                sum[0][point] += data0[point];
                sum[1][point] += data1[point];
            }
        }
    }
    newSumCalculated[0] = newSumCalculated[1] = true;
}


int Storage::NumElementsWithSameSettings()
{
    int retValue = 0;
    int numElements = NumElements();
    for (retValue = 1; retValue < numElements; retValue++)
    {
        if (!SettingsIsIdentical(retValue, retValue - 1))
        {
            break;
        }
    }
    return retValue;
}


int Storage::NumElementsWithCurrentSettings()
{
    DataSettings dp;
    dp.Init();
    int retValue = 0;
    int numElements = NumElements();

    for (retValue = 0; retValue < numElements; retValue++)
    {
        if (!GetDataSettings(retValue)->Equal(dp))
        {
            break;
        }
    }
    return retValue;
}


bool Storage::GetData(int fromEnd, DataSettings **ds, uint8 **data0, uint8 **data1)
{
    static Buffer<uint8> dataImportRel[Chan::Count];

    DataSettings *dp = GetDataSettings(fromEnd);

    if (dp == nullptr)
    {
        return false;
    }

    volatile int size = dataImportRel[ChA].Size();
    size = size;

    volatile int bytes = dp->BytesInChannel();
    bytes = bytes;

    if (dataImportRel[ChA].Size() != dp->BytesInChannel())
    {
        dataImportRel[ChA].Realloc(dp->BytesInChannel());
        dataImportRel[ChB].Realloc(dp->BytesInChannel());
    }

    size = dataImportRel[ChA].Size();

    if (data0 != 0)
    {
        *data0 = CopyData(dp, Chan::A, dataImportRel[ChA]) ? dataImportRel[ChA].Data() : nullptr;
    }

    size = dataImportRel[ChA].Size();

    if (data1 != 0)
    {
        *data1 = CopyData(dp, Chan::B, dataImportRel[ChB]) ? dataImportRel[ChB].Data() : nullptr;
    }

    size = dataImportRel[ChA].Size();

    *ds = dp;

    size = dataImportRel[ChA].Size();

    return true;
}


uint8 *Storage::GetData(Chan::E ch, int fromEnd)
{
    static Buffer<uint8> dataImport[Chan::Count];

    DataSettings *dp = GetDataSettings(fromEnd);

    if (dp == 0)
    {
        return 0;
    }

    if (dataImport[ChA].Size() != dp->BytesInChannel())
    {
        dataImport[ChA].Realloc(dp->BytesInChannel());
        dataImport[ChB].Realloc(dp->BytesInChannel());
    }

    return CopyData(dp, ch, dataImport[ch]) ? dataImport[ch].Data() : nullptr;
}


bool Storage::CopyData(DataSettings *ds, Chan::E ch, Buffer<uint8> &datatImportRel)
{
    if ((ch == Chan::A && !ds->en_a) || (ch == Chan::B && !ds->en_b))
    {
        return false;
    }

    uint8 *address = ((uint8 *)ds + sizeof(DataSettings));

    uint length = (uint)ds->BytesInChannel();

    if (ch == Chan::B && ds->en_a)
    {
        address += length;
    }

    memcpy(datatImportRel.Data(), address, length);

    return true;
}


uint8 *Storage::GetAverageData(Chan::E ch)
{
    static uint8 data[Chan::Count][FPGA::MAX_POINTS * 2];

    if (newSumCalculated[ch] == false)
    {
        return &data[ch][0];
    }

    newSumCalculated[ch] = false;

    DataSettings *ds = 0;
    uint8 *d0, *d1;
    GetData(0, &ds, &d0, &d1);

    if (ds == 0)
    {
        return 0;
    }

    uint numPoints = (uint)ds->BytesInChannel();

    if (SettingsDisplay::GetModeAveraging() == ModeAveraging::Around)
    {
        float *floatAveData = (ch == Chan::A) ? aveData0 : aveData1;

        for (uint i = 0; i < numPoints; i++)
        {
            data[ch][i] = (uint8)(floatAveData[i]);
        }
        return &data[ch][0];
    }

    int numAveraging = SettingsDisplay::NumAverages();

    LIMIT_ABOVE(numAveraging, NumElementsWithSameSettings());

    for (uint i = 0; i < numPoints; i++)
    {
        data[ch][i] = sum[ch][i] / numAveraging;
    }

    return &data[ch][0];
}


uint8 *Storage::GetLimitation(Chan::E ch, int direction)
{
    uint8 *retValue = 0;

    if (direction == 0)
    {
        retValue = &(limitDown[ch][0]);
    }
    else if (direction == 1)
    {
        retValue = &(limitUp[ch][0]);
    }

    return retValue;
}


int Storage::NumberAvailableEntries()
{
    if (first_ds == nullptr)
    {
        return 0;
    }

    return SIZE_POOL / last_ds->SizeElem();
}


void Storage::PushData(DataSettings *dp, uint8 *data0, uint8 *data1)
{
    int required = dp->SizeElem();

    while (MemoryFree() < required)
    {
        RemoveFirstElement();
    }

    uint8 *addrRecord = 0;

    if (first_ds == 0)
    {
        first_ds = (DataSettings *)beginPool;
        addrRecord = beginPool;
        dp->addrPrev = 0;
        dp->addrNext = 0;
    }
    else
    {
        addrRecord = (uint8 *)last_ds + last_ds->SizeElem();

        if (addrRecord + dp->SizeElem() > endPool)
        {
            addrRecord = beginPool;
        }

        dp->addrPrev = last_ds;
        last_ds->addrNext = addrRecord;
        dp->addrNext = 0;
    }

    last_ds = (DataSettings *)addrRecord;

#define COPY_AND_INCREASE(address, data, length) memcpy((address), (data), (length)); address += (length);

    COPY_AND_INCREASE(addrRecord, dp, sizeof(DataSettings));

    uint bytes_in_channel = (uint)dp->BytesInChannel();

    if (dp->en_a)
    {
        COPY_AND_INCREASE(addrRecord, data0, bytes_in_channel);
    }
    if (dp->en_b)
    {
        COPY_AND_INCREASE(addrRecord, data1, bytes_in_channel);
    }
}


int Storage::MemoryFree()
{
    if (first_ds == nullptr)
    {
        return SIZE_POOL;
    }
    else if (first_ds == last_ds)
    {
        return (endPool - (uint8 *)first_ds - (int)first_ds->SizeElem());
    }
    else if (first_ds < last_ds)
    {
        if ((uint8 *)first_ds == beginPool)
        {
            return (endPool - (uint8 *)last_ds - last_ds->SizeElem());
        }
        else
        {
            return (uint8 *)first_ds - beginPool;
        }
    }
    else if (last_ds < first_ds)
    {
        return (uint8 *)first_ds - (uint8 *)last_ds - last_ds->SizeElem();
    }
    return 0;
}


int DataSettings::SizeElem()
{
    int retValue = sizeof(DataSettings);

    if (en_a)
    {
        retValue += BytesInChannel();
    }

    if (en_b)
    {
        retValue += BytesInChannel();
    }

    return retValue;
}


void Storage::RemoveFirstElement()
{
    first_ds = NextElem(first_ds);
    first_ds->addrPrev = 0;
    count_data--;
}


DataSettings *Storage::NextElem(DataSettings *elem)
{
    return (DataSettings *)elem->addrNext;
}


DataSettings *Storage::GetDataSettings(int indexFromEnd)
{
    if (first_ds == nullptr)
    {
        return nullptr;
    }

    int index = indexFromEnd;
    DataSettings *ds = last_ds;

    while (index != 0 && ((ds = (DataSettings *)ds->addrPrev) != 0))
    {
        index--;
    }

    if (index != 0)
    {
        LOG_ERROR("Неправильный индекс %d, всего данных %d", indexFromEnd, NumElements());
        return 0;
    }

    return ds;
}


bool Storage::SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1)
{
    DataSettings *dp0 = GetDataSettings(elemFromEnd0);
    DataSettings *dp1 = GetDataSettings(elemFromEnd1);

    return dp0->Equal(*dp1);
}


void Storage::P2P::CreateFrame(DataSettings _ds)
{
    if (Storage::NumElements() == 0)
    {
        AppendFrame(_ds);
    }
    else
    {
        DataSettings *ds = GetDataSettings(0);

        if (ds->InModeP2P())
        {
            if (ds->Equal(_ds))
            {
                ds->last_point = 0;
            }
            else
            {
                AppendFrame(_ds);
            }
        }
        else
        {
            AppendFrame(_ds);
        }
    }
}


void Storage::P2P::AppendFrame(DataSettings ds)
{
    ds.last_point = 0;

    int num_bytes = ds.BytesInChannel();

    DataBuffer bufferA(num_bytes);
    DataBuffer bufferB(num_bytes);

    bufferA.Fill(ValueFPGA::NONE);
    bufferB.Fill(ValueFPGA::NONE);

    AddData(ds, bufferA.Data(), bufferB.Data());
}


void Storage::P2P::Reset()
{
    DataSettings *ds = GetDataSettings();

    if (ds)
    {
        ds->last_point = 0;
    }
}


void Storage::P2P::AddPoints(BitSet16 bytesA, BitSet16 bytesB)
{
    DataSettings *ds = GetDataSettings();

    if (ds)
    {

    }
}
