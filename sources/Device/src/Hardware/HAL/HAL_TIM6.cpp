#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <stm32f4xx_hal.h>


namespace HAL_TIM6
{
    static TIM_HandleTypeDef handleTIM6forTimer =
    {
        TIM6,
        {
            119,                    // Init.Prescaler
            TIM_COUNTERMODE_UP,     // Init.CounterMode
            500,                    // Init.Period
            TIM_CLOCKDIVISION_DIV1  // Init.ClockDivision
        }
    };

    void *handle = &handleTIM6forTimer;
}


void HAL_TIM6::Init()
{
    // Таймер для мс
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    if (HAL_TIM_Base_Init(&handleTIM6forTimer) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_TIM_Base_Start_IT(&handleTIM6forTimer) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}
