// 2022/2/12 12:47:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "FPGA/Storage.h"
#include "Utils/ProcessingSignal.h"
#include "Menu/Pages/PageCursors.h"
#include "VCP/VCP.h"
#include "Hardware/Timer.h"
#include "FlashDrive/FlashDrive.h"
#include "Menu/Menu.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "FlashDrive/FlashDrive.h"


namespace Device
{
    void ProcessingSignal();
}


void Device::Init()
{
    HAL::Init();
    Settings::Load(true);
//    Sound::Init();
//    VCP::Init();
//    Settings::Load(false);
//    FPGA::Init();
//    Timer::PauseOnTime(250);
//    FPGA::OnPressStartStop();
    Display::Init();
    Panel::Init();
//    FlashDrive::Init();
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
    FlashDrive::Update();
    FPGA::Update();                      // Обновляем аппаратную часть.
    ProcessingSignal();
    Panel::Update();
    Menu::UpdateInput();                 // Обновляем состояние меню
    Display::Update();                   // Рисуем экран.

    // LAN::Update(0);
    // VCP::Update();
}


void Device::ProcessingSignal()
{
    uint8** data0 = &gData0;
    uint8** data1 = &gData1;
    DataSettings** ds = &gDSet;

    int first = 0;
    int last = 0;
    sDisplay_PointsOnDisplay(&first, &last);

    if (MODE_WORK_IS_DIRECT)
    {
        Storage::GetDataFromEnd(0, &gDSet, &gData0, &gData1);
        if (sDisplay_NumAverage() != 1 || sTime_RandomizeModeEnabled())
        {
            gData0 = Storage::GetAverageData(Chan::A);
            gData1 = Storage::GetAverageData(Chan::B);
        }
    }
    else if (MODE_WORK_IS_LATEST)
    {
        data0 = &gData0memLast;
        data1 = &gData1memLast;
        ds = &gDSmemLast;
        Storage::GetDataFromEnd(CURRENT_NUM_LATEST_SIGNAL, &gDSmemLast, &gData0memLast, &gData1memLast);
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
            Processing::SetSignal(gData0, gData1, gDSet, first, last);
        }
    }
    else
    {
        Processing::SetSignal(*data0, *data1, *ds, first, last);
    }

    Cursors_Update();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.

}
