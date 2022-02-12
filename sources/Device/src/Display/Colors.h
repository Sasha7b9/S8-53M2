// 2022/02/11 15:58:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "DisplayTypes.h"


Color::E ColorChannel(Chan::E);
Color::E ColorCursors(Chan::E);
Color::E ColorTrig();                  // Возвращает цвет синхронизации.
Color::E ColorMenuTitle                    // Цвет заголовка страницы.
                    (bool inShade       // Затенена ли страница.
                    );
Color::E ColorMenuItem(bool inShade);      // Цвет элемента меню.
Color::E ColorBorderMenu(bool inShade);    // Цвет окантовки меню.
Color::E ColorContrast(Color::E color);       // Возвращает цвет, контрастный к color. Может быть белым или чёрным.
Color::E LightShadingTextColor();      // Светлый цвет в тени.

#define COLOR_BACK Color::BLACK
#define COLOR_FILL Color::WHITE
#define ColorGrid() COLOR_GRID


#define ColorMenuField() COLOR_MENU_FIELD
// Чуть менее светлый цвет, чем цвет элемента меню. Используется для создания эффекта объёма.
#define ColorMenuItemLessBright()   Color::MENU_ITEM_DARK
// Чуть более светлый цвет, чем цвет заголовка страницы. Используется для создания эффекта объёма.
#define ColorMenuTitleBrighter()    COLOR_MENU_TITLE_BRIGHT
// Чуть менее светлый цвет, чем цвет заголовка страницы. Используется для создания эффекта объёма.
#define ColorMenuTitleLessBright()  COLOR_MENU_TITLE_DARK
// Чуть более светлый цвет, чем цвет элемента меню. Используется для создания эффекта объёма.
#define ColorMenuItemBrighter()     COLOR_MENU_ITEM_BRIGHT
// Тёмный цвет в тени.
#define DarkShadingTextColor()      COLOR_MENU_TITLE_DARK


typedef struct
{
    float   red;
    float   green;
    float   blue;
    float   stepRed;
    float   stepGreen;
    float   stepBlue;
    float   brightness;
    Color::E   color;
    int8    currentField;
    bool    alreadyUsed;
} ColorType;

void Color_Log(Color::E color);           // Вывести в лог значение цвета.
void Color_BrightnessChange(ColorType *colorType, int delta);
void Color_SetBrightness(ColorType *colorType, float brightness);
void Color_Init(ColorType *colorType);
void Color_ComponentChange(ColorType *colorType, int delta);


const char* NameColorFromValue(uint16 colorValue);
const char* NameColor(Color::E color);
#define MAKE_COLOR(r, g, b) ((uint16)(((b) & 0x1f) + (((g) & 0x3f) << 5) + (((r) & 0x1f) << 11)))
#define R_FROM_COLOR(color) (((uint16)(color) >> 11) & (uint16)0x1f)
#define G_FROM_COLOR(color) (((uint16)(color) >> 5) & (uint16)0x3f)
#define B_FROM_COLOR(color) ((uint16)(color) & 0x1f)
