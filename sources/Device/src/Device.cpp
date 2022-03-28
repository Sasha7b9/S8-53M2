// 2022/2/12 12:47:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Data/Storage.h"
#include "FPGA/SettingsFPGA.h"
#include "Data/Processing.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/Timer.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/Menu.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Strings.h"
#include "Data/Data.h"
#include "Data/DataExtensions.h"


void Device::Init()
{
    HAL::Init();

    Sound::Init();

    Settings::Load();

    FPGA::Init();

    FPGA::OnPressStartStop();

    Display::Init();

    Panel::Init();

    // FDrive::Init();
    // LAN::Init();
    VCP::Init();
}


void Device::Update()
{
    if (Settings::needReset)
    {
        Settings::Reset();
        Settings::needReset = false;
    }

    Timer::StartMultiMeasurement();      // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.

    DEBUG_POINT_0;

    FDrive::Update();

    DEBUG_POINT_0;

    FPGA::Update();                      // Обновляем аппаратную часть.

    DEBUG_POINT_0;

    Panel::Update();

    DEBUG_POINT_0;

    Menu::UpdateInput();                 // Обновляем состояние меню

    DEBUG_POINT_0;

    Display::Update();                   // Рисуем экран.

    DEBUG_POINT_0;

    Settings::SaveIfNeed();

//    float voltage = 0.0f;
//
//    LOG_WRITE("%f V = %d", voltage, ValueFPGA::FromVoltage(voltage, Range::_1V, RShift::ZERO + 40));

    DEBUG_POINT_0;

    // LAN::Update(0);
}
