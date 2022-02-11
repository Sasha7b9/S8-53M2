#include "Hardware/Hardware.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_sram.h>


void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    (void)hspi;
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    (void)hspi;
}


void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    (void)hsram;
}

void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef *hsram)
{
    (void)hsram;

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_11 ||
                    GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
}



void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
    (void)hhcd;

    __GPIOB_CLK_ENABLE();
    __USB_OTG_HS_CLK_ENABLE();
    __SYSCFG_CLK_ENABLE();

    GPIO_InitTypeDef isGPIO;
    isGPIO.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    isGPIO.Mode = GPIO_MODE_AF_PP;
    isGPIO.Pull = GPIO_NOPULL;
    isGPIO.Speed = GPIO_SPEED_FAST;
    isGPIO.Alternate = GPIO_AF12_OTG_HS_FS;
    HAL_GPIO_Init(GPIOB, &isGPIO);

    /* Set USBHS Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 15, 0);

    /* Enable USBHS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
}



void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
    (void)hhcd;

    __USB_OTG_HS_CLK_DISABLE();
}
