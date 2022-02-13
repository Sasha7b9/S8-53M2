#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


void HAL_TIM2::Init()
{
    TIM_HandleTypeDef tim2handle =
    {
        TIM2,
        {
            0,
            TIM_COUNTERMODE_UP,
            0xffffffff,
            TIM_CLOCKDIVISION_DIV1
        }
    };

    if (HAL_TIM_Base_Init(&tim2handle) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_TIM_Base_Start(&tim2handle) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}
