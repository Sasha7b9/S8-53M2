// 2022/2/12 12:47:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Data/Storage.h"
#include "FPGA/FPGA.h"
#include "FPGA/SettingsFPGA.h"
#include "Data/Processing.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/Timer.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/Menu.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "Menu/Pages/Definition.h"
#include "Data/Data.h"
#include "Data/DataExtensions.h"
#include "Hardware/LAN/LAN.h"


void Device::Init()
{
    HAL::Init();

    Sound::Init();

    Settings::Reset();

    FPGA::Init();

    Display::Init();

    Panel::Init();

    VCP::Init();

    LAN::Init();

    FDrive::Init();

    FPGA::OnPressStartStop();
}


void Device::Update()
{
    if (Settings::needReset)
    {
        Settings::Reset();
        Settings::needReset = false;
    }

    Timer::StartMultiMeasurement();      // ����� ������� ��� ������ ������������ ��������� ���������� � ������� ����� �������� �����.

    FPGA::meterStart.Reset();

    FDrive::Update();

    FPGA::Update();                      // ��������� ���������� �����.

    Panel::Update();

    Menu::UpdateInput();                 // ��������� ��������� ����

    Display::Update();                   // ������ �����.

    Settings::SaveIfNeed();

    LAN::Update();
}
