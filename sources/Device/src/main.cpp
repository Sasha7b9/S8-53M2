// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
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
#include <stm32f4xx_hal.h>


int main()
{
    HAL::Init();

    Sound::Init();

    nrst.Load();

    gset.Load();

    FPGA::Init();

    Display::Init();

    Panel::Init();

    VCP::Init();

    LAN::Init();

    FDrive::Init();

    FPGA::Start();

    while(1)
    {
        Timer::StartMultiMeasurement();      // ����� ������� ��� ������ ������������ ��������� ���������� � ������� ����� �������� �����.

        FPGA::meterStart.Reset();

        FDrive::Update();

        FPGA::Update();                      // ��������� ���������� �����.

        Panel::Update();

        Menu::UpdateInput();                 // ��������� ��������� ����

        Display::Update();                   // ������ �����.

        LAN::Update();

        SCPI::Update();

        gset.Log();
    }
}
