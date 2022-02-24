// 2022/2/12 12:47:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "FPGA/Storage.h"
#include "FPGA/SettingsFPGA.h"
#include "Utils/ProcessingSignal.h"
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

//    VCP::Init();
    Settings::Load(false);

    FPGA::Init();

    Timer::PauseOnTime(250);

    FPGA::OnPressStartStop();

    Display::Init();

    Panel::Init();

    // FDrive::Init();
    // HAL_RTC::Init();

    // LAN::Init();

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
        return;
    }

    uint8* data0 = Storage::dataA;
    uint8* data1 = Storage::dataB;
    DataSettings** ds = &Storage::DS;

    int first = 0;
    int last = 0;
    SettingsDisplay::PointsOnDisplay(&first, &last);

    if (MODE_WORK_IS_DIRECT)
    {
        Storage::GetDataFromEnd(0, &Storage::DS, &Storage::dataA, &Storage::dataB);

        if (SettingsDisplay::NumAverages() != 1 || TBase::InRandomizeMode())
        {
            Storage::dataA = Storage::GetAverageData(Chan::A);
            Storage::dataB = Storage::GetAverageData(Chan::B);
        }
    }
    else if (MODE_WORK_IS_LATEST)
    {
        data0 = Storage::dataLastA;
        data1 = Storage::dataLastB;
        ds = &Storage::dsLast;
        Storage::GetDataFromEnd(CURRENT_NUM_LATEST_SIGNAL, &Storage::dsLast, &Storage::dataLastA, &Storage::dataLastB);
    }
    else if (MODE_WORK_IS_MEMINT)
    {
        data0 = Storage::dataIntA;
        data1 = Storage::dataIntB;
        ds = &Storage::dsInt;
        HAL_ROM::GetData(CURRENT_NUM_INT_SIGNAL, &Storage::dsInt, &Storage::dataIntA, &Storage::dataIntB);
    }

    if (MODE_WORK_IS_MEMINT)
    {
        if (!MODE_SHOW_MEMINT_IS_SAVED)
        {
            Processing::SetSignal(Storage::dataA, Storage::dataB, Storage::DS, first, last);
        }
    }
    else
    {
        Processing::SetSignal(data0, data1, *ds, first, last);
    }

    if (Storage::DS == nullptr)
    {
        static DataSettings ds_null;
        ds_null.FillDataPointer();
        Storage::DS = &ds_null;
    }

    PageCursors::Cursors_Update();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.
}
