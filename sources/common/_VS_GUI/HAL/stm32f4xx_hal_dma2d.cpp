#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef HAL_DMA2D_Init(DMA2D_HandleTypeDef *hdma2d)
{
    UNUSED(hdma2d);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_DMA2D_Start(DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width, uint32_t Height)
{
    UNUSED(hdma2d);
    UNUSED(pdata);
    UNUSED(DstAddress);
    UNUSED(Width);
    UNUSED(Height);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_DMA2D_PollForTransfer(DMA2D_HandleTypeDef *hdma2d, uint32_t Timeout)
{
    UNUSED(hdma2d);
    UNUSED(Timeout);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_DMA2D_ConfigLayer(DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{
    UNUSED(hdma2d);
    UNUSED(LayerIdx);

    return HAL_OK;
}