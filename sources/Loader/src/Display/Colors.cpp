// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Colors.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Display/Painter.h"
#include <math.h>



Color::E gColorBack;
Color::E gColorFill;
Color::E gColorGrid;
Color::E gColorChan[4];



Color::E ColorMenuTitle(bool inShade)
{
    return inShade ? Color::MENU_ITEM : Color::MENU_TITLE;
}


Color::E ColorMenuItem(bool inShade)
{
    return inShade ? Color::MENU_ITEM_DARK : Color::MENU_ITEM;
}


Color::E ColorBorderMenu(bool inShade)
{
    return ColorMenuTitle(inShade);
}


Color::E LightShadingTextColor()
{
    return ColorMenuTitle(false);
}


void Color_Log(Color::E)
{
}


