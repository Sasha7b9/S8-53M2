// 2022/2/12 10:07:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


namespace HAL_LTDC
{
//    static void SetBuffers(uint8 *frontBuffer, uint8 *backBuffer);

    static LTDC_HandleTypeDef handleLTDC;
    static uint frontBuffer = 0;
    static uint backBuffer = 0;
}


void HAL_LTDC::ToggleBuffers()
{
    DMA2D_HandleTypeDef hDMA2D;

    hDMA2D.Init.Mode = DMA2D_M2M;
    hDMA2D.Init.ColorMode = LTDC_PIXEL_FORMAT_L8;
    hDMA2D.Init.OutputOffset = 0;

    hDMA2D.XferCpltCallback = nullptr;

    hDMA2D.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hDMA2D.LayerCfg[1].InputAlpha = 0xFF;
    hDMA2D.LayerCfg[1].InputColorMode = LTDC_PIXEL_FORMAT_L8;
    hDMA2D.LayerCfg[1].InputOffset = 0;

    hDMA2D.Instance = DMA2D;

    HAL_DMA2D_Init(&hDMA2D);

    HAL_DMA2D_ConfigLayer(&hDMA2D, 1);

    HAL_DMA2D_Start(&hDMA2D, backBuffer, frontBuffer, 320, 240);

    HAL_DMA2D_PollForTransfer(&hDMA2D, 100);
}
