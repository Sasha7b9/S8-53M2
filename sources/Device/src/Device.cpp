// 2022/2/12 12:47:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "FPGA/FPGA.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/Menu.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "Hardware/LAN/LAN.h"
#include "SCPI/SCPI.h"


void Device::Init() 
{
    HAL::Init();

    Sound::Init();

    SettingsNRST::Load();

    Settings::Load();

    FPGA::Init();

    Display::Init();

    Panel::Init();

    VCP::Init();

    LAN::Init();

    FDrive::Init();

    FPGA::Start();
}


void Device::Update()
{
    Timer::StartMultiMeasurement();      // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.

    FPGA::meterStart.Reset();

    FDrive::Update();

    FPGA::Update();                      // Обновляем аппаратную часть.

    Panel::Update();

    Menu::UpdateInput();                 // Обновляем состояние меню

    Display::Update();                   // Рисуем экран.

    SCPI::Update();

    LAN::Update();

    if (Settings::needReset)
    {
        Settings::Reset();
        Settings::needReset = false;
    }
}
