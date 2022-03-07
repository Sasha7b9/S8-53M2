// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu.h"
#include "MenuItems.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include <stm32f4xx_hal.h>


int8 MACaddress::cur_digit = 0;
int8 IPaddress::cur_digit = 0;
int8 Governor::cur_digit = 0;

Item *Item::underKey = nullptr;
void *Hint::item = nullptr;


// ������������ ��� �������� ��������� �������� Choice
struct TimeStructChoice
{
    Choice* choice;                 // ����� Choice, ������� ��������� � ������ ������ � ��������. ���� 0 - ��� ��������.
    uint    timeStartMS;            // ����� ������ �������� choice.
    uint    inMoveIncrease : 1;
    uint    inMoveDecrease : 1;
};

static TimeStructChoice tsChoice;



Item::Item(const ItemStruct *str) : type(str->type), keeper(str->keeper), funcOfActive(str->funcOfActive)
{
    titleHint[0] = str->titleHint[0];
    titleHint[1] = str->titleHint[1];
    titleHint[2] = str->titleHint[2];
    titleHint[3] = str->titleHint[3];
}


Item::Item(TypeItem::E type_, const Page* keeper_, pFuncBV funcOfActive_, pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN) :
    type(type_), keeper(keeper_), funcOfActive(funcOfActive_)
{
    titleHint[0] = titleRU;
    titleHint[1] = titleEN;
    titleHint[2] = hintRU;
    titleHint[3] = hintEN;
}


Page::Page(const Page *keeper_, pFuncBV funcOfActive_, pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, 
           NamePage::E name_, const arrayItems *items_, pFuncVV funcOnPress_, pFuncVV funcOnDraw_, pFuncVI funcRegSetSB_) :
    Item(TypeItem::Page, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    name(name_), items(items_), funcOnPress(funcOnPress_), funcOnDraw(funcOnDraw_), funcRegSetSB(funcRegSetSB_)
{
}


Button::Button(const Page *keeper_, pFuncBV funcOfActive_,
       pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, pFuncVV funcOnPress_) :
    Item(TypeItem::Button, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    funcOnPress(funcOnPress_)
{
};


SmallButton::SmallButton(const Page *keeper_, pFuncBV funcOfActive_,
            pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
            pFuncVV funcOnPress_, pFuncVII funcOnDraw_, const arrayHints *hintUGO_) :
    Item(TypeItem::SmallButton, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    funcOnPress(funcOnPress_), funcOnDraw(funcOnDraw_), hintUGO(hintUGO_)
{
}


Governor::Governor(const Page *keeper_, pFuncBV funcOfActive_,
         pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
         int16 *cell_, int16 minValue_, int16 maxValue_, pFuncVV funcOfChanged_, pFuncVV funcBeforeDraw_) :
    Item(TypeItem::Governor, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    cell(cell_), minValue(minValue_), maxValue(maxValue_), funcOfChanged(funcOfChanged_), funcBeforeDraw(funcBeforeDraw_)
{
}


bool Item::IsPressed()
{
    return (this == UnderKey());
}


IPaddress::IPaddress(const IPaddressStruct *str) :
    Item(&str->str), ip0(str->ip0), ip1(str->ip1), ip2(str->ip2), ip3(str->ip3), funcOfChanged(str->funcOfChanged), port(str->port)
{

}


pchar Choice::NameSubItem(int i)
{
    return names[i][LANG];
}


pchar Choice::NameCurrentSubItem()
{
    return (cell == 0) ? "" : names[*cell][LANG];
}


pchar Choice::NameNextSubItem()
{
    if (cell == 0)
    {
        return "";
    }
    int index = *cell + 1;
    if (index == NumSubItems())
    {
        index = 0;
    }
    return names[index][LANG];
}


pchar Choice::NamePrevSubItem()
{
    if (cell == 0)
    {
        return "";
    }
    int index = *cell - 1;
    if (index < 0)
    {
        index = NumSubItems() - 1;
    }
    return names[index][LANG];
}


int Choice::NumSubItems()
{
    int i = 0;
    for (; i < MAX_NUM_SUBITEMS_IN_CHOICE; i++)
    {
        if (names[i][LANG] == 0)
        {
            return i;
        }
    }
    return i;
}


void Choice::StartChange(int delta)
{
    if (tsChoice.choice != 0)
    {
        return;
    }
    Sound::GovernorChangedValue();
    if (SHOW_HELP_HINTS)
    {
        SetItemForHint(this);
    }
    else if (!Menu::ItemIsActive(this))
    {
        FuncOnChanged(false);
    }
    else
    {
        tsChoice.choice = this;
        tsChoice.timeStartMS = TIME_MS;
        if (delta > 0)
        {
            tsChoice.inMoveIncrease = 1;
        }
        else if (delta < 0)
        {
            tsChoice.inMoveDecrease = 1;
        }
    }
}


void Choice::FuncOnChanged(bool active)
{
    if (funcOnChanged)
    {
        funcOnChanged(active);
    }
}


void Choice::FuncForDraw(int x, int y)
{
    if (funcForDraw)
    {
        funcForDraw(x, y);
    }
}


float Choice::Step()
{
    static const float speed = 0.1f;
    static const int numLines = 12;
    if (tsChoice.choice == this)
    {
        float delta = speed * (TIME_MS - tsChoice.timeStartMS);
        if (delta == 0.0f)
        {
            delta = 0.001f; // ������ � ��������� ������ ������ ����� ��������, ��� ������ 0 ��, �� �� ���������� ������� �����, ������ ��� ���� ����� �������� � ���, ��� �������� ���
        }
        if (tsChoice.inMoveIncrease == 1)
        {
            if (delta <= numLines)
            {
                return delta;
            }
            CircleIncreaseInt8(cell, 0, NumSubItems() - 1);
        }
        else if (tsChoice.inMoveDecrease == 1)
        {
            delta = -delta;

            if (delta >= -numLines)
            {
                return delta;
            }
            CircleDecreaseInt8(cell, 0, NumSubItems() - 1);
        }
        tsChoice.choice = 0;
        FuncOnChanged(Menu::ItemIsActive(this));
        Display::Redraw();
        tsChoice.inMoveDecrease = tsChoice.inMoveIncrease = 0;
        return 0.0f;
    }
    return 0.0f;
}


void Choice::ChangeValue(int delta)
{
    if (delta < 0)
    {
        int8 value = (*cell == NumSubItems() - 1) ? 0 : (*cell + 1);
        *cell = value;
    }
    else
    {
        int8 value = (*cell == 0) ? (NumSubItems() - 1) : (*cell - 1);
        *cell = value;
    }
    FuncOnChanged(Menu::ItemIsActive(this));
    Sound::GovernorChangedValue();
    Display::Redraw();
}
