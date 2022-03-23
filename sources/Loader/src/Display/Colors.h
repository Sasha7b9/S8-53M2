// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "DisplayTypes.h"


// Возвращает цвет синхронизации.
Color::E ColorTrig();
// Цвет заголовка страницы, inShade - затенена ли страница.
Color::E ColorMenuTitle(bool inShade);
// Цвет элемента меню.
Color::E ColorMenuItem(bool inShade);
// Цвет окантовки меню.
Color::E ColorBorderMenu(bool inShade);
// Возвращает цвет, контрастный к color. Может быть белым или чёрным.
Color::E ColorContrast(Color::E color);
// Светлый цвет в тени.
Color::E LightShadingTextColor();


#define ColorMenuField()            Color::MENU_FIELD
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


#define MAKE_COLOR(r, g, b) (((b) & 0x1f) + (((g) & 0x3f) << 5) + (((r) & 0x1f) << 11))
#define R_FROM_COLOR(color) (((uint16)(color) >> 11) & (uint16)0x1f)
#define G_FROM_COLOR(color) (((uint16)(color) >> 5) & (uint16)0x3f)
#define B_FROM_COLOR(color) ((uint16)(color) & 0x1f)

extern Color::E gColorFill;
extern Color::E gColorBack;
extern Color::E gColorGrid;
extern Color::E gColorChan[4];
