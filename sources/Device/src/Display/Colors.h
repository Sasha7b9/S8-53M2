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
#define ColorGrid() Color::GRID


#define ColorMenuField() Color::MENU_FIELD
// Чуть менее светлый цвет, чем цвет элемента меню. Используется для создания эффекта объёма.
#define ColorMenuItemLessBright()   Color::MENU_ITEM_DARK
// Чуть более светлый цвет, чем цвет заголовка страницы. Используется для создания эффекта объёма.
#define ColorMenuTitleBrighter()    Color::MENU_TITLE_BRIGHT
// Чуть менее светлый цвет, чем цвет заголовка страницы. Используется для создания эффекта объёма.
#define ColorMenuTitleLessBright()  Color::MENU_TITLE_DARK
// Чуть более светлый цвет, чем цвет элемента меню. Используется для создания эффекта объёма.
#define ColorMenuItemBrighter()     Color::MENU_ITEM_BRIGHT
// Тёмный цвет в тени.
#define DarkShadingTextColor()      Color::MENU_TITLE_DARK


struct ColorType
{
    float    red;
    float    green;
    float    blue;
    float    stepRed;
    float    stepGreen;
    float    stepBlue;
    float    brightness;
    Color::E color;
    int8     currentField;
    bool     alreadyUsed;

    void BrightnessChange(int delta);
    void SetBrightness(float brightness);
    void Init();
    void ComponentChange(int delta);
private:
    void SetBrightness();
    void CalcSteps();
    void SetColor();
};

void Color_Log(Color::E color);           // Вывести в лог значение цвета.


pchar  NameColorFromValue(uint16 colorValue);
pchar  NameColor(Color::E color);

#define R_FROM_COLOR(color) (((uint)(color) >> 16) & (uint)0xFF)
#define G_FROM_COLOR(color) (((uint)(color) >> 8) & (uint)0xFF)
#define B_FROM_COLOR(color) ((uint)(color) & 0xFF)
#define COLOR(i) set.display.colors[i]
