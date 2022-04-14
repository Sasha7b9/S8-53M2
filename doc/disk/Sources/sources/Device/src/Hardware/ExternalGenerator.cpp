#include "defines.h"
#include "Hardware/ExternalGenerator.h"
#include <stm32f4xx_hal.h>


TIM_HandleTypeDef htim8;
TIM_OC_InitTypeDef sConfigOC = {0};



void ExternalGenerator::Init()
{
    /*
    __HAL_RCC_TIM8_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    uint uhPrescalerValue = (uint32_t)(SystemCoreClock / (16000000 * 2)) - 1;
#define  PERIOD_VALUE       (uint32_t)(4)  // Period Value
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        // Capture Compare 1 Value

    LOG_WRITE("%d", HAL_RCC_GetSysClockFreq());

    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 0;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = 4;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
    {
        LOG_ERROR_TRACE("");
    }


    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        LOG_ERROR_TRACE("");
    }


    if (HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3) != HAL_OK)
    {
        LOG_ERROR_TRACE("");
    }
    */
}
