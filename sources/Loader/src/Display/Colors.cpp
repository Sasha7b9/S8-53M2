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


static pchar  colorNames[] =
{
    "Color::BLACK",
    "Color::WHITE",
    "Color::GRID",
    "Color::DATA_A",
    "Color::DATA_B",
    "Color::MENU_FIELD",
    "Color::MENU_TITLE",
    "COLOM_MENU_TITLE_DARK",
    "Color::MENU_TITLE_BRIGHT",
    "Color::MENU_ITEM",
    "Color::MENU_ITEM_DARK",
    "Color::MENU_ITEM_BRIGHT",
    "COLOR_DATA_A_WHITE",
    "COLOR_DATA_B_WHITE",
    "COLOR_CURSORS_A",
    "COLOR_CURSORS_B",
    "Color::COUNT",
    "Color::FLASH_10",
    "Color::FLASH_01",
    "INVERSE"
};


pchar  NameColorFromValue(uint16 colorValue)
{
    for (int i = 0; i < Color::COUNT; i++)
    {
        if (set.display.colors[(Color::E)i] == colorValue)
        {
            return colorNames[i];
        }
    }
    return "Sorry, this color is not in the palette";
}


pchar  NameColor(Color::E color)
{
    return colorNames[color];
}
