#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"


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


#ifdef __cplusplus
extern "C" {
#endif


    void TIM6_DAC_IRQHandler()
    {
        if (__HAL_TIM_GET_FLAG(&HAL_TIM6::handleTIM6forTimer, TIM_FLAG_UPDATE) == SET && __HAL_TIM_GET_ITSTATUS(&HAL_TIM6::handleTIM6forTimer, TIM_IT_UPDATE))
        {
            Timer::Update1ms();
            __HAL_TIM_CLEAR_FLAG(&HAL_TIM6::handleTIM6forTimer, TIM_FLAG_UPDATE);
            __HAL_TIM_CLEAR_IT(&HAL_TIM6::handleTIM6forTimer, TIM_IT_UPDATE);
        }
    }

#ifdef __cplusplus
}
#endif
