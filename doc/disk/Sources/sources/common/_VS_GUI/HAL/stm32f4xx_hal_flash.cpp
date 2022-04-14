#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *, uint32_t *)
{
    return HAL_ERROR;
}


HAL_StatusTypeDef HAL_FLASH_Program(uint32_t, uint32_t, uint64_t)
{
    return HAL_ERROR;
}


HAL_StatusTypeDef HAL_FLASH_Unlock()
{
    return HAL_ERROR;
}


HAL_StatusTypeDef HAL_FLASH_Lock()
{
    return HAL_ERROR;
}
