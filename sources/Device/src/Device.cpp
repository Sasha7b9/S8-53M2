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
    VCP_DEBUG_POINT();

    if (Settings::needReset)
    {
        Settings::Reset();
        Settings::needReset = false;
    }

    VCP_DEBUG_POINT();

    Timer::StartMultiMeasurement();      // ����� ������� ��� ������ ������������ ��������� ���������� � ������� ����� �������� �����.

    VCP_DEBUG_POINT();

    FDrive::Update();

    VCP_DEBUG_POINT();

    FPGA::Update();                      // ��������� ���������� �����.

    VCP_DEBUG_POINT();

    ProcessingSignal();

    VCP_DEBUG_POINT();

    Panel::Update();

    VCP_DEBUG_POINT();

    Menu::UpdateInput();                 // ��������� ��������� ����

    VCP_DEBUG_POINT();

    Display::Update();                   // ������ �����.

    VCP_DEBUG_POINT();

    Settings::SaveIfNeed();

    VCP_DEBUG_POINT();

    // LAN::Update(0);
}


void Device::ProcessingSignal()
{
    if (Storage::NumElements() == 0)
    {
        return;
    }

    DataStruct data;

    if (MODE_WORK_IS_DIRECT)
    {
        Storage::GetData(0, Data::dir);

        data = Data::dir;

        if (ModeAveraging::GetNumber() != 1 || TBase::InModeRandomizer())
        {
            Averager::GetData(Data::dir);
        }

        Processing::Process(Data::dir);
    }
    else if (MODE_WORK_IS_LATEST)
    {
        Storage::GetData(PageMemory::Latest::current, Data::last);

        data = Data::last;
    }
    else if (MODE_WORK_IS_MEMINT)
    {
        HAL_ROM::GetData(PageMemory::Internal::currentSignal, Data::ins);

        data = Data::ins;
    }

    PageCursors::Cursors_Update();    // � ������, ���� ��������� � ������ ��������� ���������, ��������� �� ���������, ���� �����.
}
