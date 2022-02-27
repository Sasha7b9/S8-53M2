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


namespace Device
{
    void ProcessingSignal();
}


void Device::Init()
{
    HAL::Init();

    Sound::Init();

    Settings::Load(false);

    FPGA::Init();

    Timer::PauseOnTime(250);

    FPGA::OnPressStartStop();

    Display::Init();

    Panel::Init();

    // FDrive::Init();
    // LAN::Init();
    // VCP::Init();

    set.menu.isShown = true;
}


void Device::Update()
{
    if (NEED_RESET_SETTINGS)
    {
        Settings::Load(true);
        NEED_RESET_SETTINGS = 0;
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
    if (Storage::NumElementsInStorage() == 0)
    {
        static DataSettings ds_null;
        ds_null.Init();
        Storage::DS = &ds_null;
        return;
    }

    uint8 *dataA = Storage::dataA;
    uint8 *dataB = Storage::dataB;
    DataSettings **ds = &Storage::DS;

    BitSet32 points = SettingsDisplay::PointsOnDisplay();

    if (MODE_WORK_IS_DIRECT)
    {
        Storage::GetData(0, &Storage::DS, &Storage::dataA, &Storage::dataB);
        dataA = Storage::dataA;
        dataB = Storage::dataB;

        if (SettingsDisplay::NumAverages() != 1 || TBase::InRandomizeMode())
        {
            Storage::dataA = Storage::GetAverageData(Chan::A);
            Storage::dataB = Storage::GetAverageData(Chan::B);
        }
    }
    else if (MODE_WORK_IS_LATEST)
    {
        dataA = Storage::dataLastA;
        dataB = Storage::dataLastB;
        ds = &Storage::dsLast;
        Storage::GetData(CURRENT_NUM_LATEST_SIGNAL, &Storage::dsLast, &Storage::dataLastA, &Storage::dataLastB);
    }
    else if (MODE_WORK_IS_MEMINT)
    {
        dataA = Storage::dataIntA;
        dataB = Storage::dataIntB;
        ds = &Storage::dsInt;
        HAL_ROM::GetData(CURRENT_NUM_INT_SIGNAL, &Storage::dsInt, &Storage::dataIntA, &Storage::dataIntB);
    }

    if (MODE_WORK_IS_MEMINT)
    {
        if (!MODE_SHOW_MEMINT_IS_SAVED)
        {
            Processing::SetSignal(Storage::dataA, Storage::dataB, Storage::DS, points);
        }
    }
    else
    {
        Processing::SetSignal(dataA, dataB, *ds, points);
    }

    if (Storage::DS == nullptr)
    {
        static DataSettings ds_null;
        ds_null.Init();
        Storage::DS = &ds_null;
    }

    PageCursors::Cursors_Update();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.
}
