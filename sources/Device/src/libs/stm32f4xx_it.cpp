// 2022/2/11 19:25:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "usbd_desc.h"
#include "defines.h"
#include "VCP/VCP.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Hardware/Sound.h"
#include <usbd_cdc_interface.h>


#ifdef __cplusplus
extern "C" {
#endif



void NMI_Handler()
{
}


void HardFault_Handler()
{
    while (1)
    {
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


// This interrupt call soft NSS for spi (see Hardware::SPIforPanel.c::PanelInit() and HAL_GPIO_EXTI_Callback().
void EXTI0_IRQHandler()
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}


// ������
void SPI5_IRQHandler()
{
    HAL_SPI_IRQHandler((SPI_HandleTypeDef *)HAL_SPI5::handle);
}


void OTG_HS_IRQHandler()
{
    HAL_HCD_IRQHandler((HCD_HandleTypeDef *)HAL_HCD::handle);
}


void DMA1_Stream5_IRQHandler()
{
    HAL_DMA_IRQHandler(((DAC_HandleTypeDef *)Sound::handle)->DMA_Handle1);
}


void OTG_FS_IRQHandler()
{
    HAL_PCD_IRQHandler((PCD_HandleTypeDef *)VCP::handlePCD);
}


void TIM6_DAC_IRQHandler()
{
    HAL_TIM6::CallbackOnIRQ();
}

#ifdef __cplusplus
}
#endif
