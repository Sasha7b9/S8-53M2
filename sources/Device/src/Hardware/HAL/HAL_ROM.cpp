// 2022/02/11 17:50:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Hardware/HAL/HAL_ROM.h"
#include "Hardware/Sound.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Utils/GlobalFunctions.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Strings.h"
#include <stm32f4xx_hal.h>
#include <cstring>


#define CLEAR_FLAGS \
__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR)


struct RecordConfig
{
    uint addrData;  // ������� � ����� ������ �������� ������. ���� addrData == MAX_UINT, �� ��� ������ ������, ���� ����� ������ ������
    int  sizeData;  // ������ � ������ ���������� ������
};


#define SIZE_ARRAY_POINTERS_IN_ELEMENTS 1024
#define ADDR_ARRAY_POINTERS (ADDR_SECTOR_SETTINGS + 4)
#define SIZE_ARRAY_POINTERS_IN_BYTES (SIZE_ARRAY_POINTERS_IN_ELEMENTS * sizeof(RecordConfig))
static const uint ADDR_FIRST_SET = ADDR_ARRAY_POINTERS + SIZE_ARRAY_POINTERS_IN_BYTES;      // �� ����� ������ �������� ������ ���������
static const uint SIZE_MEMORY = 1024 * 1024 + 0x08000000;


// ������� ����, ��� ������ � ���� ������� ���� ��� �������������. ���� ������� ����� ������� (������, �������� ��� ��������) ����� ��� ��������, 
// ������ ��� ���� ����������� ��� ������� ���� ���
static const uint MARK_OF_FILLED = 0x1234;
static const uint MAX_UINT = 0xffffffff;

#define READ_WORD(address) (*((volatile uint*)(address)))



static const uint startDataInfo = ADDR_SECTOR_DATA_MAIN;



void HAL_ROM::PrepareSectorForData()
{
    EraseSector(ADDR_SECTOR_DATA_MAIN);
    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        WriteWord(startDataInfo + i * 4, 0);
    }
}


bool HAL_ROM::LoadSettings()
{
    /*
        1. �������� �� ������ ���������. ����������� ���, ��� � ������ ����� ������� �������� �������� MAX_UINT, ���� ��������� ��� �� �����������.
        2. �������� �� ������ ������ �������� ��������. ������������ ���, ��� � ������ ������ � ������ ����� ������� �������� �������� ��������
        MARK_OF_FILLED, � � ����� - ������ ��������� Settings (������ ����� ���� �� ���� � ��������� Settings).
        3. ���� ������ ������ - ������ �������� � ���������� � ����� �������.
            1. ����� ���������, ����� ������� ����� ����� ��������� ���������� ��������.
            2. ���� (����� + sizeof(Settings) >= ADDR_SECTORSETTINGS + (1024 * 128)), �� ��� �������� ���������� � ����� ��������� �������������
               ���������� ���������.
            3. ����� ����� ����� ��������� ����������� ��������, ������ �� �� ������ &set.display, ���������� � set.size ������ ������ ���������
            Settings � �������� Flash_SaveSettings().
    */

    CLEAR_FLAGS;

    if(TheFirstInclusion())                                         // ���� ��� ������ ���������
    {                                                               // �� ������ ��������������� �������������
        set.common.countErasedFlashSettings = 0;
        set.common.countEnables = 0;
        set.common.countErasedFlashData = 0;
        set.common.workingTimeInSecs = 0;
        PrepareSectorForData();
    }
    
    if(READ_WORD(ADDR_SECTOR_SETTINGS) == 0x12345)
    {
        EraseSector(ADDR_SECTOR_SETTINGS);
    }
    else if (READ_WORD(ADDR_SECTOR_SETTINGS) == MARK_OF_FILLED)                             // ���� ������ �������� �������� ��������
    {
        RecordConfig *record = RecordConfigForRead();
        if (record->sizeData + record->addrData >= (ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS))   // ���� ��������� ���������� ��������� �������
        {                                                                                   // �� ������� ������� (���� ���������� ������ ����������)
            --record;                                                                       // �� ������������� ����������� ����������� �����������
        }
        std::memcpy(&set, (const void *)(record->addrData - 4), (uint)record->sizeData);               // ��������� ��
        EraseSector(ADDR_SECTOR_SETTINGS);                                                  // ������� ������ ��������
        HAL_ROM::SaveSettings(true);                                                           // � ��������� ��������� � ����� �������
    }
    else
    {
        uint addressPrev = 0;
        uint address = ADDR_SECTOR_SETTINGS;
        while (READ_WORD(address) != MAX_UINT)  // ���� �� ����� ������ ���� ��������, �������� �� (-1) (���� ������������� ������)
        {
            addressPrev = address;              // ��������� ���� �����
            address += READ_WORD(address);      // � ��������� � ���������� ������������ ��������, ����������� �� ����� ������ (������ �������
        }                                       // ��������� Settings - � ������. ��� ��������� �������� ��������������� � ������, ���� ���������
                                                // �� ������
        
        if (addressPrev != 0)                   // ���� �� ����� ������ ���-�� ��������
        {
            std::memcpy(&set, (const void *)addressPrev, READ_WORD(addressPrev));    // �������� ���������� ���������
            return true;
        }
    }
    set.common.countEnables++;
    return false;
}


void HAL_ROM::WriteAddressDataInRecord(RecordConfig *record)
{
    uint address = (record == FirstRecord()) ? ADDR_FIRST_SET : (record - 1)->addrData + (record - 1)->sizeData;

    WriteWord((uint)(&record->addrData), address);
}


void HAL_ROM::SaveSettings(bool verifyLoadede)
{
    if (!verifyLoadede && !Settings::isLoaded)
    {
        return;
    }

    CLEAR_FLAGS;

    set.size = sizeof(set);

    uint address = ADDR_SECTOR_SETTINGS;                                        // ������� ������ ��������� ����

    while (READ_WORD(address) != MAX_UINT)
    {
        address += READ_WORD(address);
    }
                                                                                // � ���� ����� address ��������� �� ������ ������������ ����

    if (address + sizeof(set) >= (ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS)) // ���� ������� �����������, �� ��� ������ ������ ������ �� �������
    {                                                                           // �������
        EraseSector(ADDR_SECTOR_SETTINGS);                                      // � ���� ������ ������� ������ ��������
        address = ADDR_SECTOR_SETTINGS;                                         // � ��������� ��������� ����� ����� � ������ �������
    }

    WriteBufferBytes(address, (uint8 *)&set, sizeof(set));                      // � �������� ��������� ���������
}


bool HAL_ROM::TheFirstInclusion()
{
    return READ_WORD(ADDR_SECTOR_SETTINGS) == MAX_UINT;
}


RecordConfig* HAL_ROM::RecordConfigForRead()
{
    if (!TheFirstInclusion())
    {
        RecordConfig *record = FirstEmptyRecord();
        return --record;
    }

    return 0;
}


RecordConfig *HAL_ROM::FirstRecord()
{
    return (RecordConfig*)ADDR_ARRAY_POINTERS;
}


bool HAL_ROM::RecordExist()
{
    return READ_WORD(ADDR_ARRAY_POINTERS) != MAX_UINT;
}


RecordConfig *HAL_ROM::FirstEmptyRecord()
{
    RecordConfig *record = FirstRecord();
    int numRecord = 0;

    while (record->addrData != MAX_UINT)
    {
        record++;
        if ((++numRecord) == SIZE_ARRAY_POINTERS_IN_ELEMENTS)
        {
            return 0;
        }
    }

    return record;
}


uint HAL_ROM::CalculatFreeMemory()
{
    if (!RecordExist())
    {
        return SIZE_MEMORY - ADDR_FIRST_SET;
    }

    RecordConfig *firstEmptyRecord = FirstEmptyRecord();

    if (firstEmptyRecord == 0)  // ���� ��� ������ ������
    {
        return 0;
    }

    return SIZE_MEMORY - (firstEmptyRecord - 1)->addrData - (firstEmptyRecord - 1)->sizeData - 4;
}


uint HAL_ROM::FindAddressNextDataInfo()
{
    uint addressNextInfo = startDataInfo + MAX_NUM_SAVED_WAVES * 4;

    while (*((uint*)addressNextInfo) != 0xffffffff)
    {
        addressNextInfo = *((uint*)addressNextInfo) + MAX_NUM_SAVED_WAVES * 4;
    }

    return addressNextInfo;
}


uint HAL_ROM::FindActualDataInfo()
{
    return FindAddressNextDataInfo() - MAX_NUM_SAVED_WAVES * 4;
}


void HAL_ROM::GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES])
{
    uint address = FindActualDataInfo();

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        existData[i] = READ_WORD(address + i * 4) != 0;
    }
}


bool HAL_ROM::ExistData(int num)
{
    uint address = FindActualDataInfo();
    return READ_WORD(address + num * 4) != 0;
}


void HAL_ROM::DeleteData(int num)
{
    uint address = FindActualDataInfo();
    WriteWord(address + num * 4, 0);
}


void HAL_ROM::EraseData()
{
    CLEAR_FLAGS;

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = FLASH_SECTOR_8;
    flashITD.NbSectors = 2;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint error = 0;

    while(Sound::isBeep) {};

    HAL_FLASHEx_Erase(&flashITD, &error);

    PrepareSectorForData();

    HAL_FLASH_Lock();
}


int HAL_ROM::CalculateSizeData(DataSettings *ds)
{
    return (int)sizeof(DataSettings) + 2 * ds->BytesInChannel();
}


int HAL_ROM::FreeMemory()
{
    return (int)(ADDR_SECTOR_DATA_MAIN + 128 * 1024 - FindAddressNextDataInfo() - 1 - 4 * MAX_NUM_SAVED_WAVES - 3000);
}


void HAL_ROM::CompactMemory()
{
    Display::ClearFromWarnings();
    Display::ShowWarningGood(Warning::MovingData);
    Display::Update();
    uint dataInfoRel = FindActualDataInfo() - ADDR_SECTOR_DATA_MAIN;

    EraseSector(ADDR_SECTOR_DATA_HELP);
    WriteBufferBytes(ADDR_SECTOR_DATA_HELP, (uint8*)ADDR_SECTOR_DATA_MAIN, 1024 * 128); //-V566
    PrepareSectorForData();

    uint addressDataInfo = ADDR_SECTOR_DATA_HELP + dataInfoRel;

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        uint addrDataOld = READ_WORD(addressDataInfo + i * 4);

        if (addrDataOld != 0)
        {
            DataStruct data;

            uint addrDataNew = addrDataOld + 1024 * 128;

            data.ds.Set(*((DataSettings *)addrDataNew));

            addrDataNew += sizeof(DataSettings);

            data.A.FromBuffer((uint8 *)addrDataNew, data.ds.BytesInChannel());
            addrDataNew += data.ds.BytesInChannel();
 
            data.B.FromBuffer((uint8 *)addrDataNew, data.ds.BytesInChannel());

            HAL_ROM::SaveData(i, data);
        }
    }
    Display::ClearFromWarnings();
}


void HAL_ROM::SaveData(int num, DataStruct &data)
{
    /*
        1. ����� ���������� ���������� ������.
        2. ���� �� ������� ��� ������ ������ � ������� ���������� - ��������� ������.
        3. ������� ��������� ���������� ������ ����������.
        4. � ����� ��� ����� �����, � ������� ����� ��������� ����. ������ ����������.
        5. ���������� �� ��� ������.
        6. �� ������� ���������� ���������� ������ ����������.
    */

    /*
        �������� ���������� ������.
        ADDR_SECTOR_DATA_HELP ������������ ��� ���������� �������� ������, ����� ��������� �������� ADDR_SECTOR_DATA_MAIN � ������ ������ ��������� ������

        ������ ����������� � ������ ��������� �������.
    
        ���������� � ADDR_SECTOR_DATA_MAIN.

        uint[MAX_NUM_SAVED_WAVES] - ������, �� ������� �������� ��������������� �������. ���� 0 - ������ ����.
        uint - ����� ������ ��������� ������ ������ (���������� ������� �������). � �� ����� ������� ����� ������� �������.
    */

    if (ExistData(num))
    {
        DeleteData(num);
    }

    int size = CalculateSizeData(&data.ds);

// 2
    if (FreeMemory() < size)
    {
        CompactMemory();
    }

// 3
    uint addrDataInfo = FindActualDataInfo();          // ������� ������ ������������ ��������������� �������

// 4
    uint addrForWrite = addrDataInfo + MAX_NUM_SAVED_WAVES * 4;             // ��� - �����, �� �������� ����� ��������� ����� ���������� ��������������� �������
    uint valueForWrite = addrForWrite + 4 + size;                           // ��� - ����� ���������� ��������������� �������
    WriteWord(addrForWrite, valueForWrite);

// 5
    uint address = addrDataInfo + MAX_NUM_SAVED_WAVES * 4 + 4;              // �����, �� �������� ����� ������� ������ � �����������
    uint addressNewData = address;
    WriteBufferBytes(address, (uint8*)&data.ds, sizeof(DataSettings));            // ��������� ��������� �������
    address += sizeof(DataSettings);
    
    if (data.ds.en_a)
    {
        WriteBufferBytes(address, (uint8*)data.A.Data(), data.ds.BytesInChannel());     // ��������� ������ �����
        address += data.ds.BytesInChannel();
    }

    if (data.ds.en_b)
    {
        WriteBufferBytes(address, (uint8*)data.B.Data(), data.ds.BytesInChannel());     // ��������� ������ �����
        address += data.ds.BytesInChannel();
    }

// 6
    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        uint addressForWrite = address + i * 4;
        if (i == num)
        {
            WriteWord(addressForWrite, addressNewData);
        }
        else
        {
            WriteWord(addressForWrite, READ_WORD(addrDataInfo + i * 4));
        }
    }
}


bool HAL_ROM::GetData(int num, DataStruct &data)
{
    uint addrDataInfo = FindActualDataInfo();

    data.A.Realloc(0);
    data.B.Realloc(0);

    if (READ_WORD(addrDataInfo + 4 * num) == 0)
    {
        data.ds.valid = 0;

        return false;
    }

    uint addrDS = READ_WORD(addrDataInfo + 4 * num);

    data.ds.Set(*((DataSettings*)addrDS));

    if (data.ds.en_a)
    {
        uint address = addrDS + sizeof(DataSettings);

        SU::LogBufferU8((uint8 *)address, 10);
    }

    if (data.ds.en_b)
    {
        uint address = 0;

        if (data.ds.en_a)
        {
            address = addrDS + sizeof(DataSettings) + data.ds.BytesInChannel();
        }
        else
        {
            address = addrDS + sizeof(DataSettings);
        }

        data.B.FromBuffer((uint8 *)address, data.ds.BytesInChannel());
    }

    return true;
}


uint HAL_ROM::GetSector(uint startAddress)
{
    switch (startAddress)
    {
        case ADDR_SECTOR_DATA_MAIN:
            return FLASH_SECTOR_8;
        case ADDR_SECTOR_DATA_HELP:
            return FLASH_SECTOR_9;
        case ADDR_SECTOR_RESOURCES:
            return FLASH_SECTOR_10;
        case ADDR_SECTOR_SETTINGS:
            return FLASH_SECTOR_11;
    }

    LOG_ERROR_TRACE("������������ ������");

    return FLASH_SECTOR_11;
}


void HAL_ROM::EraseSector(uint startAddress)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
        FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = GetSector(startAddress);
    flashITD.NbSectors = 1;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint32_t error = 0;
    while (Sound::isBeep) {};
    HAL_FLASHEx_Erase(&flashITD, &error);

    HAL_FLASH_Lock();
}


void HAL_ROM::WriteWord(uint address, uint word)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
        FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_FLASH_Unlock();

    if (HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)word) != HAL_OK)
    {
        LOG_ERROR_TRACE("�� ���� �������� � ������");
    }

    HAL_FLASH_Lock();
}


void HAL_ROM::WriteBufferBytes(uint address, uint8 *buffer, int size)
{
    HAL_FLASH_Unlock();
    for (int i = 0; i < size; i++)
    {
        if (HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, (uint64_t)(buffer[i])) != HAL_OK)
        {
            ERROR_HANDLER();
        }
        address++;
    }
    HAL_FLASH_Lock();
}


bool OTP::SaveSerialNumber(char *serialNumber)
{
    // ������� ������ ������ ������ ������ 16 ���� � ������� OPT, ������� � ������.
    uint8 *address = (uint8*)FLASH_OTP_BASE; //-V566

    while ((*address) != 0xff &&                // *address != 0xff ��������, ��� ������ � ��� ������ ��� �������������
           address < (uint8*)FLASH_OTP_END - 16) //-V566
    {
        address += 16;
    }

    if (address < (uint8*)FLASH_OTP_END - 16) //-V566
    {
        HAL_ROM::WriteBufferBytes((uint)address, (uint8*)serialNumber, (int)std::strlen(serialNumber) + 1);
        return true;
    }

    return false;
}


int OTP::GetSerialNumber(char buffer[17])
{
    // \todo �������� - ������ �������������� �������. ������ ������ ������ �������� ������� �����, ������� � ��� ��������, � �� 16, ��� ������.

    const int allShotsMAX = 512 / 16;   // ������������ ����� ������� � OPT ��������� ������.

    uint8* address = (uint8*)FLASH_OTP_END - 15; //-V566

    do
    {
        address -= 16;
    } while(*address == 0xff && address > (uint8*)FLASH_OTP_BASE); //-V566

    if (*address == 0xff)   // �� ����� ������ � �����������, ����� �� ���������� ������ OTP
    {
        buffer[0] = 0;
        return allShotsMAX;
    }

    std::strcpy(buffer, (char*)address);

    return allShotsMAX - (address - (uint8*)FLASH_OTP_BASE) / 16 - 1; //-V566
}
