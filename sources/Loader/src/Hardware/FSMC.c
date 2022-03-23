// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "FSMC.h"
#include "Hardware.h"
#include "Hardware/Timer.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_ll_fsmc.h>
#include <stm32f4xx_hal_sram.h>


void FSMC_Init();
uint8 FSMC_Read(uint8 *address);
void FSMC_Write(uint8 *address, uint8 value);



uint8 FSMC_Read(uint8 *address)
{
    return(*(address));
}


void FSMC_Write(uint8 *address, uint8 value)
{
    *address = value;
}
