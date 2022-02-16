// 2022/2/11 20:59:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/HAL/HAL_ROM.h"


/*
                         |      ����                  |                        �����                                    |

    ��� �������������    |                            | ADC1 ; ADC_CHANNEL_0 ; EXTI4_IRQHandler ; in - PA0 ; it - PH4   |
    ������� �������      |                            | DAC1 ; DAC1_CHANNEL_1 ; out - PA4                               |
    ����                 | DAC1 ; DMA1                | DAC2 ; DMA_CHANNEL_7 ; DAC_CHANNEL_2 ; DMA1_Stream6 ; out - PA5 |
    ������               | TIM6 ; TIM6_DAC_IRQHandler |                                                                 |
    ������               |                            |                                                                 |
    USB                  |                            |                                                                 |
    Flash                |                            |                                                                 |
    ������������         | TIM2                       |                                                                 |

    OTP                  |                            |                                                                 |
    RTC                  |                            |                                                                 |
    ROM                  |                            |                                                                 |
    LTDC                 |                            |                                                                 |
    LAN                  |                            |                                                                 |
    ����                 |                            |                                                                 |

                         |                            |                                                                 |
*/


namespace HAL
{
    void Init();
    uint CalculateCRC32();
}


namespace HAL_ADC3
{
    void Init();
    uint16 GetValue();
    extern void *handle;        // ADC_HandleTypeDef
}


// ������������ ��� ���������� �������� �������
namespace HAL_DAC1
{
    void Init();

    // �������� value �� 0 �� 100
    void SetBrightness(int value);
};


#define ADDR_BANK  0x60000000
#define ADDR_FPGA  ((uint8*)(ADDR_BANK + 0x00c80000))  // ����� ������ � ���������� ��������.

// ������������ ��� ����� � FPGA � RAM
namespace HAL_FMC
{
    // ��� ������ ������� ������/������
    uint8 Read(uint8* address);
    void  Write(uint8* address, uint8 value);

    void Init();

    inline static uint16 Read(volatile const uint16 *const address) { return *address; };

    void Write(uint16 *const address, uint16 value);

    extern uint16 *const _ADDR_FPGA;                // ����� ������ � �������

    extern uint8 *const _ADDR_RAM_DISPLAY_FRONT;   // ������� ����������� �������. ��������� � RAM � �� ����� ������-
                                                    // �������� ��� ������ �����
    extern uint8 *const _ADDR_RAM_BEGIN;            // ������ ��������� ����� � RAM.
    extern uint8 *const _ADDR_RAM_END;              // ��������� �� ������ ������������ ����� RAM. ������������ �����
                                                    // ������ [ADDR_RAM_BEGIN...(ADDR_RAM_END - 1)]
};


namespace HAL_LTDC
{
    void Init(uint8 *front_buffer, uint8 *back_buffer);

    void LoadPalette();

    void ToggleBuffers();
};


namespace HAL_SPI1
{
    extern void *handle;       // SPI_HandleTypeDef
}


// ��� ����� � �������
namespace HAL_SPI5
{
    void Init();

    extern void *handle;       // SPI_HandleTypeDef
};


// ������ ��� �����
namespace HAL_TIM2
{
    void Init();
}


// ������ �����������
namespace HAL_TIM6
{
    void Init();

    bool CallbackOnIRQ();
}


namespace HAL_RTC
{
    void Init();
    bool SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);
    PackedTime GetPackedTime();
}


namespace HAL_HCD
{
    void Init();
    extern void *handle;        // HCD_HandleTypeDef
}


namespace HAL_USBH
{
    extern void *handle;
}
