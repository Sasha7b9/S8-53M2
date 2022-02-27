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
    uint8        *dataA = nullptr;
    uint8        *dataB = nullptr;
    DataSettings *DS = nullptr;

    uint8        *dataLastA = nullptr;
    uint8        *dataLastB = nullptr;
    DataSettings *dsLast = nullptr;

    uint8        *dataIntA = nullptr;
    uint8        *dataIntB = nullptr;
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
    DataSettings *firstElem = nullptr;

    // Указатель на последние сохранённые данные
    DataSettings *lastElem = nullptr;

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
    DataSettings* NextElem(DataSettings *elem);

    // Возвращает указатель на данные, отстоящие на indexFromEnd oт последнего сохранённого
    DataSettings* GetDataSettings(int indexFromEnd);

    // Возвращает true, если настройки измерений с индексами elemFromEnd0 и elemFromEnd1 совпадают, и false в ином случае.
    bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1);

    // Возващает true, если настройки в обоих структурах одинаковы
    bool SettingsIsEquals(DataSettings *dp0, DataSettings *dp1);

    // Очистка значений мин, макс и сумм
    void ClearLimitsAndSums();

    void CalculateLimits(uint8 *data0, uint8 *data1, DataSettings *dss);

    DataSettings* GetSettingsDataFromEnd(int fromEnd);

    // Копирует данные канала chan из, определяемые ds, в одну из двух строк массива dataImportRel. Возвращаемое
    // значение false означает, что данный канал выключен.
    bool CopyData(DataSettings *ds, Chan::E ch, Buffer<uint8> &datatImportRel);

    void CalculateAroundAverage(uint8 *data0, uint8 *data1, DataSettings *dss);
}


void DataSettings::PrintElement()
{
    LOG_WRITE("addr:%x, addrNext:%x, addrPrev:%x, size:%d", this, addrNext, addrPrev, SizeElem());
}


void Storage::Clear()
{
    firstElem = 0;
    lastElem = (DataSettings*)beginPool;
    lastElem->addrNext = lastElem->addrPrev = 0;
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
        float* aData0 = &aveData0[0];
        float* aData1 = &aveData1[0];
        uint8* d0 = &data0[0];
        uint8* d1 = &data1[0];
        float* endData = &aveData0[size];

        do 
        {
            *aData0 = ((*aData0) * numAveDataFless + (float)(*d0++)) * numAveDataInv;
            aData0++;
            *aData1 = ((*aData1) * numAveDataFless + (float)(*d1++)) * numAveDataInv;
            aData1++;
        } while (aData0 != endData);
    }
}


void Storage::AddData(uint8 *a, uint8 *b, DataSettings dss)
{
    dss.time = HAL_RTC::GetPackedTime();

    if(!dss.en_a && !dss.en_b)
    {
        return;
    }

    CalculateLimits(a, b, &dss);

    PushData(&dss, a, b);

    CalculateSums();

    CalculateAroundAverage(a, b, &dss);

    count_data++;
}


int Storage::CountDatas()
{
    return count_data;
}


void Storage::CalculateLimits(uint8 *data0, uint8 *data1, DataSettings *dss)
{
    uint numElements = (uint)dss->PointsInChannel();

    if(NumElementsInStorage() == 0 || NUM_MIN_MAX == 1 || (!SettingsIsEquals(dss, GetSettingsDataFromEnd(0))))
    {
        for(uint i = 0; i < numElements; i++)
        {
            limitDown[0][i] = limitUp[0][i] = data0[i];
            limitDown[1][i] = limitUp[1][i] = data1[i];
        }
    }
    else
    {
        int allDatas = NumElementsWithSameSettings();
        LIMITATION(allDatas, allDatas, 1, NUM_MIN_MAX);
        
        if(NumElementsWithSameSettings() >= NUM_MIN_MAX)
        {
            for(uint i = 0; i < numElements; i++)
            {
                limitDown[0][i] = limitUp[0][i] = data0[i];
                limitDown[1][i] = limitUp[1][i] = data1[i];
            }
            allDatas--;
        }

        for(int numData = 0; numData < allDatas; numData++)
        {
            const uint8 *dA = GetData(Chan::A, numData);
            const uint8 *dB = GetData(Chan::B, numData);

            for(uint i = 0; i < numElements; i++)
            {
                if(dA[i] < limitDown[0][i])  limitDown[0][i] = dA[i];
                if(dA[i] > limitUp[0][i])    limitUp[0][i] = dA[i];
                if(dB[i] < limitDown[1][i])  limitDown[1][i] = dB[i];
                if(dB[i] > limitUp[1][i])    limitUp[1][i] = dB[i];
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

    for(uint i = 0; i < numPoints; i++)
    {
        sum[0][i] = data0[i];
        sum[1][i] = data1[i];
    }
    if(numAveragings > 1)
    {
        int numSameSettins = NumElementsWithSameSettings();

        if(numSameSettins < numAveragings)
        {
            numAveragings = numSameSettins;
        }

        for(int i = 1; i < numAveragings; i++)
        {
            GetData(i, &ds, &data0, &data1);

            for(uint point = 0; point < numPoints; point++)
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
    int numElements = NumElementsInStorage();
    for(retValue = 1; retValue < numElements; retValue++)
    {
        if(!SettingsIsIdentical(retValue, retValue - 1))
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
    int numElements = NumElementsInStorage();

    for(retValue = 0; retValue < numElements; retValue++)
    {
        if(!SettingsIsEquals(&dp, GetDataSettings(retValue)))
        {
            break;
        }
    }
    return retValue;
}


int Storage::NumElementsInStorage()
{
    int retValue = 0;
    DataSettings *elem = firstElem;
    if(firstElem != 0)
    {
        if(firstElem == lastElem)
        {
            retValue = 1;
        }
        else
        {
            retValue++;
            while((elem = NextElem(elem)) != 0)
            {
                retValue++;
            }
        }
    }
    return retValue;
}


DataSettings* Storage::GetSettingsDataFromEnd(int fromEnd)
{
    return GetDataSettings(fromEnd);
}


bool Storage::GetData(int fromEnd, DataSettings **ds, uint8 **data0, uint8 **data1)
{
    static Buffer<uint8> dataImportRel[Chan::Count];

    DataSettings* dp = GetDataSettings(fromEnd);

    if(dp == 0)
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

    if(data0 != 0)
    {
        *data0 = CopyData(dp, Chan::A, dataImportRel[ChA]) ?  dataImportRel[ChA].Data() : nullptr;
    }
    
    size = dataImportRel[ChA].Size();

    if(data1 != 0)
    {
        *data1 = CopyData(dp, Chan::B, dataImportRel[ChB]) ? dataImportRel[ChB].Data() : nullptr;
    }
    
    size = dataImportRel[ChA].Size();

    *ds = dp;
    
    size = dataImportRel[ChA].Size();
    
    return true;
}


uint8* Storage::GetData(Chan::E ch, int fromEnd)
{
    static Buffer<uint8> dataImport[Chan::Count];

    DataSettings* dp = GetDataSettings(fromEnd);

    if(dp == 0)
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
    if((ch == Chan::A && !ds->en_a) || (ch == Chan::B && !ds->en_b))
    {
        return false;
    }

    uint8* address = ((uint8*)ds + sizeof(DataSettings));

    uint length = (uint)ds->BytesInChannel();

    if(ch == Chan::B && ds->en_a)
    {
        address += length;
    }

    memcpy(datatImportRel.Data(), address, length);

    return true;
}


uint8* Storage::GetAverageData(Chan::E ch)
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

    for(uint i = 0; i < numPoints; i++)
    {
        data[ch][i] = sum[ch][i] / numAveraging;
    }

    return &data[ch][0];
}


uint8* Storage::GetLimitation(Chan::E ch, int direction)
{
    uint8 *retValue = 0;

    if(direction == 0)
    {
        retValue = &(limitDown[ch][0]);
    }
    else if(direction == 1)
    {
        retValue = &(limitUp[ch][0]);
    }

    return retValue;
}


int Storage::NumberAvailableEntries()
{
    if(firstElem == 0)
    {
        return 0;
    }
    return SIZE_POOL / lastElem->SizeElem();
}


void Storage::PushData(DataSettings *dp, uint8 *data0, uint8 *data1)
{
    int required = dp->SizeElem();

    while(MemoryFree() < required)
    {
        RemoveFirstElement();
    }

    uint8* addrRecord = 0;

    if(firstElem == 0)
    {
        firstElem = (DataSettings*)beginPool;
        addrRecord = beginPool;
        dp->addrPrev = 0;
        dp->addrNext = 0;
    }
    else
    {
        addrRecord = (uint8*)lastElem + lastElem->SizeElem();

        if(addrRecord + dp->SizeElem() > endPool)
        {
            addrRecord = beginPool;
        }

        dp->addrPrev = lastElem;
        lastElem->addrNext = addrRecord;
        dp->addrNext = 0;
    }

    lastElem = (DataSettings*)addrRecord;

#define COPY_AND_INCREASE(address, data, length) memcpy((address), (data), (length)); address += (length);

    COPY_AND_INCREASE(addrRecord, dp, sizeof(DataSettings));

    uint bytes_in_channel = (uint)dp->BytesInChannel();

    if(dp->en_a)
    {
        COPY_AND_INCREASE(addrRecord, data0, bytes_in_channel);
    }
    if(dp->en_b)
    {
        COPY_AND_INCREASE(addrRecord, data1, bytes_in_channel);
    }
}


int Storage::MemoryFree()
{
    if (firstElem == 0)
    {
        return SIZE_POOL;
    }
    else if (firstElem == lastElem)
    {
        return (endPool - (uint8*)firstElem - (int)firstElem->SizeElem());
    }
    else if (firstElem < lastElem)
    {
        if ((uint8*)firstElem == beginPool)
        {
            return (endPool - (uint8*)lastElem - lastElem->SizeElem());
        }
        else
        {
            return (uint8*)firstElem - beginPool;
        }
    }
    else if (lastElem < firstElem)
    {
        return (uint8*)firstElem - (uint8*)lastElem - lastElem->SizeElem();
    }
    return 0;
}


int DataSettings::SizeElem()
{
    int retValue = sizeof(DataSettings);

    if(en_a)
    {
        retValue += BytesInChannel();
    }

    if(en_b)
    {
        retValue += BytesInChannel();
    }

    return retValue;
}


void Storage::RemoveFirstElement()
{
    firstElem = NextElem(firstElem);
    firstElem->addrPrev = 0;
    count_data--;
}


DataSettings* Storage::NextElem(DataSettings *elem)
{
    return (DataSettings*)elem->addrNext;
}


DataSettings* Storage::GetDataSettings(int indexFromEnd)
{
    if(firstElem == 0)
    {
        return 0;
    }
    int index = indexFromEnd;
    DataSettings *retValue = lastElem;
    while(index != 0 && ((retValue = (DataSettings *)retValue->addrPrev) != 0))
    {
        index--;
    }
    if(index != 0)
    {
        LOG_ERROR("Неправильный индекс %d, всего данных %d", indexFromEnd, CountDatas());
        return 0;
    }
    return retValue;
}


bool Storage::SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1)
{
    DataSettings* dp0 = GetDataSettings(elemFromEnd0);
    DataSettings* dp1 = GetDataSettings(elemFromEnd1);
    return SettingsIsEquals(dp0, dp1);
}


bool Storage::SettingsIsEquals(DataSettings *dp0, DataSettings *dp1)
{
    bool retValue = (dp0->en_a == dp1->en_a) &&
        (dp0->en_b     == dp1->en_b) &&
        (dp0->inv_a    == dp1->inv_a) &&
        (dp0->inv_b    == dp1->inv_b) &&
        (dp0->range[0] == dp1->range[0]) &&
        (dp0->range[1] == dp1->range[1]) &&
        (dp0->rShiftA  == dp1->rShiftA) &&
        (dp0->rShiftB  == dp1->rShiftB) &&
        (dp0->tBase    == dp1->tBase) &&
        (dp0->tShift   == dp1->tShift) &&
        (dp0->coupleA  == dp1->coupleA) &&
        (dp0->coupleB  == dp1->coupleB) &&
        (dp0->trigLevA == dp1->trigLevA) &&
        (dp0->trigLevB == dp1->trigLevB) &&
        (dp0->div_a    == dp1->div_a) &&
        (dp0->div_b    == dp1->div_b) &&
        (dp0->peakDet  == dp1->peakDet); 
    return retValue;
}


void Storage::P2P::CreateFrame(DataSettings ds)
{



    ds.last_point = 0;

    int num_bytes = ds.BytesInChannel();

    DataBuffer bufferA(num_bytes);
    DataBuffer bufferB(num_bytes);

    AddData(bufferA.Data(), bufferB.Data(), ds);
}


void Storage::P2P::Reset()
{

}


void Storage::P2P::AddPoints(BitSet16 bytesA, BitSet16 bytesB)
{

}
