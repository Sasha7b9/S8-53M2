#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_ROM.h"
#include "Settings/Settings.h"
#include "Hardware/Sound.h"
#include <stm32f4xx_hal.h>


/*
*   �������� �������� ������ �����
*   � ������� 12 ��������������� �������� ��������� ������ ����:
*   {
*     uint addr   - �����, �� �������� ������� ����� ������
*     uint number - ����� ������, ��� �� ����������� �� ������ ������-����� ��
*     uint size   - ������� ����� �������� ����� ������ (DataSettings + 2 * BytesInChannelStored()). ���� 0, �� ������ �����
*   }
*   ���� ������ �������� � �������� 21, 22, 23
*   ��� ����� ������ ������ ������ ������������ ������ �������
*/


#define CLEAR_FLAGS \
__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR)


// ��������� � ����������� ������
#define ADDR_SECTOR_0               ((uint)0x08000000)  //  0 16k
#define ADDR_SECTOR_1               ((uint)0x08004000)  //  1 16k
#define ADDR_SECTOR_2               ((uint)0x08008000)  //  2 16k
#define ADDR_SECTOR_3               ((uint)0x0800C000)  //  3 16k
#define ADDR_SECTOR_4               ((uint)0x08010000)  //  4 64k
#define ADDR_FIRMWARE_1             ((uint)0x08020000)  //  5 128k \-
#define ADDR_FIRMWARE_2             ((uint)0x08040000)  //  6 128k |- �������� ��������
#define ADDR_FIRMWARE_3             ((uint)0x08060000)  //  7 128k /-
#define ADDR_SECTOR_8               ((uint)0x08080000)  //  8 128k
#define ADDR_SECTOR_9               ((uint)0x080A0000)  //  9 128k
#define ADDR_SECTOR_10              ((uint)0x080C0000)  // 10 128k

// ���������
#define ADDR_SECTOR_SETTINGS        ((uint)0x080E0000)  // 11 128k
#define SIZE_SECTOR_SETTINGS        (128 * 1024)

#define ADDR_SECTOR_DATA_INFO       ((uint)0x08100000)  // 12 16k  ���������� � ���������� ������
#define ADDR_SECTOR_13              ((uint)0x08104000)  // 13 16k
#define ADDR_SECTOR_14              ((uint)0x08108000)  // 14 16k
#define ADDR_SECTOR_15              ((uint)0x0810�000)  // 15 16k
#define ADDR_SECTOR_16              ((uint)0x08110000)  // 16 64k
#define ADDR_SECTOR_17              ((uint)0x08120000)  // 17 128k
#define ADDR_SECTOR_18              ((uint)0x08140000)  // 18 128k
#define ADDR_SECTOR_19              ((uint)0x08160000)  // 19 128k
#define ADDR_SECTOR_20              ((uint)0x08180000)  // 20 128k
#define ADDR_SECTOR_DATA_1          ((uint)0x081A0000)  // 21 128k
#define ADDR_SECTOR_DATA_2          ((uint)0x081C0000)  // 22 128k
#define ADDR_SECTOR_DATA_3          ((uint)0x081E0000)  // 23 128k


#define READ_WORD(address) (*((volatile uint*)(address)))

// ������� ����, ��� ������ � ���� ������� ���� ��� �������������. ���� ������� ����� ������� (������, �������� ��� ��������) ����� ��� ��������, 
// ������ ��� ���� ����������� ��� ������� ���� ���
static const uint MARK_OF_FILLED = 0x1234;
static const uint MAX_UINT = 0xffffffff;


#define SIZE_ARRAY_POINTERS_IN_ELEMENTS 1024
#define ADDR_ARRAY_POINTERS (ADDR_SECTOR_SETTINGS + 4)


namespace HAL_ROM
{
    void WriteBufferBytes(uint address, uint8 *buffer, int size);

    // ���������� true, ���� ��� ������ ���������
    bool TheFirstInclusion();

    void EraseSector(uint startAddress);

    void WriteWord(uint address, uint);
    void WriteWord(void *address, uint);

    uint GetSector(uint startAddress);

    struct RecordConfig
    {
        uint addrData;  // ������� � ����� ������ �������� ������. ���� addrData == MAX_UINT, �� ��� ������ ������, ���� ����� ������ ������
        int  sizeData;  // ������ � ������ ���������� ������

        static RecordConfig *ForRead();
        static RecordConfig *FirstEmpty();
        static RecordConfig *FirstRecord();
    };

    namespace Data
    {
        struct StructInfo
        {
            uint address;   // �� ����� ������ ��������� ������ ������
            int number;     // ���� ����� ��������� �� �������� ������-����� ��
            uint size;      // ������ ����� ������ (DataStruct + 2 * BytesInChannelStored()). ���� 0, �� ������ �����

            // ���������� true, ���� ��������� ���������� � ��������� �� �������� ������ (���� ���������, ���� ������ ��� ����)
            bool Exist() const
            {
                return (address != MAX_UINT);
            }

            // ���������� true, ���� ����� ����
            bool Erased() const
            {
                return Exist() && (size == 0);
            }

            // ���������� ��������� �� ������ ������� DataStructROM. ������ ������� ������ ��������� � ������ ������� ADDR_SECTOR_DATA_INFO
            static StructInfo *First()
            {
                return (StructInfo *)ADDR_SECTOR_DATA_INFO;
            }

            // ��������� ������� (����� ������� ADDR_SECTOR_DATA_INFO)
            static StructInfo *Latest()
            {
                return (StructInfo *)(ADDR_SECTOR_DATA_INFO + 16 * 1024);
            }

            // ���������� ��������� �� ���������� � ������ � ������� num. ���� ����������� - nullptr
            static StructInfo *Get(int num)
            {
                StructInfo *info = First();

                while (info < Latest())
                {
                    if (!info->Exist())
                    {
                        return nullptr;
                    }

                    if (!info->Erased())
                    {
                        if (info->number == num)
                        {
                            return info;
                        }
                    }

                    info++;
                }

                return nullptr;
            }
        };
    }
}


void HAL_ROM::Data::EraseAll()
{
    EraseSector(ADDR_SECTOR_DATA_INFO);

    EraseSector(ADDR_SECTOR_DATA_1);
    EraseSector(ADDR_SECTOR_DATA_2);
    EraseSector(ADDR_SECTOR_DATA_3);
}


bool HAL_ROM::Data::Get(int, DataStruct &)
{
    return false;
}


void HAL_ROM::Data::Delete(int num)
{
    StructInfo *info = StructInfo::Get(num);

    if (info)
    {
        WriteWord(&info->size, 0);
    }
}

// ���� ������� ����, ��������������� ������� ������� ����� true/.
void HAL_ROM::Data::GetInfo(bool [MAX_NUM_SAVED_WAVES])
{

}


void HAL_ROM::Data::Save(int, DataStruct &)
{

}


bool HAL_ROM::Settings::Load()
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

    if (TheFirstInclusion())                                         // ���� ��� ������ ���������
    {                                                               // �� ������ ��������������� �������������
        set.common.countErasedFlashSettings = 0;
        set.common.countEnables = 0;
        set.common.countErasedFlashData = 0;
        set.common.workingTimeInSecs = 0;
    }

    if (READ_WORD(ADDR_SECTOR_SETTINGS) == 0x12345)
    {
        EraseSector(ADDR_SECTOR_SETTINGS);
    }
    else if (READ_WORD(ADDR_SECTOR_SETTINGS) == MARK_OF_FILLED)                             // ���� ������ �������� �������� ��������
    {
        RecordConfig *record = RecordConfig::ForRead();
        if (record->sizeData + record->addrData >= (ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS))   // ���� ��������� ���������� ��������� �������
        {                                                                                   // �� ������� ������� (���� ���������� ������ ����������)
            --record;                                                                       // �� ������������� ����������� ����������� �����������
        }
        std::memcpy(&set, (const void *)(record->addrData - 4), (uint)record->sizeData);               // ��������� ��
        EraseSector(ADDR_SECTOR_SETTINGS);                                                  // ������� ������ ��������
        HAL_ROM::Settings::Save(true);                                                           // � ��������� ��������� � ����� �������
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


void HAL_ROM::Settings::Save(bool verifyLoadede)
{
    if (!verifyLoadede && !::Settings::isLoaded)
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


HAL_ROM::RecordConfig *HAL_ROM::RecordConfig::ForRead()
{
    if (!TheFirstInclusion())
    {
        RecordConfig *record = FirstEmpty();
        return --record;
    }

    return 0;
}


HAL_ROM::RecordConfig *HAL_ROM::RecordConfig::FirstEmpty()
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


void HAL_ROM::WriteWord(uint address, uint word)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
        FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_FLASH_Unlock();

    while (Sound::isBeep) {};

    if (HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)word) != HAL_OK)
    {
        LOG_ERROR_TRACE("�� ���� �������� � ������");
    }

    HAL_FLASH_Lock();
}


void HAL_ROM::WriteWord(void *address, uint word)
{
    WriteWord((uint)address, word);
}


uint HAL_ROM::GetSector(uint startAddress)
{
    switch (startAddress)
    {
    case ADDR_SECTOR_SETTINGS:  return FLASH_SECTOR_11;
    case ADDR_SECTOR_DATA_INFO: return FLASH_SECTOR_12;
    case ADDR_SECTOR_DATA_1:    return FLASH_SECTOR_21;
    case ADDR_SECTOR_DATA_2:    return FLASH_SECTOR_22;
    case ADDR_SECTOR_DATA_3:    return FLASH_SECTOR_23;
    }

    LOG_ERROR_TRACE("������������ ������");

    return FLASH_SECTOR_11;
}


HAL_ROM::RecordConfig *HAL_ROM::RecordConfig::FirstRecord()
{
    return (RecordConfig *)ADDR_ARRAY_POINTERS;
}


int OTP::GetSerialNumber(char buffer[17])
{
    // \todo �������� - ������ �������������� �������. ������ ������ ������ �������� ������� �����, ������� � ��� ��������, � �� 16, ��� ������.

    const int allShotsMAX = 512 / 16;   // ������������ ����� ������� � OPT ��������� ������.

    uint8 *address = (uint8 *)FLASH_OTP_END - 15; //-V566

    do
    {
        address -= 16;
    } while (*address == 0xff && address > (uint8 *)FLASH_OTP_BASE); //-V566

    if (*address == 0xff)   // �� ����� ������ � �����������, ����� �� ���������� ������ OTP
    {
        buffer[0] = 0;
        return allShotsMAX;
    }

    std::strcpy(buffer, (char *)address);

    return allShotsMAX - (address - (uint8 *)FLASH_OTP_BASE) / 16 - 1; //-V566
}


bool OTP::SaveSerialNumber(char *serialNumber)
{
    // ������� ������ ������ ������ ������ 16 ���� � ������� OPT, ������� � ������.
    uint8 *address = (uint8 *)FLASH_OTP_BASE; //-V566

    while ((*address) != 0xff &&                // *address != 0xff ��������, ��� ������ � ��� ������ ��� �������������
        address < (uint8 *)FLASH_OTP_END - 16) //-V566
    {
        address += 16;
    }

    if (address < (uint8 *)FLASH_OTP_END - 16) //-V566
    {
        HAL_ROM::WriteBufferBytes((uint)address, (uint8 *)serialNumber, (int)std::strlen(serialNumber) + 1);
        return true;
    }

    return false;
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
