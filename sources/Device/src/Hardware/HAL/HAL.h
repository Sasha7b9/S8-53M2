// 2022/2/11 20:59:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


/*
                         |      Было      |                        Стало                                    |

    АЦП рандомизатора    |                | ADC1 ; ADC_CHANNEL_0 ; EXTI4_IRQHandler ; in - PA0 ; it - PH4   |
    Яркость дисплея      |                | DAC1 ; DAC1_CHANNEL_1 ; out - PA4                               |
    Звук                 |                | DAC2 ; DMA_CHANNEL_7 ; DAC_CHANNEL_2 ; DMA1_Stream6 ; out - PA5 |
    Таймер               |                |                                                                 |
    Панель               |                |                                                                 |
    OTP                  |                |                                                                 |
    RTC                  |                |                                                                 |
    ROM                  |                |                                                                 |
    PCD                  |                |                                                                 |
    LTDC                 |                |                                                                 |
    LAN                  |                |                                                                 |
    ШИНА                 |                |                                                                 |

                         |                |                                                                 |
*/

// Используется для управления яркостью дисплея
namespace HAL_DAC1
{
    void Init();

    // Значение value от 0 до 100
    void SetBrightness(int value);
};


// Используется для связи с FPGA и RAM
namespace HAL_FMC
{
    void Init();
    inline static uint16 Read(volatile const uint16 *const address) { return *address; };

    void Write(uint16 *const address, uint16 value);

    extern uint16 *const _ADDR_FPGA;                // Адрес записи в альтеру

    extern uint16 *const _ADDR_RAM_DISPLAY_FRONT;   // Видимое изображение дисплея. Находится в RAM и не может исполь-
                                                    // зоваться для других целей
    extern uint8 *const _ADDR_RAM_BEGIN;            // Первый свободный адрес в RAM.
    extern uint8 *const _ADDR_RAM_END;              // Указывает на первый недопустимый адрес RAM. Использовать можно
                                                    // адреса [ADDR_RAM_BEGIN...(ADDR_RAM_END - 1)]
};


namespace HAL_LTDC
{
    void Init(uint8 *frontBuffer, uint8 *backBuffer);

    void LoadPalette();

    void ToggleBuffers();
};
