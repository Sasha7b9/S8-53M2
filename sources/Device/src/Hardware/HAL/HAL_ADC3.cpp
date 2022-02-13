#include "defines.h"
#include "Hardware/HAL/HAL.h"


namespace HAL_ADC3
{
    static ADC_HandleTypeDef handleADC;

    static uint16 adcValue = 0;

    void *handle = &handleADC;
}


void HAL_ADC3::Init()
{
    /*
    ��� ��� �������������
    ���� - ADC3 - 18 ADC3_IN4 - PF6
    �������� ������� 25���
    ����� ������ :
    -��������� �� 1 ����������� ������
    - ��������� ��������� �� ������ �������� �������(���������� �� 112 - EXT11 - PC11)
    */

    __ADC3_CLK_ENABLE();

    static GPIO_InitTypeDef isGPIO =
    {
        GPIO_PIN_6,
        GPIO_MODE_ANALOG,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOF, &isGPIO);

    GPIO_InitTypeDef isGPIOC =
    {
        GPIO_PIN_11,
        GPIO_MODE_IT_RISING,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOC, &isGPIOC);

    HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

    handleADC.Instance = ADC3;
    handleADC.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    handleADC.Init.Resolution = ADC_RESOLUTION12b;
    handleADC.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    handleADC.Init.ScanConvMode = DISABLE;
    handleADC.Init.EOCSelection = ENABLE;
    handleADC.Init.ContinuousConvMode = DISABLE;
    handleADC.Init.DMAContinuousRequests = DISABLE;
    handleADC.Init.NbrOfConversion = 1;
    handleADC.Init.DiscontinuousConvMode = DISABLE;
    handleADC.Init.NbrOfDiscConversion = 0;
    handleADC.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    handleADC.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_Ext_IT11;

    if (HAL_ADC_Init(&handleADC) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(&handleADC, &sConfig) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_ADC_Start_IT(&handleADC) != HAL_OK)
    {
        ERROR_HANDLER();
    }
}


uint16 HAL_ADC3::GetValue()
{
    return adcValue;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    HAL_ADC3::adcValue = (uint16)HAL_ADC_GetValue(hadc);
}
