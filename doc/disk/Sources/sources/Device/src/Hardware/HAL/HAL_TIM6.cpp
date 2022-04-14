#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <stm32f4xx_hal.h>


namespace HAL_TIM6
{
    static TIM_HandleTypeDef handle =
    {
        TIM6,
        {
            119,                    // Init.Prescaler
            TIM_COUNTERMODE_UP,     // Init.CounterMode
            500,                    // Init.Period
            TIM_CLOCKDIVISION_DIV1  // Init.ClockDivision
        }
    };
}


void HAL_TIM6::Init()
{
    // Таймер для мс
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    if (HAL_TIM_Base_Init(&handle) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_TIM_Base_Start_IT(&handle) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}


bool HAL_TIM6::CallbackOnIRQ()
{
    if (__HAL_TIM_GET_FLAG(&handle, TIM_FLAG_UPDATE) == SET &&
        __HAL_TIM_GET_ITSTATUS(&handle, TIM_IT_UPDATE))
    {
        Timer::Update1ms();
        __HAL_TIM_CLEAR_FLAG(&handle, TIM_FLAG_UPDATE);
        __HAL_TIM_CLEAR_IT(&handle, TIM_IT_UPDATE);

        return true;
    }

    return false;
}
