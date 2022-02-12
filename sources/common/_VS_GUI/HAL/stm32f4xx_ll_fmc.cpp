#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef  FMC_NORSRAM_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_InitTypeDef *Init)
{
    UNUSED(Device);
    UNUSED(Init);

    return HAL_OK;
}


HAL_StatusTypeDef  FMC_NORSRAM_Timing_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank)
{
    UNUSED(Device);
    UNUSED(Timing);
    UNUSED(Bank);

    return HAL_OK;
}