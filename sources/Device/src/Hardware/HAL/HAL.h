// 2022/2/11 20:59:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/HAL/HAL_ROM.h"
#include "FPGA/SettingsFPGA.h"


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


void ERROR_HANDLER();


namespace HAL
{
    void Init();
    uint CalculateCRC32();
}


// ��� �������������
namespace HAL_ADC1
{
    static const uint16 INVALID_VALUE = 0xFFFF;

    void Init();

    // ���� 0xFFFF - �������� ������������
    uint16 GetValue();

    // ���������� ����� ������� �������
    void ResetValue();

    bool CallbackOnIRQ();
};


// ������������ ��� ���������� �������� �������
namespace HAL_DAC1
{
    void Init();

    // �������� value �� 0 �� 100
    void SetBrightness(int value);
};


// �������
namespace HAL_DAC2
{
    void Init();
    void StartDMA(uint8 *points, int numPoints);
    void StopDMA();
};


// ������������ ��� ����� � FPGA � RAM
namespace HAL_FMC
{
    void Init();

    inline uint16 Read(volatile const uint16 *const address) { return *address; };

    void Write(uint16 *const address, uint16 value);

    extern uint16 *const ADDR_FPGA;                 // ����� ������ � �������

    extern uint8 *const ADDR_RAM_DISPLAY_FRONT;     // ������� ����������� �������. ��������� � RAM � �� ����� ������-
                                                    // �������� ��� ������ �����
    extern uint8 *const ADDR_RAM_BEGIN;             // ������ ��������� ����� � RAM.
    extern uint8 *const ADDR_RAM_END;               // ��������� �� ������ ������������ ����� RAM. ������������ �����
                                                    // ������ [ADDR_RAM_BEGIN...(ADDR_RAM_END - 1)]
};


namespace HAL_LTDC
{
    void Init(uint8 *front_buffer, uint8 *back_buffer);

    void LoadPalette();

    void ToggleBuffers();
};


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


// ��������������� ������ ��� DAC2
namespace HAL_TIM7
{
    void Init();

    void Config(uint16 prescaler, uint16 period);
};


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
