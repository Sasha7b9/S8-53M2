#include "defines.h"
#include "Hardware/Sound.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Memory/Sector.h"
#include <stm32f4xx_hal.h>

#define  _16K (16 * 1024)
#define  _64K (64 * 1024)
#define _128K (128 * 1024)

#ifdef USE_HAL_DRIVER
#include "Hardware/HAL/HAL_ROM_stm.cpp"
#else
#include "Hardware/HAL/HAL_ROM_win.cpp"
#endif


void HAL_ROM::WriteBytes(uint address, const uint8 *data, uint size)
{
    CLEAR_FLASH_FLAGS;

    HAL_FLASH_Unlock();

    for (uint i = 0; i < size; i++)
    {
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, data[i]);
        address++;
    }

    HAL_FLASH_Lock();
}


void HAL_ROM::Fill(uint address, uint8 value, int size)
{
    CLEAR_FLASH_FLAGS;

    HAL_FLASH_Unlock();

    for(int i = 0; i < size; i++)
    {
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, value);
        address++;
    }

    HAL_FLASH_Lock();
}


void HAL_ROM::WriteBufferBytes(uint address, const void *buffer, uint size)
{
#ifdef DEVICE
    Sound::WaitForCompletion();
#endif

    CLEAR_FLASH_FLAGS

    HAL_FLASH_Unlock();

    for (uint i = 0; i < size; i++)
    {
        uint64_t data = static_cast<uint8 *>(const_cast<void *>(buffer))[i];
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, data);
        address++;
    }

    HAL_FLASH_Lock();
}
