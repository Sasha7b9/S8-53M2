// 2022/02/11 15:58:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "DisplayTypes.h"


Color::E ColorChannel(Chan::E);
Color::E ColorCursors(Chan::E);
Color::E ColorTrig();                  // ���������� ���� �������������.
Color::E ColorMenuTitle                    // ���� ��������� ��������.
                    (bool inShade       // �������� �� ��������.
                    );
Color::E ColorMenuItem(bool inShade);      // ���� �������� ����.
Color::E ColorBorderMenu(bool inShade);    // ���� ��������� ����.
Color::E ColorContrast(Color::E color);       // ���������� ����, ����������� � color. ����� ���� ����� ��� ������.
Color::E LightShadingTextColor();      // ������� ���� � ����.

#define COLOR_BACK Color::BLACK
#define COLOR_FILL Color::WHITE
#define ColorGrid() Color::GRID


#define ColorMenuField() Color::MENU_FIELD
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
};

void Color_Log(Color::E color);           // ������� � ��� �������� �����.
void Color_BrightnessChange(ColorType *colorType, int delta);
void Color_SetBrightness(ColorType *colorType, float brightness);
void Color_Init(ColorType *colorType);
void Color_ComponentChange(ColorType *colorType, int delta);


pchar  NameColorFromValue(uint16 colorValue);
pchar  NameColor(Color::E color);
#define MAKE_COLOR(r, g, b) ((uint16)(((b) & 0x1f) + (((g) & 0x3f) << 5) + (((r) & 0x1f) << 11)))

#define R_FROM_COLOR(color) (((uint)(color) >> 16) & (uint)0xFF)
#define G_FROM_COLOR(color) (((uint)(color) >> 8) & (uint)0xFF)
#define B_FROM_COLOR(color) ((uint)(color) & 0xFF)
#define COLOR(i) set.display.colors[i]
