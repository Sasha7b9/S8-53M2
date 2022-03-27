#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_ROM.h"
#include "Settings/Settings.h"
#include "Hardware/Sound.h"
#include <stm32f4xx_hal.h>


/*
*   Алгоритм хранения данных такой
*   В секторе 12 последовательно хранятся структуры такого вида:
*   {
*     uint addr   - адрес, по которому сохранён фрейм данных
*     uint number - номер данных, как он представлен на экране ПАМЯТЬ-ВНУТР ЗУ
*     uint size   - сколько места занимает фрейм данных (DataSettings + 2 * BytesInChannelStored()). Если 0, то запись стёрта
*   }
*   Сами данные хранятся в секторах 21, 22, 23
*   Все байты одного фрейма всегда принадлежать одному сектору
*/


#define CLEAR_FLAGS \
__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR)


// Программа и константные данные
#define ADDR_SECTOR_0               ((uint)0x08000000)  //  0 16k
#define ADDR_SECTOR_1               ((uint)0x08004000)  //  1 16k
#define ADDR_SECTOR_2               ((uint)0x08008000)  //  2 16k
#define ADDR_SECTOR_3               ((uint)0x0800C000)  //  3 16k
#define ADDR_SECTOR_4               ((uint)0x08010000)  //  4 64k
#define ADDR_FIRMWARE_1             ((uint)0x08020000)  //  5 128k \-
#define ADDR_FIRMWARE_2             ((uint)0x08040000)  //  6 128k |- Основная прошивка
#define ADDR_FIRMWARE_3             ((uint)0x08060000)  //  7 128k /-
#define ADDR_SECTOR_8               ((uint)0x08080000)  //  8 128k
#define ADDR_SECTOR_9               ((uint)0x080A0000)  //  9 128k
#define ADDR_SECTOR_10              ((uint)0x080C0000)  // 10 128k

// Настройки
#define ADDR_SECTOR_SETTINGS        ((uint)0x080E0000)  // 11 128k
#define SIZE_SECTOR_SETTINGS        (128 * 1024)

#define ADDR_SECTOR_DATA_INFO       ((uint)0x08100000)  // 12 16k  Информация о сохранённых данных
#define ADDR_SECTOR_13              ((uint)0x08104000)  // 13 16k
#define ADDR_SECTOR_14              ((uint)0x08108000)  // 14 16k
#define ADDR_SECTOR_15              ((uint)0x0810С000)  // 15 16k
#define ADDR_SECTOR_16              ((uint)0x08110000)  // 16 64k
#define ADDR_SECTOR_17              ((uint)0x08120000)  // 17 128k
#define ADDR_SECTOR_18              ((uint)0x08140000)  // 18 128k
#define ADDR_SECTOR_19              ((uint)0x08160000)  // 19 128k
#define ADDR_SECTOR_20              ((uint)0x08180000)  // 20 128k
#define ADDR_SECTOR_DATA_FIRST      ((uint)0x081A0000)  // 21 128k
#define ADDR_SECTOR_DATA_2          ((uint)0x081C0000)  // 22 128k
#define ADDR_SECTOR_DATA_LATEST     ((uint)0x081E0000)  // 23 128k


#define READ_WORD(address) (*((volatile uint*)(address)))

// Признак того, что запись в этоу область флэш уже производилась. Если нулевое слово области (данных, ресурсов или настроек) имеет это значение, 
// запись уже была произведена как минимум один раз
static const uint MARK_OF_FILLED = 0x1234;
static const uint MAX_UINT = 0xffffffff;


#define SIZE_ARRAY_POINTERS_IN_ELEMENTS 1024
#define ADDR_ARRAY_POINTERS (ADDR_SECTOR_SETTINGS + 4)


namespace HAL_ROM
{
    void WriteBufferBytes(uint address, uint8 *buffer, int size);

    // Возвращает true, если это первое включение
    bool TheFirstInclusion();

    void EraseSector(uint startAddress);

    void WriteWord(uint address, uint);
    void WriteWord(void *address, uint);

    uint GetSector(uint startAddress);

    namespace Settings
    {
        struct RecordConfig
        {
            uint addrData;  // Начиная с этого адреса записаны данные. Если addrData == MAX_UINT, то это пустая запись, сюда можно писать данные
            int  sizeData;  // Размер в байтах записанных данных

            static RecordConfig *ForRead();
            static RecordConfig *FirstEmpty();
            static RecordConfig *FirstRecord();
        };
    }

    namespace Data
    {
        namespace SectorInfo
        {
            // Возвращает true, если заполнены все информационные поля (сектор ADDR_SECTOR_DATA_INFO)
            bool Filled();

        }

        namespace SectorData
        {
            // Возвращает начало свободного места для хранения данных
            static uint StartFreeeSpace();

            static uint Begin()
            {
                return ADDR_SECTOR_DATA_FIRST;
            }

            static uint End()
            {
                return (ADDR_SECTOR_DATA_LATEST + 128 * 1024);
            }

            // Возвращает true, если address принадлежит сектору
            static bool BelongAddress(uint address)
            {
                return (address >= Begin()) && (address < End());
            }
        }

        struct StructInfo
        {
            uint address;   // По этому адресу сохранены данные фрейма
            int number;     // Этот номер выводится на странице ПАМЯТЬ-ВНУТР ЗУ
            uint size;      // Размер фрема данных (DataStruct + 2 * BytesInChannelStored()). Если 0, то запись стёрта


            // Возвращает true, если структура существует и указывает на реальный объект (либо указывала, если объект был стёрт)
            bool Exist() const
            {
                return (address != MAX_UINT);
            }


            // Возвращает true, если фрейм стёрт
            bool Erased() const
            {
                return Exist() && (size == 0);
            }


            // Возвращает указатель на первый элемент DataStructROM. Первый элемент всегда находится в начале сектора ADDR_SECTOR_DATA_INFO
            static StructInfo *First()
            {
                return (StructInfo *)ADDR_SECTOR_DATA_INFO;
            }


            // Последний элемент (конец сектора ADDR_SECTOR_DATA_INFO)
            static StructInfo *Latest()
            {
                return (StructInfo *)(ADDR_SECTOR_DATA_INFO + 16 * 1024);
            }


            // Возвращает указатель на последнюю существующую структуру. nullptr, если структур не существует
            static StructInfo *LatestExist()
            {
                StructInfo *info = First();

                if (!info->Exist())
                {
                    return nullptr;
                }

                while (info->Exist())
                {
                    if (info == Latest())
                    {
                        return info;
                    }

                    info++;
                }

                return info - 1;
            }


            // Первое свободное поле. Сюда будем сохранять следующие данные
            static StructInfo *FirstEmpty()
            {
                if (!LatestExist())
                {
                    return StructInfo::First();
                }

                if (SectorInfo::Filled())
                {
                    return nullptr;
                }

                return LatestExist() + 1;
            }


            // Возвращает указатель на информацию о данных с номером num. Если отсутствует - nullptr
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


            DataSettings *GetDataSettings()
            {
                return (DataSettings *)address;
            }


            uint8 *GetDataChannel(Chan ch)
            {
                DataSettings *ds = GetDataSettings();

                uint8 *address_data = (uint8 *)ds + sizeof(DataSettings);

                if (ch.IsB())
                {
                    address_data += ds->BytesInChanStored();
                }

                return address_data;
            }


            // Сохраняет данные в данную структуру. Она должна быть пустая
            void SaveData(const DataStruct &data)
            {
                if (Exist())
                {
                    LOG_ERROR("Структура уже заполнена");
                    return;
                }

                if ((uint)this < ADDR_SECTOR_DATA_INFO)
                {
                    LOG_ERROR_TRACE("StructInfo вне сектора записи");
                    return;
                }

                if ((uint)this + sizeof(StructInfo) > (ADDR_SECTOR_DATA_INFO + 16 * 1024))
                {
                    LOG_ERROR_TRACE("StructInfo вне сектора записи");
                    return;
                }

                uint address_start = SectorData::StartFreeeSpace();         // Начальный адрес записи

                if (!SectorData::BelongAddress(address_start) && !SectorData::BelongAddress(address_start + data.ds.SizeFrame() - 1))
                {
                    LOG_ERROR_TRACE("Данные вне сектора записи");
                    return;
                }


            }
        };

        // true, если в области данных есть место для сохранения DataStruct
        static bool ExistPlaceToSave(const DataStruct &);

    }
}


void HAL_ROM::Data::EraseAll()
{
    EraseSector(ADDR_SECTOR_DATA_INFO);

    EraseSector(ADDR_SECTOR_DATA_FIRST);
    EraseSector(ADDR_SECTOR_DATA_2);
    EraseSector(ADDR_SECTOR_DATA_LATEST);
}


void HAL_ROM::Data::Delete(int num)
{
    StructInfo *info = StructInfo::Get(num);

    if (info)
    {
        WriteWord(&info->size, 0);
    }
}

// Если даннные есть, соответствующий элемент массива равен true/.
void HAL_ROM::Data::GetInfo(bool info[MAX_DATAS])
{
    for (int i = 0; i < MAX_DATAS; i++)
    {
        info[i] = (StructInfo::Get(i) != nullptr);
    }
}


// Возвращает true, если заполнены все информационные поля (сектор ADDR_SECTOR_DATA_INFO)
static bool HAL_ROM::Data::SectorInfo::Filled()
{
    StructInfo *info = StructInfo::First();

    while (info->Exist())
    {
        info++;

        if (info == StructInfo::Latest())
        {
            return true;
        }
    }

    return false;
}


static bool HAL_ROM::Data::ExistPlaceToSave(const DataStruct &data)
{
    StructInfo *latest = StructInfo::LatestExist();

    if (!latest)
    {
        return true;
    }

    return ((SectorData::StartFreeeSpace() + data.ds.SizeFrame()) < SectorData::End());
}


static uint HAL_ROM::Data::SectorData::StartFreeeSpace()
{
    StructInfo *info = StructInfo::LatestExist();

    if (!info)
    {
        return Begin();
    }

    return info->address + info->GetDataSettings()->SizeFrame();
}


bool HAL_ROM::Data::Get(int num, DataStruct &data)
{
    StructInfo *info = StructInfo::Get(num);

    if (info)
    {
        data.ds = *info->GetDataSettings();

        int num_bytes = data.ds.BytesInChanStored();

        data.A.ReallocFromBuffer(info->GetDataChannel(ChA), num_bytes);
        data.B.ReallocFromBuffer(info->GetDataChannel(ChB), num_bytes);

        return true;
    }

    return false;
}


void HAL_ROM::Data::Save(int num, DataStruct &data)
{
    Delete(num);

    if (!StructInfo::FirstEmpty())
    {
        EraseAll();
    }

    if (!ExistPlaceToSave(data))
    {
        EraseAll();
    }

    StructInfo *info = StructInfo::FirstEmpty();

    info->SaveData(data);
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
    }

    if (READ_WORD(ADDR_SECTOR_SETTINGS) == 0x12345)
    {
        EraseSector(ADDR_SECTOR_SETTINGS);
    }
    else if (READ_WORD(ADDR_SECTOR_SETTINGS) == MARK_OF_FILLED)                             // Если старый алгоритм хранения настроек
    {
        RecordConfig *record = RecordConfig::ForRead();
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


HAL_ROM::Settings::RecordConfig *HAL_ROM::Settings::RecordConfig::ForRead()
{
    if (!TheFirstInclusion())
    {
        RecordConfig *record = FirstEmpty();
        return --record;
    }

    return nullptr;
}


HAL_ROM::Settings::RecordConfig *HAL_ROM::Settings::RecordConfig::FirstEmpty()
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
        LOG_ERROR_TRACE("Не могу записать в память");
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
    case ADDR_SECTOR_SETTINGS:      return FLASH_SECTOR_11;
    case ADDR_SECTOR_DATA_INFO:     return FLASH_SECTOR_12;
    case ADDR_SECTOR_DATA_FIRST:    return FLASH_SECTOR_21;
    case ADDR_SECTOR_DATA_2:        return FLASH_SECTOR_22;
    case ADDR_SECTOR_DATA_LATEST:   return FLASH_SECTOR_23;
    }

    LOG_ERROR_TRACE("Недопустимый сектор");

    return FLASH_SECTOR_11;
}


HAL_ROM::Settings::RecordConfig *HAL_ROM::Settings::RecordConfig::FirstRecord()
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
