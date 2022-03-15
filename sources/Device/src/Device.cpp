// 2022/2/12 12:47:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Data/Storage.h"
#include "FPGA/SettingsFPGA.h"
#include "Data/Processing.h"
#include "VCP/VCP.h"
#include "Hardware/Timer.h"
#include "FDrive/FDrive.h"
#include "Menu/Menu.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "FDrive/FDrive.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Strings.h"
#include <stm32f4xx_hal.h>


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
    // VCP::Init();
}


void Device::Update()
{
    if (Settings::needReset)
    {
        Settings::Reset();
        Settings::needReset = false;
    }

    Timer::StartMultiMeasurement();      // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.

    FDrive::Update();

    FPGA::Update();                      // Обновляем аппаратную часть.

    ProcessingSignal();

    Panel::Update();

    Menu::UpdateInput();                 // Обновляем состояние меню

    Display::Update();                   // Рисуем экран.

    Settings::SaveIfNeed();

    // LAN::Update(0);

    // VCP::Update();
}


void Device::ProcessingSignal()
{
    if (Storage::NumElements() == 0)
    {
        static DataSettings ds_null;
        ds_null.Init();
        Storage::data.ds = &ds_null;
        return;
    }

    uint8 *dataA = Storage::data.A;
    uint8 *dataB = Storage::data.B;
    DataSettings **ds = &Storage::data.ds;

    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    if (MODE_WORK_IS_DIRECT)
    {
        Storage::GetData(0, &Storage::data.ds, &Storage::data.A, &Storage::data.B);
        dataA = Storage::data.A;
        dataB = Storage::data.B;

        if (SettingsDisplay::NumAverages() != 1 || TBase::InModeRandomizer())
        {
            Storage::data.A = Storage::GetAverageData(Chan::A);
            Storage::data.B = Storage::GetAverageData(Chan::B);
        }
    }
    else if (MODE_WORK_IS_LATEST)
    {
        dataA = Storage::last.A;
        dataB = Storage::last.B;
        ds = &Storage::last.ds;
        Storage::GetData(PageMemory::Latest::current, &Storage::last.ds, &Storage::last.A, &Storage::last.B);
    }
    else if (MODE_WORK_IS_MEMINT)
    {
        dataA = Storage::ins.A;
        dataB = Storage::ins.B;
        ds = &Storage::ins.ds;
        HAL_ROM::GetData(PageMemory::Internal::currentSignal, &Storage::ins.ds, &Storage::ins.A, &Storage::ins.B);
    }

    if (MODE_WORK_IS_MEMINT)
    {
        if (!MODE_SHOW_MEMINT_IS_SAVED)
        {
            Processing::SetSignal(Storage::data.A, Storage::data.B, Storage::data.ds, points);
        }
    }
    else
    {
        Processing::SetSignal(dataA, dataB, *ds, points);
    }

    if (Storage::data.ds == nullptr)
    {
        static DataSettings ds_null;
        ds_null.Init();
        Storage::data.ds = &ds_null;
    }

    PageCursors::Cursors_Update();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.
}
