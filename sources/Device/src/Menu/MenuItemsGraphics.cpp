// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Menu.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Display/font/Font.h"
#include <string.h>
#include <stdio.h>


static void DrawGovernorChoiceColorFormulaHiPart(Item *item, int x, int y, bool pressed, bool shade, bool opened)
{
    int delta = pressed && !shade ? 1 : 0;
    int width = MI_WIDTH_VALUE;

    if (item->GetType() == TypeItem::IP && opened && ((IPaddress*)item)->port != 0)
    {
        width += MOI_WIDTH_D_IP;
    }

    Color::E color = shade ? ColorMenuTitleLessBright() : (IS_COLOR_SCHEME_WHITE_LETTERS ? Color::WHITE : Color::BLACK);
    Painter::DrawHLine(y + 1, x, x + width + 3, ColorBorderMenu(false));

    if (shade)
    {
        Painter::FillRegion(x + 1, y + 2, width + 2, MI_HEIGHT_VALUE + 3, ColorMenuItem(false));
    }
    else
    {
        Painter::DrawVolumeButton(x + 1, y + 2, width + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuItem(false),
            ColorMenuItemBrighter(), ColorMenuItemLessBright(), pressed, shade);
    }

    PText::Draw(x + 6 + delta, y + 6 + delta, item->Title(), color);
    
    TypeItem::E type = item->GetType();

    if(Item::Current() == item)
    {
        char symbol = 0;

        if (type == TypeItem::Governor)
        {
            symbol = GetSymbolForGovernor(*((Governor*)item)->cell);
            Governor::address = item;
        }
        else if (type == TypeItem::ChoiceReg || (item->IsOpened() && type == TypeItem::Choice))
        {
            symbol = GetSymbolForGovernor(*((Choice*)item)->cell);
        }
        else if (type == TypeItem::Time)
        {
            Time *time = (Time*)item;
            if ((Item::Opened() == item) && (*time->curField != iEXIT) && (*time->curField != iSET))
            {
                int8 values[7] =
                {
                    0,
                    *time->day,
                    *time->month,
                    *time->year,
                    *time->hours,
                    *time->minutes,
                    *time->seconds
                };
                symbol = GetSymbolForGovernor(values[*time->curField]);
            }
        }

        PText::Draw4SymbolsInRect(x + MI_WIDTH - 13, y + 5 + (item->IsOpened() ? 0 : 15), symbol, IS_COLOR_SCHEME_WHITE_LETTERS ? COLOR_BACK : COLOR_FILL);
    }
}

void Governor::DrawLowPart(int x, int y, bool, bool shade)
{
    char buffer[20];
    
    Color::E colorTextDown = COLOR_BACK;

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(), 
        ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);

    if(shade)
    {
        colorTextDown = ColorMenuItem(false);
    }

    x = PText::Draw(x + 4, y + 21, "\x80", colorTextDown);

    if(Item::Opened() != this)
    {
        float delta = Step();

        if(delta == 0.0f)
        {
            x = PText::Draw(x + 1, y + 21, Int2String(*cell, false, 1, buffer));
        }
        else
        {
            int drawX = x + 1;
            int limX = x + 1;
            int limY = y + 19;
            int limWidth = MI_WIDTH_VALUE;
            int limHeight = MI_HEIGHT_VALUE - 1;

            if(delta > 0.0f)
            {
                x = PText::DrawWithLimitation(drawX, y + 21 - delta, Int2String(*cell, false, 1, buffer),
                                            COLOR_BACK, limX, limY, limWidth, limHeight);
                PText::DrawWithLimitation(drawX, y + 21 + 10 - delta, Int2String(NextValue(), false, 1, buffer),
                                            COLOR_BACK, limX, limY, limWidth, limHeight);
            }

            if(delta < 0.0f)
            {
                x = PText::DrawWithLimitation(drawX, y + 21 - delta, Int2String(*cell, false, 1, buffer),
                                            COLOR_BACK, limX, limY, limWidth, limHeight);
                PText::DrawWithLimitation(drawX, y + 21 - 10 - delta, Int2String(PrevValue(), false, 1, buffer),
                    COLOR_BACK, limX, limY, limWidth, limHeight);
            }
        }
    }
    else
    {
        x = PText::Draw(x + 1, y + 21, Int2String(*cell, false, 1, buffer), COLOR_FILL);
    }

    PText::Draw(x + 1, y + 21, "\x81", colorTextDown);
}

void IPaddress::DrawLowPart(int x, int y, bool, bool shade)
{
    char buffer[20];

    Color::E colorTextDown = COLOR_BACK;

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(),
                             ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    if (shade)
    {
        colorTextDown = ColorMenuItem(false);
    }

    sprintf(buffer, "%03d.%03d.%03d.%03d", *ip0, *ip1, *ip2, *ip3);

    if (Item::Opened() != this)
    {
        
        PText::Draw(x + 4, y + 21, buffer, colorTextDown);
    }
    else
    {
        PText::Draw(x + 4, y + 21, buffer, COLOR_FILL);
    }
}

static void DrawMACaddressLowPart(MACaddress *mac, int x, int y, bool, bool shade)
{
    char buffer[20];

    Color::E colorTextDown = COLOR_BACK;

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(),
                             ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    if (shade)
    {
        colorTextDown = ColorMenuItem(false);
    }

    sprintf(buffer, "%02X.%02X.%02X.%02X.%02X.%02X", *mac->mac0, *mac->mac1, *mac->mac2, *mac->mac3, *mac->mac4, *mac->mac5);

    if (Item::Opened() != (Item *)mac)
    {

        PText::Draw(x + 4, y + 21, buffer, colorTextDown);
    }
    else
    {
        PText::Draw(x + 4, y + 21, buffer, COLOR_FILL);
    }
}


void Formula::WriteText(int x, int y, bool)
{
    if (*function != Function_Mul && *function != Function_Sum)
    {
        return;
    }

    bool funcIsMul = (*function == Function_Mul);
    int8 koeff1 = funcIsMul ? *koeff1mul : *koeff1add;
    int8 koeff2 = funcIsMul ? *koeff2mul : *koeff2add;

    if (koeff1 != 0)
    {
       PText::DrawChar(x, y, koeff1 < 0 ? '-' : '+');
    }

    PText::DrawChar(x + 5, y, (char)(koeff1 + 0x30));
    PText::DrawChar(x + 10, y, '*');
    PText::Draw(x + 14, y, "K1");
    PText::DrawChar(x + 27, y, funcIsMul ? '*' : '+');

    if (koeff2 != 0)
    {
       PText::DrawChar(x + 30, y, koeff2 < 0 ? '-' : '+');
    }

    PText::DrawChar(x + 39, y, (char)(koeff2 + 0x30));
    PText::DrawChar(x + 44, y, '*');
    PText::Draw(x + 48, y, "K2");
}


void Formula::DrawLowPart(int x, int y, bool, bool shade)
{
    Color::E colorTextDown = COLOR_BACK;

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(),
                     ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    if (shade)
    {
        colorTextDown = ColorMenuItem(false);
    }

    Color::SetCurrent(colorTextDown);
    WriteText(x + 6, y + 21, false);
}


void Governor::DrawClosed(int x, int y)
{
    bool pressed = IsPressed();
    bool shade = IsShade() || !IsActive();
    DrawLowPart(x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(this, x, y, pressed, shade, false);
}


static void ItemIPaddress_DrawClosed(IPaddress *ip, int x, int y)
{
    bool pressed = ((Item *)ip)->IsPressed();
    bool shade = ((Item *)ip)->IsShade() || !((Item *)ip)->IsActive();
    ip->DrawLowPart(x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(ip, x, y, pressed, shade, false);
}

static void ItemMACaddress_DrawClosed(MACaddress *mac, int x, int y)
{
    bool pressed = ((Item *)mac)->IsPressed();
    bool shade = ((Item *)mac)->IsShade() || !((Item *)mac)->IsActive();
    DrawMACaddressLowPart(mac, x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart((Item *)mac, x, y, pressed, shade, false);
}

void Formula_DrawClosed(Formula *formula, int x, int y)
{
    bool pressed = ((Item *)formula)->IsPressed();
    bool shade = ((Item *)formula)->IsShade() || !((Item *)formula)->IsActive();
    formula->DrawLowPart(x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart((Item *)formula, x, y, pressed, shade, false);
}

static void DrawValueWithSelectedPosition(int x, int y, int value, int numDigits, int selPos, bool hLine, bool fillNull) // Если selPos == -1, подсвечивать не нужно
{
    int firstValue = value;
    int height = hLine ? 9 : 8;
    for (int i = 0; i < numDigits; i++)
    {
        int rest = value % 10;
        value /= 10;
        if (selPos == i)
        {
            Painter::FillRegion(x - 1, y, 5, height, COLOR_FILL);
        }
        if (!(rest == 0 && value == 0) || (firstValue == 0 && i == 0))
        {
            PText::DrawChar(x, y, rest + 48, selPos == i ? COLOR_BACK : COLOR_FILL);
        }
        else if (fillNull)
        {
            PText::DrawChar(x, y, '0', selPos == i ? COLOR_BACK : COLOR_FILL);
        }
        if (hLine)
        {
            Painter::DrawLine(x, y + 9, x + 3, y + 9, COLOR_FILL);
        }
        x -= 6;
    }
}

void DrawGovernorValue(int x, int y, Governor *governor)
{
    char buffer[20];

    int startX = x + 40;
    int16 value = *governor->cell;
    int signGovernor = *governor->cell < 0 ? -1 : 1;
    if(signGovernor == -1)
    {
        value = -value;
    }

    Font::Set(TypeFont::_5);
    bool sign = governor->minValue < 0;
    PText::Draw(x + 55, y - 5, Int2String(governor->maxValue, sign, 1, buffer), COLOR_FILL);
    PText::Draw(x + 55, y + 2, Int2String(governor->minValue, sign, 1, buffer));
    Font::Set(TypeFont::_8);

    DrawValueWithSelectedPosition(startX, y, value, governor->NumDigits(), Governor::cur_digit, true, true);

    if(sign)
    {
       PText::DrawChar(startX - 1, y, signGovernor < 0 ? '\x9b' : '\x9a');
    }
}

static void DrawIPvalue(int x, int y, IPaddress *ip)
{
    if (IPaddress::cur_digit > (ip->port == 0 ? 11 : 16))
    {
        IPaddress::cur_digit = 0;
    }

    uint8 *bytes = ip->ip0;

    x += 15;

    y += 1;

    int numIP = 0;
    int selPos = 0;

    ip->GetNumPosIPvalue(&numIP, &selPos);

    for (int i = 0; i < 4; i++)
    {
        DrawValueWithSelectedPosition(x, y, bytes[i], 3, numIP == i ? selPos : -1, false, true);
        if (i != 3)
        {
            PText::DrawChar(x + 5, y, '.', COLOR_FILL);
        }
        x += 19;
    }

    if (ip->port != 0)
    {
        PText::DrawChar(x - 13, y, ':', COLOR_FILL);
        DrawValueWithSelectedPosition(x + 14, y, *ip->port, 5, numIP == 4 ? selPos : -1, false, true);
    }
}


void MACaddress::DrawValue(int x, int y)
{
    if (MACaddress::cur_digit > 5)
    {
        MACaddress::cur_digit = 0;
    }

    uint8 *bytes = mac0;
    x += MOI_WIDTH - 14;
    y++;

    for (int num = 5; num >= 0; num--)
    {
        int value = (int)(*(bytes + num));
        if (MACaddress::cur_digit == num)
        {
            Painter::FillRegion(x - 1, y, 10, 8, COLOR_FILL);
        }
        char buffer[20];
        sprintf(buffer, "%02X", value);
        PText::Draw(x, y, buffer, (MACaddress::cur_digit == num) ? COLOR_BACK : COLOR_FILL);
        x -= 12;
    }
}


void Governor::Draw(int x, int y, bool opened)
{
    if (funcBeforeDraw)
    {
        funcBeforeDraw();
    }
    if(opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        DrawClosed(x, y);
    }
}


void IPaddress::Draw(int x, int y, bool opened)
{
    if (opened)
    {
        DrawOpened(x - (port == 0 ? 0 : MOI_WIDTH_D_IP), y);
    }
    else
    {
        ItemIPaddress_DrawClosed(this, x, y);
    }
}


void MACaddress::Draw(int x, int y, bool opened)
{
    if (opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        ItemMACaddress_DrawClosed(this, x, y);
    }
}

void Formula::Draw(int x, int y, bool opened)
{
    if (opened)
    {

    }
    else
    {
        Formula_DrawClosed(this, x, y);
    }
}

void GovernorColor::DrawValue(int x, int y, int delta)
{
    char buffer[20];
    
    ColorType *ct = colorType;
    int8 field = ct->currentField;
    char *texts[4] = {"Яр", "Сн", "Зл", "Кр"};
    int color = (int)COLOR(ct->color);
    int red = (int)R_FROM_COLOR(color);
    int green = (int)G_FROM_COLOR(color);
    int blue = (int)B_FROM_COLOR(color);

    if(!ct->alreadyUsed)
    {
        ct->Init();
    }

    int vals[4] = {(int)(ct->brightness * 100), blue, green, red};

    Painter::FillRegion(x, y, MI_WIDTH + delta - 2, MI_HEIGHT / 2 - 3, Color::BLACK);
    x += 92;
    
    for(int i = 0; i < 4; i++)
    {
        Color::E colorBack = (field == i) ? Color::WHITE : Color::BLACK;
        Color::E colorDraw = (field == i) ? Color::BLACK : Color::WHITE;
        Painter::FillRegion(x - 1, y + 1, 29, 10, colorBack);
        PText::Draw(x, y + 2, texts[i], colorDraw);
        PText::Draw(x + 14, y + 2, Int2String(vals[i], false, 1, buffer));
        x -= 30;
    }
    
}

void GovernorColor::DrawOpened(int x, int y)
{
    static const int delta = 43;
    x -= delta;
    colorType->Init();
    Painter::DrawRectangle(x - 1, y - 1, MI_WIDTH + delta + 2, MI_HEIGHT + 2, Color::BLACK);
    Painter::DrawRectangle(x, y, MI_WIDTH + delta, MI_HEIGHT, ColorMenuTitle(false));
    Painter::DrawVolumeButton(x + 1, y + 1, MI_WIDTH_VALUE + 2 + delta, MI_HEIGHT_VALUE + 3, 2, ColorMenuItem(false), 
        ColorMenuItemBrighter(), ColorMenuItemLessBright(), ((Item *)this)->IsPressed(), ((Item *)this)->IsShade());
    Painter::DrawHLine(y + MI_HEIGHT / 2 + 2, x, x + MI_WIDTH + delta, ColorMenuTitle(false));
    PText::DrawStringInCenterRect(x + (((Item *)this)->IsPressed() ? 2 : 1), y + (((Item *)this)->IsPressed() ? 2 : 1),
        MI_WIDTH + delta, MI_HEIGHT / 2 + 2, ((Item *)this)->Title(), Color::WHITE);
    DrawValue(x + 1, y + 19, delta);
}

void GovernorColor::DrawClosed(int x, int y)
{
    colorType->Init();
    DrawGovernorChoiceColorFormulaHiPart((Item *)this, x, y, ((Item *)this)->IsPressed(), ((Item *)this)->IsShade() ||
        !((Item *)this)->IsActive(), true);
    Painter::FillRegion(x + 2, y + 20, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1, colorType->color);
}

void GovernorColor::Draw(int x, int y, bool opened)
{
    if(opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        DrawClosed(x, y);
    }
}

void Choice::DrawOpened(int x, int y)
{
    int height = ((Item *)this)->HeightOpened();

    Painter::DrawRectangle(x - 1, y - 1, MP_TITLE_WIDTH + 2, height + 3, COLOR_BACK);
    
    DrawGovernorChoiceColorFormulaHiPart((Item *)this, x - 1, y - 1, ((Item *)this)->IsPressed(), false, true);
    Painter::DrawRectangle(x - 1, y, MP_TITLE_WIDTH + 1, height + 1, ColorMenuTitle(false));
 
    Painter::DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH);
    Painter::DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1, height - MOI_HEIGHT_TITLE, 1, Color::BLACK, ColorMenuTitleBrighter(),
                        ColorMenuTitleLessBright(), false, false);
    int index = *((int8*)cell);
    for(int i = 0; i < NumSubItems(); i++)
    {
        int yItem = y + MOI_HEIGHT_TITLE + i * MOSI_HEIGHT + 1;
        bool pressed = i == index;
        if(pressed)
        {
            Painter::DrawVolumeButton(x + 1, yItem, MOI_WIDTH - 2 , MOSI_HEIGHT - 2, 2, ColorMenuField(), ColorMenuTitleBrighter(),
                ColorMenuTitleLessBright(), pressed, false);
        }
        PText::Draw(x + 4, yItem + 2, NameSubItem(i), pressed ? Color::BLACK : ColorMenuField());
    }
}

void Time::DrawOpened(int x, int y)
{
    char buffer[20];
    
    int width = MI_WIDTH_VALUE + 3;
    int height = 61;
    Painter::DrawRectangle(x - 1, y - 1, width + 2, height + 3, COLOR_BACK);
    DrawGovernorChoiceColorFormulaHiPart((Item *)this, x - 1, y - 1, ((Item *)this)->IsPressed(), false, true);

    Painter::DrawRectangle(x - 1, y, width + 1, height + 1, ColorMenuTitle(false));

    Painter::DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH);
    Painter::DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1, height - MOI_HEIGHT_TITLE, 1, Color::BLACK, ColorMenuTitleBrighter(),
                             ColorMenuTitleLessBright(), false, false);

    int y0 = 21;
    int y1 = 31;
    int y2 = 41;
    int y3 = 51;

    int x0 = 41;
    int dX = 13;
    int wS = 10;

    struct StructPaint
    {
        int x;
        int y;
        int width;
    } strPaint[8] =
    {
        {3,             y3, 60},    // Не сохранять
        {x0,            y0, wS},    // день
        {x0 + dX,       y0, wS},    // месяц
        {x0 + 2 * dX,   y0, wS},    // год
        {x0,            y1, wS},    // часы
        {x0 + dX,       y1, wS},    // мин
        {x0 + 2 * dX,   y1, wS},    // сек
        {3,             y2, 46}     // Сохранить
    };

    char strI[8][20];
    strcpy(strI[iEXIT],     "Не сохранять");
    strcpy(strI[iDAY],      Int2String(*day,      false, 2, buffer));
    strcpy(strI[iMONTH],    Int2String(*month,    false, 2, buffer));
    strcpy(strI[iYEAR],     Int2String(*year,     false, 2, buffer));
    strcpy(strI[iHOURS],    Int2String(*hours,    false, 2, buffer));
    strcpy(strI[iMIN],      Int2String(*minutes,  false, 2, buffer));
    strcpy(strI[iSEC],      Int2String(*seconds,  false, 2, buffer));
    strcpy(strI[iSET],      "Сохранить");

    PText::Draw(x + 3, y + y0, "д м г - ", COLOR_FILL);
    PText::Draw(x + 3, y + y1, "ч м с - ");

    for (int i = 0; i < 8; i++)
    {
        if (*curField == i)
        {
            Painter::FillRegion(x + strPaint[i].x - 1, y + strPaint[i].y, strPaint[i].width, 8, Color::FLASH_10);
        }
        PText::Draw(x + strPaint[i].x, y + strPaint[i].y, strI[i], *curField == i ? Color::FLASH_01 : COLOR_FILL);
    }
}

static void GovernorIpCommon_DrawOpened(Item *item, int x, int y, int dWidth)
{
    int height = 34;
    Painter::DrawRectangle(x - 1, y - 1, MP_TITLE_WIDTH + 2 + dWidth, height + 3, COLOR_BACK);
    Painter::DrawRectangle(x - 1, y, MP_TITLE_WIDTH + 1 + dWidth, height + 1, ColorMenuTitle(false));
    Painter::DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH + dWidth);
    DrawGovernorChoiceColorFormulaHiPart((Item *)item, x - 1, y - 1, item->IsPressed(), false, true);
    Painter::DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1 + dWidth, height - MOI_HEIGHT_TITLE, 1, Color::BLACK, ColorMenuTitleBrighter(),
                             ColorMenuTitleLessBright(), false, false);
}

void Governor::DrawOpened(int x, int y)
{
    GovernorIpCommon_DrawOpened(this, x, y, 0);
    DrawGovernorValue(x, y + 22, this);
}

void IPaddress::DrawOpened(int x, int y)
{
    GovernorIpCommon_DrawOpened(this, x, y, port == 0 ? 0 : MOI_WIDTH_D_IP);
    DrawIPvalue(x, y + 22, this);
}

void MACaddress::DrawOpened(int x, int y)
{
    GovernorIpCommon_DrawOpened((Item *)this, x, y, 0);
    DrawValue(x, y + 22);
}

void Choice::DrawClosed(int x, int y)
{
    bool pressed = ((Item *)this)->IsPressed();
    bool shade = ((Item *)this)->IsShade() || !((Item *)this)->IsActive();
        
    if (shade)
    {
        Painter::FillRegion(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, ColorMenuTitleLessBright());
    }
    else
    {
        Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(), ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    }

    float deltaY = Step();
    Color::E colorText = shade ? LightShadingTextColor() : COLOR_BACK;
    Color::SetCurrent(colorText);
    if(deltaY == 0.0f)
    {
        PText::Draw(x + 4, y + 21, NameCurrentSubItem());
    }
    else
    {
        PText::DrawWithLimitation(x + 4, y + 21 - deltaY, NameCurrentSubItem(), colorText, x, y + 19, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1);
        Painter::DrawHLine(y + (deltaY > 0 ? 31 : 19) - deltaY, x + 3, x + MI_WIDTH_VALUE + 1, Color::BLACK);
        PText::DrawWithLimitation(x + 4, y + (deltaY > 0 ? 33 : 9) - deltaY, deltaY > 0 ? NameNextSubItem() : NamePrevSubItem(), colorText, x, y + 19, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1);
    }
    Painter::DrawHLine(y + MI_HEIGHT + 1, x, x + MI_WIDTH, ColorBorderMenu(false));

    DrawGovernorChoiceColorFormulaHiPart((Item *)this, x, y, pressed, shade, false);

    FuncForDraw(x, y);
}

void Choice::Draw(int x, int y, bool opened)
{
    if(opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        DrawClosed(x, y);
    }
}

void Time::DrawClosed(int x, int y)
{
    char buffer[20];
    
    bool pressed = ((Item *)this)->IsPressed();
    bool shade = ((Item *)this)->IsShade();
    DrawGovernorChoiceColorFormulaHiPart((Item *)this, x, y, pressed, shade, false);

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, shade ? ColorMenuTitleLessBright() : ColorMenuField(), 
        ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    //int delta = 0;

    int deltaField = 10;
    int deltaSeparator = 2;
    int startX = 3;
    y += 21;
    PackedTime time = HAL_RTC::GetPackedTime();
    PText::Draw(x + startX, y, Int2String((int)time.hours, false, 2, buffer), COLOR_BACK);
    PText::Draw(x + startX + deltaField, y, ":");
    PText::Draw(x + startX + deltaField + deltaSeparator, y, Int2String((int)time.minutes, false, 2, buffer));
    PText::Draw(x + startX + 2 * deltaField + deltaSeparator, y, ":");
    PText::Draw(x + startX + 2 * deltaField + 2 * deltaSeparator, y, Int2String((int)time.seconds, false, 2, buffer));

    startX = 44;
    PText::Draw(x + startX, y, Int2String((int)time.day, false, 2, buffer));
    PText::Draw(x + startX + deltaField, y, ":");
    PText::Draw(x + startX + deltaField + deltaSeparator, y, Int2String((int)time.month, false, 2, buffer));
    PText::Draw(x + startX + 2 * deltaField + deltaSeparator, y, ":");
    PText::Draw(x + startX + 2 * deltaField + 2 * deltaSeparator, y, Int2String((int)time.year, false, 2, buffer));
}

void Time::Draw(int x, int y, bool opened)
{
    if(opened)
    {
        DrawOpened(x, y);
    }
    else
    {
        DrawClosed(x, y);
    }
}


void Button::Draw(int x, int y)
{
    bool pressed = IsPressed();
    bool shade = IsShade() || !IsActive();

    Painter::DrawHLine(y + 1, x, x + MI_WIDTH, ColorMenuTitle(shade));
    Color::E color = shade ? Color::MENU_SHADOW : Color::WHITE;
    Painter::FillRegion(x + 1, y + 2, MI_WIDTH - 2, MI_HEIGHT - 2, ColorMenuItem(false));
    Painter::DrawVolumeButton(x + 4, y + 5, MI_WIDTH - 8, MI_HEIGHT - 8, 3, ColorMenuItem(false), ColorMenuItemBrighter(), 
                            ColorMenuItemLessBright(), pressed, shade);

    int delta = (pressed && (!shade)) ? 2 : 1;
    
    PText::DrawStringInCenterRect(x + delta, y + delta, MI_WIDTH, MI_HEIGHT, Title(), color);
}


void SmallButton::Draw(int x, int y)
{
    if (IsActive())
    {
        if (IsPressed())
        {
            Painter::FillRegion(x, y, WIDTH_SB, WIDTH_SB, COLOR_FILL);
            Color::SetCurrent(COLOR_BACK);
        }
        else
        {
            Painter::DrawRectangle(x, y, WIDTH_SB, WIDTH_SB, COLOR_FILL);
            Color::SetCurrent(COLOR_FILL);
        }
        funcOnDraw(x, y);
    }
    else
    {
        Painter::DrawRectangle(x, y, WIDTH_SB, WIDTH_SB, COLOR_FILL);
    }
}


void Page::Draw(int x, int y)
{
    bool isShade = IsShade() || !IsActive();
    bool isPressed = IsPressed();
    Painter::DrawHLine(y + 1, x, x + MI_WIDTH, ColorBorderMenu(false));

    if (isShade)
    {
        Painter::FillRegion(x + 1, y + 2, MI_WIDTH - 2, MI_HEIGHT - 2, ColorMenuTitleLessBright());
    }
    else
    {
        Painter::DrawVolumeButton(x + 1, y + 2, MI_WIDTH - 2, MI_HEIGHT - 2, 2, ColorMenuItem(isShade),
            ColorMenuItemBrighter(), ColorMenuItemLessBright(), isPressed, isShade);
    }

    Color::E colorText = isShade ? LightShadingTextColor() : Color::BLACK;
    int delta = 0;

    if(isPressed && (!isShade))
    {
        colorText = COLOR_FILL;
        delta = 1;
    }

    PText::DrawStringInCenterRect(x + delta, y + delta, MI_WIDTH, MI_HEIGHT, Title(), colorText);
}
