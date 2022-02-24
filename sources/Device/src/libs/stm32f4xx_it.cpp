// 2022/2/11 19:25:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "usbd_desc.h"
#include "defines.h"
#include "VCP/VCP.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Hardware/Sound.h"
#include "Utils/Debug.h"
#include <usbd_cdc_interface.h>


#ifdef __cplusplus
extern "C" {
#endif



void NMI_Handler()
{
}


void HardFault_Handler()
{
    volatile int line = Debug::line[0];
    volatile pchar file = Debug::file[0];

    while (1)
    {
        line = line;
        file = file;
    }
}


void MemManage_Handler()
{
  while (1)
  {
  }
}


void BusFault_Handler()
{
  while (1)
  {
  }
}


void UsageFault_Handler()
{
  while (1)
  {
  }
}


void SVC_Handler()
{
}


void DebugMon_Handler()
{
}


void PendSV_Handler()
{
}


void SysTick_Handler()
{
    HAL_IncTick();
}


void ADC_IRQHandler()
{
    HAL_ADC_IRQHandler((ADC_HandleTypeDef *)HAL_ADC3::handle);
}


// Панель
void SPI5_IRQHandler()
{
    HAL_SPI_IRQHandler((SPI_HandleTypeDef *)HAL_SPI5::handle);
}


void TIM6_DAC_IRQHandler()
{
    HAL_TIM6::CallbackOnIRQ();
}


#ifdef __cplusplus
}
#endif
