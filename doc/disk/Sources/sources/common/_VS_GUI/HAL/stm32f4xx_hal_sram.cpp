#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef HAL_SRAM_Init(SRAM_HandleTypeDef *hsram, FMC_NORSRAM_TimingTypeDef *Timing,
    FMC_NORSRAM_TimingTypeDef *ExtTiming)
{
    UNUSED(hsram);
    UNUSED(Timing);
    UNUSED(ExtTiming);

    return HAL_OK;
}