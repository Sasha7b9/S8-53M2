// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "common/Display/Painter/Text_.h"
#include "common/Hardware/Memory/Sector_.h"
#include "device.h"
#include "GUI/Application.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"



void Application::Init()
{
    Sector::Get(Sector::_12_NRST_1).Erase();

    Device::Init();

    Menu::Show();
}



void Application::Update()
{
    Device::Update();
}
