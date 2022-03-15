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
#include "Data/Data.h"


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
        Data::dir.ds = &ds_null;
        return;
    }

    uint8 *dataA = nullptr;
    uint8 *dataB = nullptr;
    DataSettings **ds = nullptr;

    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    if (MODE_WORK_IS_DIRECT)
    {
        Storage::GetData(0, Data::dir);
        dataA = Data::dir.A.Data();
        dataB = Data::dir.B.Data();

        if (SettingsDisplay::NumAverages() != 1 || TBase::InModeRandomizer())
        {
            Storage::GetAverageData(Chan::A, Data::dir.A);
            Storage::GetAverageData(Chan::B, Data::dir.B);
        }
    }
    else if (MODE_WORK_IS_LATEST)
    {
        Storage::GetData(PageMemory::Latest::current, Data::last);
        dataA = Data::last.A.Data();
        dataB = Data::last.B.Data();
        ds = &Data::last.ds;
    }
    else if (MODE_WORK_IS_MEMINT)
    {
        HAL_ROM::GetData(PageMemory::Internal::currentSignal, Data::ins);
        dataA = Data::ins.A.Data();
        dataB = Data::ins.B.Data();
        ds = &Data::ins.ds;
    }

    if (MODE_WORK_IS_MEMINT)
    {
        if (!MODE_SHOW_MEMINT_IS_SAVED)
        {
            Processing::SetSignal(Data::dir.A.Data(), Data::dir.B.Data(), Data::dir.ds, points);
        }
    }
    else
    {
        Processing::SetSignal(dataA, dataB, *ds, points);
    }

    if (Data::dir.ds == nullptr)
    {
        static DataSettings ds_null;
        ds_null.Init();
        Data::dir.ds = &ds_null;
    }

    PageCursors::Cursors_Update();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.
}
