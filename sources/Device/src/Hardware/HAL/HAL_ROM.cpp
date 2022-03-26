#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_ROM.h"
#include "Settings/Settings.h"
#include "Hardware/Sound.h"
#include <stm32f4xx_hal.h>


#define CLEAR_FLAGS \
__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR)


// Программа и константные данные
#define ADDR_FLASH_SECTOR_0     ((uint)0x08000000)  // Base @ of Sector 0, 16 Kbytes
#define ADDR_FLASH_SECTOR_1     ((uint)0x08004000)  // Base @ of Sector 1, 16 Kbytes
#define ADDR_FLASH_SECTOR_2     ((uint)0x08008000)  // Base @ of Sector 2, 16 Kbytes
#define ADDR_FLASH_SECTOR_3     ((uint)0x0800C000)  // Base @ of Sector 3, 16 Kbytes
#define ADDR_FLASH_SECTOR_4     ((uint)0x08010000)  // Base @ of Sector 4, 64 Kbytes
#define ADDR_FLASH_SECTOR_5     ((uint)0x08020000)  // Base @ of Sector 5, 128 Kbytes  |
#define ADDR_FLASH_SECTOR_6     ((uint)0x08040000)  // Base @ of Sector 6, 128 Kbytes  | Основная прошивка
#define ADDR_FLASH_SECTOR_7     ((uint)0x08060000)  // Base @ of Sector 7, 128 Kbytes  /

// Сохранённые данные
#define ADDR_SECTOR_DATA_MAIN   ((uint)0x08080000)  // Base @ of Sector 8, 128 Kbytes
#define ADDR_SECTOR_DATA_HELP   ((uint)0x080A0000)  // Base @ of Sector 9, 128 Kbytes

// Графические и звуковые ресурсы
#define ADDR_SECTOR_RESOURCES   ((uint)0x080C0000)  // Base @ of Sector 10, 128 Kbytes

// Настройки
#define ADDR_SECTOR_SETTINGS    ((uint)0x080E0000)  // Base @ of Sector 11, 128 Kbytes
#define SIZE_SECTOR_SETTINGS    (128 * 1024)


#define READ_WORD(address) (*((volatile uint*)(address)))

// Признак того, что запись в этоу область флэш уже производилась. Если нулевое слово области (данных, ресурсов или настроек) имеет это значение, 
// запись уже была произведена как минимум один раз
static const uint MARK_OF_FILLED = 0x1234;
static const uint MAX_UINT = 0xffffffff;


#define SIZE_ARRAY_POINTERS_IN_ELEMENTS 1024
#define ADDR_ARRAY_POINTERS (ADDR_SECTOR_SETTINGS + 4)


namespace HAL_ROM
{
    static const uint startDataInfo = ADDR_SECTOR_DATA_MAIN;

    void WriteBufferBytes(uint address, uint8 *buffer, int size);

    bool TheFirstInclusion();

    void PrepareSectorForData();

    void EraseSector(uint startAddress);

    void WriteWord(uint address, uint word);

    uint GetSector(uint startAddress);

    struct RecordConfig
    {
        uint addrData;  // Начиная с этого адреса записаны данные. Если addrData == MAX_UINT, то это пустая запись, сюда можно писать данные
        int  sizeData;  // Размер в байтах записанных данных

        static RecordConfig *GetForRead();
        static RecordConfig *GetFirstEmpty();
        static RecordConfig *GetFirstRecord();
    };
}


void HAL_ROM::Data::EraseData()
{

}


bool HAL_ROM::Data::GetData(int, DataFrame &)
{
    return false;
}


void HAL_ROM::Data::DeleteData(int)
{

}

// Если даннные есть, соответствующий элемент массива равен true/.
void HAL_ROM::Data::GetDataInfo(bool [MAX_NUM_SAVED_WAVES])
{

}


void HAL_ROM::Data::SaveData(int, DataStruct &)
{

}


void HAL_ROM::Data::SaveData(int, const DataFrame &)
{

}


bool HAL_ROM::Settings::Load()
{
    /*
        1. Проверка на первое включение. Выполняется тем, что в первом слове сектора настроек хранится MAX_UINT, если настройки ещё не сохранялись.
        2. Проверка на старую версию хранения настроек. Определяется тем, что в старой версии в первом слове сектора настроек хранится значение
        MARK_OF_FILLED, а в новой - размер структуры Settings (раньше этого поля не было в структуре Settings).
        3. Если старая версия - чтение настроек и сохранение в новом формате.
            1. Чтобы прочитать, нужно сначала найти адрес последних сохранённых настроек.
            2. Если (адрес + sizeof(Settings) >= ADDR_SECTORSETTINGS + (1024 * 128)), то эти нстройки повреждены и нужно считывать предпоследние
               сохранённые настройки.
            3. Когда нашли адрес последних действующих настроек, читаем их по адресу &set.display, записываем в set.size полный размер структуры
            Settings и вызываем Flash_SaveSettings().
    */

    CLEAR_FLAGS;

    if (TheFirstInclusion())                                         // Если это первое включение
    {                                                               // то делаем предварительные приготовления
        set.common.countErasedFlashSettings = 0;
        set.common.countEnables = 0;
        set.common.countErasedFlashData = 0;
        set.common.workingTimeInSecs = 0;
        PrepareSectorForData();
    }

    if (READ_WORD(ADDR_SECTOR_SETTINGS) == 0x12345)
    {
        EraseSector(ADDR_SECTOR_SETTINGS);
    }
    else if (READ_WORD(ADDR_SECTOR_SETTINGS) == MARK_OF_FILLED)                             // Если старый алгоритм хранения настроек
    {
        RecordConfig *record = RecordConfig::GetForRead();
        if (record->sizeData + record->addrData >= (ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS))   // Если последние сохранённые настройки выходят
        {                                                                                   // за пределы сектора (глюк предыдущей версии сохранения)
            --record;                                                                       // то воспользуемся предыдущими сохранёнными настройками
        }
        std::memcpy(&set, (const void *)(record->addrData - 4), (uint)record->sizeData);               // Считываем их
        EraseSector(ADDR_SECTOR_SETTINGS);                                                  // Стираем сектор настроек
        HAL_ROM::Settings::Save(true);                                                           // И сохраняем настройки в новом формате
    }
    else
    {
        uint addressPrev = 0;
        uint address = ADDR_SECTOR_SETTINGS;
        while (READ_WORD(address) != MAX_UINT)  // Пока по этому адресу есть значение, отличное от (-1) (сюда производилась запись)
        {
            addressPrev = address;              // сохраняем этот адрес
            address += READ_WORD(address);      // И переходим к следующему прибавлением значения, хранящегося по этому адресу (первый элемент
        }                                       // структуры Settings - её размер. Все настройки хранятся последовательно в памяти, одна структура
                                                // за другой

        if (addressPrev != 0)                   // Если по этому адресу что-то записано
        {
            std::memcpy(&set, (const void *)addressPrev, READ_WORD(addressPrev));    // Счтываем сохранённые настройки
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

    uint address = ADDR_SECTOR_SETTINGS;                                        // Находим первый свободный байт

    while (READ_WORD(address) != MAX_UINT)
    {
        address += READ_WORD(address);
    }
    // В этой точке address указывает на первый незаписанный байт

    if (address + sizeof(set) >= (ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS)) // Если условие выполняется, то при записи данные выйдут за пределы
    {                                                                           // сектора
        EraseSector(ADDR_SECTOR_SETTINGS);                                      // В этом случае стираем сектор настроек
        address = ADDR_SECTOR_SETTINGS;                                         // и сохранять настройки будем прямо в начало сектора
    }

    WriteBufferBytes(address, (uint8 *)&set, sizeof(set));                      // И банально сохраняем настройки
}


bool HAL_ROM::TheFirstInclusion()
{
    return READ_WORD(ADDR_SECTOR_SETTINGS) == MAX_UINT;
}


void HAL_ROM::PrepareSectorForData()
{
    EraseSector(ADDR_SECTOR_DATA_MAIN);

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        WriteWord(startDataInfo + i * 4, 0);
    }
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


HAL_ROM::RecordConfig *HAL_ROM::RecordConfig::GetForRead()
{
    if (!TheFirstInclusion())
    {
        RecordConfig *record = GetFirstEmpty();
        return --record;
    }

    return 0;
}


HAL_ROM::RecordConfig *HAL_ROM::RecordConfig::GetFirstEmpty()
{
    RecordConfig *record = GetFirstRecord();
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

    if (HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)word) != HAL_OK)
    {
        LOG_ERROR_TRACE("Не могу записать в память");
    }

    HAL_FLASH_Lock();
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

    LOG_ERROR_TRACE("Недопустимый сектор");

    return FLASH_SECTOR_11;
}


HAL_ROM::RecordConfig *HAL_ROM::RecordConfig::GetFirstRecord()
{
    return (RecordConfig *)ADDR_ARRAY_POINTERS;
}


int OTP::GetSerialNumber(char buffer[17])
{
    // \todo улучшить - нельзя разбрасываться байтами. Каждая запись должна занимать столько места, сколько в ней символов, а не 16, как сейчас.

    const int allShotsMAX = 512 / 16;   // Максимальное число записей в OPT серийного номера.

    uint8 *address = (uint8 *)FLASH_OTP_END - 15; //-V566

    do
    {
        address -= 16;
    } while (*address == 0xff && address > (uint8 *)FLASH_OTP_BASE); //-V566

    if (*address == 0xff)   // Не нашли строки с информацией, дойдя до начального адреса OTP
    {
        buffer[0] = 0;
        return allShotsMAX;
    }

    std::strcpy(buffer, (char *)address);

    return allShotsMAX - (address - (uint8 *)FLASH_OTP_BASE) / 16 - 1; //-V566
}


bool OTP::SaveSerialNumber(char *serialNumber)
{
    // Находим первую пустую строку длиной 16 байт в области OPT, начиная с начала.
    uint8 *address = (uint8 *)FLASH_OTP_BASE; //-V566

    while ((*address) != 0xff &&                // *address != 0xff означает, что запись в эту строку уже производилась
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
