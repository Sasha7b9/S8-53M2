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
#include "FDrive/FDrive.h"
#include "Menu/Menu.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "FDrive/FDrive.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Strings.h"
#include "Data/Data.h"
#include "Data/DataExtensions.h"


namespace Device
{
    void ProcessingSignal();
}


void Device::Init()
{
    HAL::Init();

    Sound::Init();

    Settings::Load();

    FPGA::Init();

    Timer::PauseOnTime(250);

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

    DEBUG_POINT_0;

    Timer::StartMultiMeasurement();      // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.

    DEBUG_POINT_0;

    FDrive::Update();

    DEBUG_POINT_0;

    FPGA::Update();                      // Обновляем аппаратную часть.

    DEBUG_POINT_0;

    ProcessingSignal();

    DEBUG_POINT_0;

    Panel::Update();

    DEBUG_POINT_0;

    Menu::UpdateInput();                 // Обновляем состояние меню

    DEBUG_POINT_0;

    Display::Update();                   // Рисуем экран.

    DEBUG_POINT_0;

    Settings::SaveIfNeed();

    // LAN::Update(0);
}


void Device::ProcessingSignal()
{
    if (Storage::NumFrames() == 0)
    {
        return;
    }

    if (MODE_WORK_IS_DIRECT)
    {
        Storage::GetData(0, Data::dir);

        if (ModeAveraging::GetNumber() != 1 || TBase::InModeRandomizer())
        {
            Averager::GetData(Data::dir);
        }

        Processing::Process(Data::dir);
    }
    else if (MODE_WORK_IS_LATEST)
    {
        Storage::GetData(PageMemory::Latest::current, Data::last);
    }
    else if (MODE_WORK_IS_MEMINT)
    {
        HAL_ROM::GetData(PageMemory::Internal::currentSignal, Data::ins);
    }

    PageCursors::Cursors_Update();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.
}
