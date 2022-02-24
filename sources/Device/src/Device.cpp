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
    DEBUG_POINT_0;
    HAL::Init();
    DEBUG_POINT_0;
    Sound::Init();
    DEBUG_POINT_0;
//    VCP::Init();
    Settings::Load(false);
    DEBUG_POINT_0;
    FPGA::Init();
    DEBUG_POINT_0;
    Timer::PauseOnTime(250);
    DEBUG_POINT_0;
    FPGA::OnPressStartStop();
    DEBUG_POINT_0;
    Display::Init();
    DEBUG_POINT_0;
    Panel::Init();
    DEBUG_POINT_0;
//    FDrive::Init();
//    HAL_RTC::Init();

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

    DEBUG_POINT_0;

    // LAN::Update(0);

    // VCP::Update();
}


void Device::ProcessingSignal()
{
    if (Storage::NumElementsInStorage() == 0)
    {
        return;
    }

    uint8** data0 = &Storage::dataA;
    uint8** data1 = &Storage::dataB;
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
        data0 = &Storage::gData0memLast;
        data1 = &gData1memLast;
        ds = &gDSmemLast;
        Storage::GetDataFromEnd(CURRENT_NUM_LATEST_SIGNAL, &gDSmemLast, &Storage::gData0memLast, &gData1memLast);
    }
    else if (MODE_WORK_IS_MEMINT)
    {
        data0 = &gData0memInt;
        data1 = &gData1memInt;
        ds = &gDSmemInt;
        HAL_ROM::GetData(CURRENT_NUM_INT_SIGNAL, &gDSmemInt, &gData0memInt, &gData1memInt);
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
        Processing::SetSignal(*data0, *data1, *ds, first, last);
    }

    if (Storage::DS == nullptr)
    {
        static DataSettings ds_null;
        ds_null.FillDataPointer();
        Storage::DS = &ds_null;
    }

    PageCursors::Cursors_Update();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.
}
