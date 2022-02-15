// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Measures.h"
#include "Tables.h"
#include "Panel/Panel.h"
#include "MenuItemsGraphics.h"
#include "Menu.h"
#include "MenuItemsLogic.h"
#include "Display/Display.h"
#include "Display/Colors.h"
#include "Display/font/Font.h"
#include "Display/Painter.h"
#include "Display/Grid.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/Measures.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include <stdio.h>


static void *itemUnderButton[B_NumButtons] = {0};



namespace Menu
{
    void ResetItemsUnderButton();

    int CalculateX(int layer);

    void DrawTitlePage(Page *, int layer, int yTop);

    void DrawPagesUGO(Page *, int right, int bottom);

    void DrawItemsPage(Page *, int layer, int yTop);

    int ItemOpenedPosY(void *item);
}



PanelButton GetFuncButtonFromY(int _y)
{
    int y = GRID_TOP + GRID_HEIGHT / 12;
    int step = GRID_HEIGHT / 6;
    PanelButton button = B_Menu;
    for(int i = 0; i < 6; i++)
    {
        if(_y < y)
        {
            return button;
        }
        ++button;
        y += step;
    }
    return  B_F5;
}


static void DrawHintItem(int x, int y, int width)
{
    if (!gItemHint)
    {
        return;
    }

    pchar names[Item_NumberItems][2] =
    {
        {"",            ""},        // Item_None
        {"",            ""},        // Item_Choice
        {"Кнопка",      "Button"},  // Item_Button
        {"Страница",    "Page"},    // Item_Page
        {"",            ""},        // Item_Governor
        {"",            ""},        // Item_Time
        {"",            ""},        // Item_IP
        {"",            ""},        // Item_SwitchButton
        {"",            ""},        // Item_GovernorColor
        {"",            ""},        // Item_Formula
        {"",            ""},        // Item_MAC
        {"",            ""},        // Item_ChoiceReg
        {"Кнопка",      "Button"}   // Item_SmallButton
    };
    TypeItem type = Menu::TypeMenuItem(gItemHint);
    Language lang = set.common.lang;
    Page *item = (Page*)gItemHint;

    const int SIZE = 100;
    char title[SIZE];
    snprintf(title, SIZE, "%s \"%s\"", names[type][lang], TITLE(item));

    if (item->type == Item_SmallButton)
    {
        y -= 9;
    }

    PText::DrawStringInCenterRectAndBoundIt(x, y, width, 15, title, COLOR_BACK, COLOR_FILL);
    y = PText::DrawTextInBoundedRectWithTransfers(x, y + 15, width, HINT(item), COLOR_BACK, COLOR_FILL);

    if (item->type == Item_SmallButton)
    {
        Painter::DrawHintsForSmallButton(x, y, width, (void *)item);
    }
}


void Menu::Draw()
{
    if(MenuIsShown() || TypeMenuItem(OpenedItem()) != Item_Page)
    {
        ResetItemsUnderButton();
        void *item = OpenedItem();
        if(MenuIsShown())
        {
            if (TypeMenuItem(item) == Item_Page)
            {
                ((Page *)item)->DrawOpened(0, GRID_TOP);
            }
            else
            {
                Keeper(item)->DrawOpened(0, GRID_TOP);
            }
        }
        else
        {
            if(TypeMenuItem(item) == Item_Choice)
            {
                ItemChoice_Draw((Choice *)item, CalculateX(0), GRID_TOP, true);
            }
            else if(TypeMenuItem(item) == Item_Governor)
            {
                ((Governor *)item)->Draw(CalculateX(0), GRID_TOP, true);
            }
        }
    }

    if (SHOW_HELP_HINTS)
    {
        int x = 0;
        int y = 0;
        int width = MenuIsMinimize() ? 289 : 220;
        PText::DrawTextInBoundedRectWithTransfers(x + 1, y, width - 1,
            set.common.lang == Russian ?    "Включён режим подсказок. В этом режиме при нажатии на кнопку на экран выводится информация о её назначении. "
                                                "Чтобы выключить этот режим, нажмите кнопку ПОМОЩЬ и удерживайте её в течение 0.5с." : 
                                                "Mode is activated hints. In this mode, pressing the button displays the information on its purpose. "
                                                "To disable this mode, press the button HELP and hold it for 0.5s.",
                                                COLOR_BACK, COLOR_FILL);
        y += set.common.lang == Russian ? 49 : 40;
        if (gStringForHint)
        {
            PText::DrawTextInBoundedRectWithTransfers(x + 1, y, width - 1, gStringForHint, COLOR_BACK, COLOR_FILL);
        }
        else if (gItemHint)
        {
            DrawHintItem(x + 1, y, width - 1);
        }
    }
}


void Menu::DrawTitlePage(Page *page, int layer, int yTop)
{
    int x = CalculateX(layer);
    if (PageIsSB(page))
    {
        ItemSmallButton_Draw(SmallButonFromPage(page, 0), LEFT_SB, yTop + 3);
        return;
    }
    int height = HeightOpenedItem(page);
    bool shade = CurrentItemIsOpened(GetNamePage(page));
    Painter::FillRegion(x - 1, yTop, MP_TITLE_WIDTH + 2, height + 2, COLOR_BACK);
    Painter::DrawRectangle(x, yTop, MP_TITLE_WIDTH + 1, height + 1, ColorBorderMenu(shade));

    if (shade)
    {
        Painter::FillRegion(x + 1, yTop + 1, MP_TITLE_WIDTH - 1, MP_TITLE_HEIGHT - 1, ColorMenuTitleLessBright());
        Painter::FillRegion(x + 4, yTop + 4, MP_TITLE_WIDTH - 7, MP_TITLE_HEIGHT - 7, Color::MENU_TITLE_DARK);
    }
    else
    {
        Painter::DrawVolumeButton(x + 1, yTop + 1, MP_TITLE_WIDTH - 1, MP_TITLE_HEIGHT - 1, 3, ColorMenuTitle(false), ColorMenuTitleBrighter(), ColorMenuTitleLessBright(), shade, false);
    }
    
    Painter::DrawVLine(x, yTop, yTop + HeightOpenedItem(page), ColorBorderMenu(false));
    bool condDrawRSet = NumSubPages(page) > 1 && TypeMenuItem(CurrentItem()) != Item_ChoiceReg && TypeMenuItem(CurrentItem()) != Item_Governor && TypeOpenedItem() == Item_Page;
    int delta = condDrawRSet ? -10 : 0;
    Color::E colorText = shade ? LightShadingTextColor() : Color::BLACK;
    x = PText::DrawStringInCenterRect(x, yTop, MP_TITLE_WIDTH + 2 + delta, MP_TITLE_HEIGHT, TitleItem(page), colorText);
    if(condDrawRSet)
    {
        PText::Draw4SymbolsInRect(x + 4, yTop + 11, GetSymbolForGovernor(NumCurrentSubPage(page)), colorText);
    }

    itemUnderButton[GetFuncButtonFromY(yTop)] = page;

    delta = 0;
    
    Color::SetCurrent(colorText);
    DrawPagesUGO(page, CalculateX(layer) + MP_TITLE_WIDTH - 3 + delta, yTop + MP_TITLE_HEIGHT - 2 + delta);
}


void Menu::DrawPagesUGO(Page *page, int right, int bottom)
{
    int size = 4;
    int delta = 2;
    
    int allPages = (NumItemsInPage(page)- 1) / MENU_ITEMS_ON_DISPLAY + 1;
    int currentPage = NumCurrentSubPage(page);

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
    ItemMACaddress_Draw((MACaddress *)item, x, y, false);
}


static void DrawFormula(void* item, int x, int y)
{
    ItemFormula_Draw((Formula *)item, x, y, false);
}


static void DrawChoice(void *item, int x, int y)
{
    ItemChoice_Draw((Choice *)item, x, y, false);
}


static void DrawSmallButton(void *item, int x, int y)
{
    ItemSmallButton_Draw((SmallButton *)item, LEFT_SB, y + 7);
}


static void DrawTime(void *item, int x, int y)
{
    ItemTime_Draw((Time *)item, x, y, false);
}


static void DrawGovernorColor(void *item, int x, int y)
{
    ((GovernorColor *)item)->Draw(x, y, false);
}


static void DrawButton(void *item, int x, int y)
{
    ItemButton_Draw((Button *)item, x, y);
}


static void DrawPage(void *item, int x, int y)
{
    ItemPage_Draw((Page *)item, x, y);
}


void Menu::DrawItemsPage(Page *page, int layer, int yTop)
{
    void (*funcOfDraw[Item_NumberItems])(void*, int, int) = 
    {  
        EmptyFuncpVII,      // Item_None
        DrawChoice,         // Item_Choice
        DrawButton,         // Item_Button
        DrawPage,           // Item_Page
        DrawGovernor,       // Item_Governor
        DrawTime,           // Item_Time
        DrawIPaddress,      // Item_IP
        EmptyFuncpVII,      // Item_SwitchButton
        DrawGovernorColor,  // Item_GovernorColor
        DrawFormula,        // Item_Formula
        DrawMACaddress,     // Item_Mac
        DrawChoice,         // Item_ChoiceReg
        DrawSmallButton     // Item_SmallButton
    };
    int posFirstItem = PosItemOnTop(page);
    int posLastItem = posFirstItem + MENU_ITEMS_ON_DISPLAY - 1;
    LIMITATION(posLastItem, posLastItem, 0, NumItemsInPage(page) - 1);
    int count = 0;

    for(int posItem = posFirstItem; posItem <= posLastItem; posItem++)
    {
        void *item = Item(page, posItem);
        TypeItem type = TypeMenuItem(item);
        int top = yTop + MI_HEIGHT * count;
        funcOfDraw[type](item, CalculateX(layer), top);
        count++;
        itemUnderButton[GetFuncButtonFromY(top)] = item;
    }
}


void Page::DrawOpened(int layer, int yTop)
{
    Menu::DrawTitlePage(this, layer, yTop);
    Menu::DrawItemsPage(this, layer, yTop + MP_TITLE_HEIGHT);

    if (Menu::CurrentItemIsOpened(Menu::GetNamePage(this)))
    {
        int8 posCurItem = Menu::PosCurrentItem(this);
        void *item = Menu::Item(this, posCurItem);

        for (int i = 0; i < 5; i++)
        {
            if (itemUnderButton[i + B_F1] != item)
            {
                itemUnderButton[i + B_F1] = 0;
            }
        }

        if (type == Item_Choice || type == Item_ChoiceReg)
        {
            ItemChoice_Draw((Choice *)item, Menu::CalculateX(1), Menu::ItemOpenedPosY(item), true);
        }
        else if (type == Item_Governor)
        {
            ((Governor *)item)->Draw(Menu::CalculateX(1), Menu::ItemOpenedPosY(item), true);
        }
        else if (type == Item_GovernorColor)
        {
            ((GovernorColor *)item)->Draw(Menu::CalculateX(1), Menu::ItemOpenedPosY(item), true);
        }
        else if (type == Item_Time)
        {
            ItemTime_Draw((Time *)item, Menu::CalculateX(1), Menu::ItemOpenedPosY(item), true);
        }
        else if (type == Item_IP)
        {
            ((IPaddress *)item)->Draw(Menu::CalculateX(1), Menu::ItemOpenedPosY(item), true);
        }
        else if (type == Item_MAC)
        {
            ItemMACaddress_Draw((MACaddress *)item, Menu::CalculateX(1), Menu::ItemOpenedPosY(item), true);
        }
    }

    if (funcOnDraw)
    {
        funcOnDraw();
    }
}


int Menu::CalculateX(int layer)
{
    return MP_X - layer * GRID_DELTA / 4;
}


bool Menu::IsShade(void* item)
{
    return CurrentItemIsOpened(GetNamePage(Keeper(item))) && (item != OpenedItem());
}


bool Menu::IsPressed(void* item)
{
    return item == Menu::ItemUnderKey();
}


void* Menu::ItemUnderButton(PanelButton button)
{
    return itemUnderButton[button];
}


void Menu::ResetItemsUnderButton()
{
    for(int i = 0; i < B_NumButtons; i++)
    {
        itemUnderButton[i] = 0;
    }
}


int Menu::ItemOpenedPosY(void *item)
{
    Page *page = Keeper(item);
    int8 posCurItem = PosCurrentItem(page);
    int y = GRID_TOP + (posCurItem % MENU_ITEMS_ON_DISPLAY) * MI_HEIGHT + MP_TITLE_HEIGHT;
    if(y + HeightOpenedItem(item) > GRID_BOTTOM)
    {
        y = GRID_BOTTOM - HeightOpenedItem(item) - 2;
    }
    return y + 1;
}
