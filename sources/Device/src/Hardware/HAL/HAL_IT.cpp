// 2022/2/11 19:25:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/usbd_desc.h"
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Hardware/Sound.h"
#include "Utils/Debug.h"
#include "Hardware/VCP/usbd_cdc_interface.h"


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

        while (1) //-V776
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

    void OTG_HS_IRQHandler(void)
    {
        HAL_HCD_IRQHandler((HCD_HandleTypeDef *)HAL_HCD::handle);
    }



    void OTG_FS_IRQHandler()
    {
        HAL_PCD_IRQHandler((PCD_HandleTypeDef *)HAL_PCD::handle);
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


    // Прерывание на АЦП рандомизатора
    void EXTI4_IRQHandler()
    {
        HAL_ADC1::CallbackOnIRQ();
    }


#ifdef __cplusplus
}
#endif
