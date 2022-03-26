#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_ROM.h"
#include <stm32f4xx_hal.h>


namespace HAL_ROM
{
    void WriteBufferBytes(uint address, uint8 *buffer, int size);
}


void HAL_ROM::EraseData()
{

}


bool HAL_ROM::GetData(int, DataFrame &)
{
    return false;
}


void HAL_ROM::DeleteData(int)
{

}

// ≈сли даннные есть, соответствующий элемент массива равен true/.
void HAL_ROM::GetDataInfo(bool [MAX_NUM_SAVED_WAVES])
{

}


void HAL_ROM::SaveData(int, DataStruct &)
{

}


void HAL_ROM::SaveData(int, const DataFrame &)
{

}


int OTP::GetSerialNumber(char buffer[17])
{
    // \todo улучшить - нельз€ разбрасыватьс€ байтами.  ажда€ запись должна занимать столько места, сколько в ней символов, а не 16, как сейчас.

    const int allShotsMAX = 512 / 16;   // ћаксимальное число записей в OPT серийного номера.

    uint8 *address = (uint8 *)FLASH_OTP_END - 15; //-V566

    do
    {
        address -= 16;
    } while (*address == 0xff && address > (uint8 *)FLASH_OTP_BASE); //-V566

    if (*address == 0xff)   // Ќе нашли строки с информацией, дойд€ до начального адреса OTP
    {
        buffer[0] = 0;
        return allShotsMAX;
    }

    std::strcpy(buffer, (char *)address);

    return allShotsMAX - (address - (uint8 *)FLASH_OTP_BASE) / 16 - 1; //-V566
}


bool OTP::SaveSerialNumber(char *serialNumber)
{
    // Ќаходим первую пустую строку длиной 16 байт в области OPT, начина€ с начала.
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
