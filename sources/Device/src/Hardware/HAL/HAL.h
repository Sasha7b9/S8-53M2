// 2022/2/11 20:59:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


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
    extern uint16 *const ADDR_RAM_DISPLAY_FRONT;    // Видимое изображение дисплея. Находится в RAM и не может исполь-
                                                    // зоваться для других целей
}


namespace HAL_LTDC
{
    void Init(uint8 *frontBuffer, uint8 *backBuffer);

    void LoadPalette();

    void ToggleBuffers();
};
