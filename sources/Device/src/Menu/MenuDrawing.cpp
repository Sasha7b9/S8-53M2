// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Data/Measures.h"
#include "Display/Tables.h"
#include "Panel/Panel.h"
#include "Menu/Menu.h"
#include "Display/Display.h"
#include "Display/Colors.h"
#include "Display/font/Font.h"
#include "Display/Painter.h"
#include "Display/Screen/Grid.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Data/Measures.h"
#include "Hardware/Timer.h"
#include "Menu/Pages/Definition.h"
#include <cstdio>


Item *ItemsUnderKey::items[Key::Count];


namespace Menu
{
    int ItemOpenedPosY(const Item *item);
}


void ItemsUnderKey::Set(Key::E key, Item *item)
{
    items[key] = item;
}


Item *ItemsUnderKey::Get(Key::E key)
{
    return items[key];
}


void ItemsUnderKey::Reset()
{
    for (int i = 0; i < Key::Count; i++)
    {
        items[i] = nullptr;
    }
}


Key::E GetFuncButtonFromY(int _y)
{
    int y = GRID_TOP + GRID_HEIGHT / 12;
    int step = GRID_HEIGHT / 6;

    Key::E buttons[6] = {Key::Menu, Key::F1, Key::F2, Key::F3, Key::F4, Key::F5};

    for(int i = 0; i < 6; i++)
    {
        if(_y < y)
        {
            return buttons[i];
        }

        y += step;
    }

    return  Key::F5;
}


static void DrawHintItem(int x, int y, int width)
{
    if (!Hint::item)
    {
        return;
    }

    pchar names[TypeItem::Count][2] =
    {
        {"",            ""},        // TypeItem::None
        {"",            ""},        // TypeItem::Choice
        {"������",      "Button"},  // TypeItem::Button
        {"��������",    "Page"},    // TypeItem::Page
        {"",            ""},        // TypeItem::Governor
        {"",            ""},        // TypeItem::Time
        {"",            ""},        // TypeItem::IP
        {"",            ""},        // Item_SwitchButton
        {"",            ""},        // TypeItem::GovernorColor
        {"",            ""},        // Item_Formula
        {"",            ""},        // TypeItem::MAC
        {"",            ""},        // TypeItem::ChoiceReg
        {"������",      "Button"}   // TypeItem::SmallButton
    };

    TypeItem::E type = Hint::item->GetType();
    Lang::E lang = LANG;
    Page *item = (Page*)Hint::item;

    const int SIZE = 100;
    char title[SIZE];
    std::snprintf(title, SIZE, "%s \"%s\"", names[type][lang], TITLE(item));

    if (item->type == TypeItem::SmallButton)
    {
        y -= 9;
    }

    PText::DrawStringInCenterRectAndBoundIt(x, y, width, 15, title, COLOR_BACK, COLOR_FILL);
    y = PText::DrawInBoundedRectWithTransfers(x, y + 15, width, HINT(item), COLOR_BACK, COLOR_FILL);

    if (item->type == TypeItem::SmallButton)
    {
        Painter::DrawHintsForSmallButton(x, y, width, (void *)item);
    }
}


void Menu::Draw()
{
    if (IsShown())
    {
        Item *item = Item::Opened();

        if (item->IsPage())
        {
            ItemsUnderKey::Reset();

            ((Page *)item)->DrawOpened(GRID_TOP);
        }
        else
        {
            item->Keeper()->DrawOpened(GRID_TOP);
        }
    }

    if (Hint::show)
    {
        const int x = 0;
        int y = 0;
        int width = IsMinimize() ? 289 : 220;
        PText::DrawInBoundedRectWithTransfers(x + 1, y, width - 1, //-V2007
            LANG_RU ?    "������� ����� ���������. � ���� ������ ��� ������� �� ������ �� ����� ��������� ���������� � � ����������. "
                                                "����� ��������� ���� �����, ������� ������ ������ � ����������� � � ������� 0.5�." : 
                                                "Mode is activated hints. In this mode, pressing the button displays the information on its purpose. "
                                                "To disable this mode, press the button HELP and hold it for 0.5s.",
                                                COLOR_BACK, COLOR_FILL);
        y += LANG_RU ? 49 : 40;

        if (Hint::string)
        {
            PText::DrawInBoundedRectWithTransfers(x + 1, y, width - 1, Hint::string, COLOR_BACK, COLOR_FILL); //-V2007
        }
        else if (Hint::item)
        {
            DrawHintItem(x + 1, y, width - 1); //-V2007
        }
    }
}


void Page::DrawTitle(int yTop)
{
    int x = MP_X;

    if (IsSB())
    {
        GetSmallButon(0)->Draw(LEFT_SB, yTop + 3);
        return;
    }

    int height = HeightOpened();
    bool shade = CurrentItemIsOpened();
    Painter::FillRegion(x - 1, yTop, MP_TITLE_WIDTH + 2, height + 2, COLOR_BACK);
    Painter::DrawRectangle(x, yTop, MP_TITLE_WIDTH + 1, height + 1, ColorBorderMenu(shade));

    if (shade)
    {
        Painter::FillRegion(x + 1, yTop + 1, MP_TITLE_WIDTH - 1, MP_TITLE_HEIGHT - 1, ColorMenuTitleLessBright());
        Painter::FillRegion(x + 4, yTop + 4, MP_TITLE_WIDTH - 7, MP_TITLE_HEIGHT - 7, Color::MENU_TITLE_DARK);
    }
    else
    {
        Painter::DrawVolumeButton(x + 1, yTop + 1, MP_TITLE_WIDTH - 1, MP_TITLE_HEIGHT - 1, 3, ColorMenuTitle(false),
            ColorMenuTitleBrighter(), ColorMenuTitleLessBright(), shade, false);
    }

    Painter::DrawVLine(x, yTop, yTop + HeightOpened(), ColorBorderMenu(false));
    bool condDrawRSet = NumSubPages() > 1 && Item::Current()->GetType() != TypeItem::ChoiceReg &&
        Item::Current()->GetType() != TypeItem::Governor && Item::TypeOpened() == TypeItem::Page;
    int delta = condDrawRSet ? -10 : 0;
    Color::E colorText = shade ? LightShadingTextColor() : Color::BLACK;
    x = PText::DrawStringInCenterRect(x, yTop, MP_TITLE_WIDTH + 2 + delta, MP_TITLE_HEIGHT, Title(), colorText);
    if(condDrawRSet)
    {
        PText::Draw4SymbolsInRect(x + 4, yTop + 11, GetSymbolForGovernor(NumCurrentSubPage()), colorText);
    }

    ItemsUnderKey::Set(GetFuncButtonFromY(yTop), this);

    delta = 0;

    Color::SetCurrent(colorText);
    DrawUGO(MP_X + MP_TITLE_WIDTH - 3 + delta, yTop + MP_TITLE_HEIGHT - 2 + delta);
}


void Page::DrawUGO(int right, int bottom)
{
    int size = 4;
    int delta = 2;
    
    int allPages = (NumItems() - 1) / MENU_ITEMS_ON_DISPLAY + 1;
    int currentPage = NumCurrentSubPage();

    int left = right - (size + 1) * allPages - delta + (3 - allPages);
    int top = bottom - size - delta;

    for(int p = 0; p < allPages; p++)
    {
        int x = left + p * (size + 2);
        if(p == currentPage)
        {
            Painter::FillRegion(x, top, size, size);
        }
        else
        {
            Painter::DrawRectangle(x, top, size, size);
        }
    }
}


static void DrawGovernor(void* item, int x, int y)
{
    ((Governor *)item)->Draw(x, y, false);
}


static void DrawIPaddress(void* item, int x, int y)
{
    ((IPaddress *)item)->Draw(x, y, false);
}


static void DrawMACaddress(void* item, int x, int y)
{
    ((MACaddress *)item)->Draw(x, y, false);
}


static void DrawFormula(void* item, int x, int y)
{
    ((Formula *)item)->Draw(x, y, false);
}


static void DrawChoice(void *item, int x, int y)
{
    ((Choice *)item)->Draw(x, y, false);
}


static void DrawSmallButton(void *item, int, int y)
{
    ((SmallButton *)item)->Draw(LEFT_SB, y + 7);
}


static void DrawTime(void *item, int x, int y)
{
    ((Time *)item)->Draw(x, y, false);
}


static void DrawGovernorColor(void *item, int x, int y)
{
    ((GovernorColor *)item)->Draw(x, y, false);
}


static void DrawButton(void *item, int x, int y)
{
    ((Button *)item)->Draw(x, y);
}


static void DrawPage(void *item, int x, int y)
{
    ((Page *)item)->Draw(x, y);
}


void Page::DrawItems(int yTop)
{
    void (*funcOfDraw[TypeItem::Count])(void*, int, int) = 
    {  
        EmptyFuncpVII,      // TypeItem::None
        DrawChoice,         // TypeItem::Choice
        DrawButton,         // TypeItem::Button
        DrawPage,           // TypeItem::Page
        DrawGovernor,       // TypeItem::Governor
        DrawTime,           // TypeItem::Time
        DrawIPaddress,      // TypeItem::IP
        EmptyFuncpVII,      // Item_SwitchButton
        DrawGovernorColor,  // TypeItem::GovernorColor
        DrawFormula,        // Item_Formula
        DrawMACaddress,     // Item_Mac
        DrawChoice,         // TypeItem::ChoiceReg
        DrawSmallButton     // TypeItem::SmallButton
    };

    int posFirstItem = PosItemOnTop();
    int posLastItem = posFirstItem + MENU_ITEMS_ON_DISPLAY - 1;
    LIMITATION(posLastItem, posLastItem, 0, NumItems() - 1);
    int count = 0;

    for(int posItem = posFirstItem; posItem <= posLastItem; posItem++)
    {
        Item *item = GetItem(posItem);
        int top = yTop + Item::HEIGHT * count;
        funcOfDraw[item->GetType()](item, MP_X, top);
        count++;

        ItemsUnderKey::Set(GetFuncButtonFromY(top), item);
    }
}


void Page::DrawOpened(int yTop)
{
    Item *item = Item::Opened();

    if (item->Keeper() == this)
    {
        for (int key = Key::F1; key <= Key::F5; key++)
        {
            if (ItemsUnderKey::Get((Key::E)key) != item)
            {
                ItemsUnderKey::Set((Key::E)key, nullptr);
            }
        }

        if (item->IsChoice() || item->IsChoiceReg())
        {
            ((Choice *)item)->DrawOpened(MP_X, Menu::ItemOpenedPosY(item)); //-V1027
        }
        else if (item->IsGovernor())
        {
            ((Governor *)item)->DrawOpened(MP_X, Menu::ItemOpenedPosY(item));
        }
        else if (item->IsGovernorColor())
        {
            ((GovernorColor *)item)->DrawOpened(MP_X, Menu::ItemOpenedPosY(item)); //-V1027
        }
        else if (item->IsTime())
        {
            ((Time *)item)->DrawOpened(MP_X, Menu::ItemOpenedPosY(item)); //-V1027
        }
        else if (item->IsIP())
        {
            ((IPaddress *)item)->DrawOpened(MP_X, Menu::ItemOpenedPosY(item));
        }
        else if (item->IsMAC())
        {
            ((MACaddress *)item)->DrawOpened(MP_X, Menu::ItemOpenedPosY(item)); //-V1027
        }
    }
    else
    {
        DrawTitle(yTop);
        DrawItems(yTop + MP_TITLE_HEIGHT);
    }

    if (funcOnDraw)
    {
        funcOnDraw();
    }
}


bool Item::IsShade() const
{
    return Keeper()->CurrentItemIsOpened() && (this != Item::Opened());
}


bool Item::IsPressed() const
{
    return (this == Item::now_pressed);
}


int Menu::ItemOpenedPosY(const Item *item)
{
    Page *page = item->Keeper();
    int8 posCurItem = page->PosCurrentItem();
    int y = GRID_TOP + (posCurItem % MENU_ITEMS_ON_DISPLAY) * Item::HEIGHT + MP_TITLE_HEIGHT;

    if(y + item->HeightOpened() > GRID_BOTTOM)
    {
        y = GRID_BOTTOM - item->HeightOpened() - 2;
    }

    return y + 1;
}
