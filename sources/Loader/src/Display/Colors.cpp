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


Color::E ColorContrast(Color::E color)
{
    uint16 colorValue = set.display.colors[color];
    if (R_FROM_COLOR(colorValue) > 16 || G_FROM_COLOR(colorValue) > 32 || B_FROM_COLOR(colorValue) > 16)    //-V112
    {
        return Color::BLACK;
    }
    return Color::WHITE;
}


Color::E LightShadingTextColor()
{
    return ColorMenuTitle(false);
}


void Color_Log(Color::E)
{
}


/*
    јлгоритм изменени€ €ркости.
    1. »нициализаци€.
    а. –ассчитать €ркость по принципу - €ркость равна относительной интенсивности самого €ркого цветового канала
    б. –ассчитать шаг изменени€ цветовой составл€ющей каждого канала на 1% €ркости по формуле:
    Ўаг = »нтенсивность канала * яркость, где яркость - относительна€ величина общей €ркости
    в. ≈сли интенсивность всех каналов == 0, то дать полный шаг каждому каналу
    2. ѕри изменнении €ркости на 1% мен€ть интенсивность каждого канала на Ўаг, расчитанный в предыдущем пункте.
    3. ѕри изменени€ интенсивности цветового канала пересчитывать €ркость и шаг изменени€ каждого канала.
*/


static void ColorType_CalcSteps(ColorType *colorType)
{
    colorType->stepRed = colorType->red / (colorType->brightness * 100.0f);
    colorType->stepGreen = colorType->green / (colorType->brightness * 100.0f);
    colorType->stepBlue = colorType->blue / (colorType->brightness * 100.0f);
}


static void ColorType_SetBrightness(ColorType *colorType)
{
    colorType->brightness = MaxFloat(colorType->red / 255.0f, colorType->green / 255.0f, colorType->blue / 255.0f);

    ColorType_CalcSteps(colorType);
}


void Color_SetBrightness(ColorType *colorType, float brightness)
{
    int delta = (int)((brightness + 0.0005f) * 100.0f) - (int)(colorType->brightness * 100.0f);

    if (delta > 0)
    {
        for (int i = 0; i < delta; i++)
        {
            colorType->BrightnessChange(1);
        }
    }
    else
    {
        for (int i = 0; i < -delta; i++)
        {
            colorType->BrightnessChange(-1);
        }
    }
}


void ColorType::BrightnessChange(int )
{
//    if ((delta > 0 && colorType->brightness == 1.0f) || (delta < 0 && colorType->brightness == 0.0f))
//    {
//        return;
//    }
//
//    int sign = Math_Sign(delta);
//
//    LIMITATION(colorType->brightness, colorType->brightness + sign * 0.01f, 0.0f, 1.0f);
//
//    colorType->red += sign * colorType->stepRed;
//    colorType->green += sign * colorType->stepGreen;
//    colorType->blue += sign * colorType->stepBlue;
//
//    SetColor(colorType);
//
//    if (colorType->stepRed < 0.01f && colorType->stepGreen < 0.01f && colorType->stepBlue < 0.01f)
//    {
//        colorType->stepRed = 0.31f;
//        colorType->stepGreen = 0.63f;
//        colorType->stepBlue = 0.31f;
//    }
}


void Color_Init(ColorType *colorType, bool forced)
{
    if (forced)
    {
        colorType->alreadyUsed = false;
    }

    if (!colorType->alreadyUsed)
    {
        colorType->alreadyUsed = true;                  // ѕризнак того, что начальные установки уже произведены

        uint16 colorValue = set.display.colors[colorType->color];

        colorType->red = (float)R_FROM_COLOR(colorValue);
        colorType->green = (float)G_FROM_COLOR(colorValue);
        colorType->blue = (float)B_FROM_COLOR(colorValue);

        ColorType_SetBrightness(colorType);

        if (colorType->red == 0.0f && colorType->green == 0.0f && colorType->blue == 0.0f)
        {
            colorType->stepRed = 0.31f;
            colorType->stepGreen = 0.63f;
            colorType->stepBlue = 0.31f;
        }
    }
}


void Color_ComponentChange(ColorType * const , int )
{
//    static const float maxs[4] = {0.0f, 31.0f, 63.0f, 31.0f};
//    float * const pointers[4] = {0, &colorType->blue, &colorType->green, &colorType->red};
//    int8 index = colorType->currentField;
//
//    if (index >= 1 && index <= 3)
//    {
//        AddLimitationFloat(pointers[index], (float)Math_Sign(delta), 0.0f, maxs[index]);
//    }
//
//    SetColor(colorType);
//
//    ColorType_SetBrightness(colorType);
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
