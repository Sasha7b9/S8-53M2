// 2022/02/11 15:58:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "DisplayTypes.h"


// ��� �����
struct Color
{
    enum E
    {
        BLACK = 0x00,
        WHITE = 0x01,
        GRID = 0x02,
        DATA_A = 0x03,
        DATA_B = 0x04,
        MENU_FIELD = 0x05,
        MENU_TITLE = 0x06,
        MENU_TITLE_DARK = 0x07,
        MENU_TITLE_BRIGHT = 0x08,
        MENU_ITEM = 0x09,
        MENU_ITEM_DARK = 0x0a,
        MENU_ITEM_BRIGHT = 0x0b,
        MENU_SHADOW = 0x0c,
        EMPTY = 0x0d,
        EMPTY_A = 0x0e,
        EMPTY_B = 0x0f,
        Count,
        FLASH_10,
        FLASH_01,
        SET_INVERSE
    } value;

    static void ResetFlash();
    static void SetCurrent(Color::E);
    static Color::E GetCurrent();
    static uint Make(uint8 r, uint8 g, uint8 b);
private:
    static void CalculateColor();
    static void OnTimerFlashDisplay();
    static Color::E current;
    static bool inverse;
};


Color::E ColorChannel(Chan);
Color::E ColorCursors(Chan);
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

    void BrightnessChange(int delta);
    void SetBrightness(float brightness);
    void Init();
    void ComponentChange(int delta);
private:
    void SetBrightness();
    void CalcSteps();
    void SetColor();
};

void Color_Log(Color::E color);           // ������� � ��� �������� �����.


pchar  NameColorFromValue(uint16 colorValue);
pchar  NameColor(Color::E color);

#define R_FROM_COLOR(color) (((uint)(color) >> 16) & (uint)0xFF)
#define G_FROM_COLOR(color) (((uint)(color) >> 8) & (uint)0xFF)
#define B_FROM_COLOR(color) ((uint)(color) & 0xFF)
#define COLOR(i) set.display.colors[i]
