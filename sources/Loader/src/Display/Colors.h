// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "DisplayTypes.h"


// ���������� ���� �������������.
Color::E ColorTrig();
// ���� ��������� ��������, inShade - �������� �� ��������.
Color::E ColorMenuTitle(bool inShade);
// ���� �������� ����.
Color::E ColorMenuItem(bool inShade);
// ���� ��������� ����.
Color::E ColorBorderMenu(bool inShade);
// ���������� ����, ����������� � color. ����� ���� ����� ��� ������.
Color::E ColorContrast(Color::E color);
// ������� ���� � ����.
Color::E LightShadingTextColor();


#define ColorMenuField()            Color::MENU_FIELD
// ���� ����� ������� ����, ��� ���� �������� ����. ������������ ��� �������� ������� ������.
#define ColorMenuItemLessBright()   Color::MENU_ITEM_DARK
// ���� ����� ������� ����, ��� ���� ��������� ��������. ������������ ��� �������� ������� ������.
#define ColorMenuTitleBrighter()    Color::MENU_TITLE_BRIGHT
// ���� ����� ������� ����, ��� ���� ��������� ��������. ������������ ��� �������� ������� ������.
#define ColorMenuTitleLessBright()  Color::MENU_TITLE_DARK
// ���� ����� ������� ����, ��� ���� �������� ����. ������������ ��� �������� ������� ������.
#define ColorMenuItemBrighter()     Color::MENU_ITEM_BRIGHT
// Ҹ���� ���� � ����.
#define DarkShadingTextColor()      Color::MENU_TITLE_DARK


#define MAKE_COLOR(r, g, b) (((b) & 0x1f) + (((g) & 0x3f) << 5) + (((r) & 0x1f) << 11))
#define R_FROM_COLOR(color) (((uint16)(color) >> 11) & (uint16)0x1f)
#define G_FROM_COLOR(color) (((uint16)(color) >> 5) & (uint16)0x3f)
#define B_FROM_COLOR(color) ((uint16)(color) & 0x1f)

extern Color::E gColorFill;
extern Color::E gColorBack;
extern Color::E gColorGrid;
extern Color::E gColorChan[4];
