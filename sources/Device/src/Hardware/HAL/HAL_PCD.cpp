// 2022/03/17 15:24:50 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


static PCD_HandleTypeDef handlePCD;
void *HAL_PCD::handle = &handlePCD;
