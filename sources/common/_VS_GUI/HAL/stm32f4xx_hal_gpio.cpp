// Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include <stm32f4xx_hal.h>


void HAL_GPIO_Init(GPIO_TypeDef *, GPIO_InitTypeDef *)
{

}


GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *, uint16_t)
{
    return GPIO_PIN_RESET;
}


void HAL_GPIO_WritePin(GPIO_TypeDef *, uint16_t, GPIO_PinState)
{

}
