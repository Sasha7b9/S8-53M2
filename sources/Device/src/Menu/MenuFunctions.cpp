// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/DisplayTypes.h"
#include "MenuItems.h"
#include "Panel/Controls.h"
#include "Menu/Menu.h"
#include "Settings/SettingsTypes.h"
#include "Data/Measures.h"
#include "Tables.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Panel/Panel.h"
#include "Log.h"
#include "Hardware/Sound.h"



extern const Page mainPage;


namespace Menu
{
    void *RetLastOpened(Page *, TypeItem::E *);
}



TypeItem::E Menu::TypeMenuItem(const void *address) 
{
    return address ? (*((TypeItem::E *)address)) : TypeItem::None;
}


bool Page::CurrentItemIsOpened()
{
    bool retValue = _GET_BIT(Menu::PosActItem(GetName()), 7) == 1;
    return retValue;
}


void Menu::SetCurrentItem(const void *item, bool active)
{
    if(item != 0)
    {
        Page *page = (Keeper(item));
        if(!active)
        {
            SetMenuPosActItem(page->name, 0x7f);
        }
        else
        {
            for(int8 i = 0; i < page->NumItems(); i++)
            {
                if(page->GetItem(i) == item)
                {
                    SetMenuPosActItem(page->name, i);
                    return;
                }
            }
        }
    }
}


TypeItem::E Menu::TypeOpenedItem()
{
    return TypeMenuItem(Item::Opened());
}


Item* Item::Opened()
{
    TypeItem::E type = TypeItem::None;
    return (Item *)Menu::RetLastOpened((Page*)&mainPage, &type);
}


void *Page::GetItem(int numElement) const
{
    const arrayItems &array = (*items);
    return array[numElement + (IsSB() ? 1 : 0)];
}


void* Menu::CurrentItem()
{
    TypeItem::E type = TypeItem::None;
    void *lastOpened = RetLastOpened((Page*)&mainPage, &type);
    int8 pos = ((const Page *)lastOpened)->PosCurrentItem();

    if(type == TypeItem::Page && pos != 0x7f)
    {
        return ((const Page *)lastOpened)->GetItem(pos);
    }

    return lastOpened;
}


int Menu::HeightOpenedItem(void *item) 
{
    TypeItem::E type = TypeMenuItem(item);

    if(type == TypeItem::Page)
    {
        int numItems = ((const Page *)item)->NumItems() - ((Page *)item)->NumCurrentSubPage() * MENU_ITEMS_ON_DISPLAY;
        LIMITATION(numItems, numItems, 0, MENU_ITEMS_ON_DISPLAY);
        return MP_TITLE_HEIGHT + MI_HEIGHT * numItems;
    } 
    else if(type == TypeItem::Choice || type == TypeItem::ChoiceReg)
    {
        return MOI_HEIGHT_TITLE + ((Choice *)item)->NumSubItems() * MOSI_HEIGHT - 1;
    }

    return MI_HEIGHT;
}


int Page::NumCurrentSubPage()
{
    return MenuCurrentSubPage(name);
}


pchar Item::Title() 
{
    return TITLE((Page*)this);
}


int Page::PosItemOnTop()
{
    return NumCurrentSubPage() * MENU_ITEMS_ON_DISPLAY;
}


bool Menu::IsFunctionalButton(Key::E button)
{
    return button >= Key::F1 && button <= Key::F5;
}


void Page::ChangeSubPage(int delta) const
{
    if (delta > 0 && MenuCurrentSubPage(name) < NumSubPages() - 1)
    {
        Sound::RegulatorSwitchRotate();
        SetMenuCurrentSubPage(name, MenuCurrentSubPage(name) + 1);
    }
    else if (delta < 0 && MenuCurrentSubPage(name) > 0)
    {
        Sound::RegulatorSwitchRotate();
        SetMenuCurrentSubPage(name, MenuCurrentSubPage(name) - 1);
    }
}


int Page::NumSubPages() const
{
    return (NumItems() - 1) / MENU_ITEMS_ON_DISPLAY + 1;
}


void* Menu::RetLastOpened(Page *page, TypeItem::E *type)
{
    if(page->CurrentItemIsOpened())
    {
        int8 posActItem = page->PosCurrentItem();
        void *item = page->GetItem(posActItem);
        TypeItem::E typeLocal = TypeMenuItem(page->GetItem(posActItem));

        if(typeLocal == TypeItem::Page)
        {
            return RetLastOpened((Page *)item, type);
        }
        else
        {
            return item;
        }
    }

    *type = TypeItem::Page;
    return page;
}


void Menu::CloseOpenedItem()
{
    void *item = Item::Opened();

    if(TypeOpenedItem() == TypeItem::Page)
    {
        if (((const Page *)item)->IsSB())                           // Для страницы малых кнопок
        {
            SmallButton *sb = ((Page *)item)->GetSmallButon(0);     // Выполняем функцию нажатия кнопки Key::Menu
            if (sb->funcOnPress)                                    // Если она есть
            {
                sb->funcOnPress();
            }
        }

        if(NEED_CLOSE_PAGE_SB == 1)
        {
            NamePage::E namePage = Keeper(item)->name;
            SetMenuPosActItem(namePage, PosActItem(namePage) & 0x7f);   // Сбрасываем бит 7 - "закрываем" активный пункт страницы namePage
        }

        NEED_CLOSE_PAGE_SB = 1;

        if(item == &mainPage)
        {
            ShowMenu(false);
        }
    }
    else
    {
        OpenItem(item, false);
    } 
}


void Menu::OpenItem(const void *item, bool open)
{
    if(item)
    {
        Page *page = Keeper(item);
        SetMenuPosActItem(page->GetName(), open ? (page->PosCurrentItem() | 0x80) : (page->PosCurrentItem() & 0x7f));
    }
}


bool Menu::ItemIsOpened(const void *item)
{
    TypeItem::E type = TypeMenuItem(item);
    Page *page = Keeper(item);

    if(type == TypeItem::Page)
    {
        return Keeper(item)->CurrentItemIsOpened();
    }

    return (PosActItem(page->name) & 0x80) != 0;
}


Page* Menu::Keeper(const void *item)
{
    const Page* page = ((Page*)(item))->keeper;
    return (Page *)page;
}


NamePage::E Page::GetName() const
{
    if(Menu::TypeMenuItem((void*)this) != TypeItem::Page)
    {
        return NamePage::NoPage;
    }

    return name;
}


NamePage::E Menu::GetNameOpenedPage()
{
    return ((const Page *)Item::Opened())->GetName();
}


void Page::OpenAndSetCurrent() const
{
    Menu::SetCurrentItem(this, true);
    Menu::OpenItem(this, !Menu::ItemIsOpened(this));
}


bool Menu::ItemIsActive(void *item)
{
    TypeItem::E type = TypeMenuItem(item);

    if (type == TypeItem::Choice || type == TypeItem::Page || type == TypeItem::Button || type == TypeItem::Governor ||
        type == TypeItem::SmallButton)
    {
        pFuncBV func = ((Page*)(item))->funcOfActive;

        return func ? func() : true;
    }

    return true;
}


int Page::NumItems() const
{
    if (name == NamePage::MainPage)
    {
        return (SHOW_DEBUG_MENU == 0) ? 10 : 11;
    }
    else if (IsSB())
    {
        return 5;
    }
    else
    {
        for (int i = 0; i < MAX_NUM_ITEMS_IN_PAGE; i++)
        {
            if (GetItem(i) == 0)
            {
                return i;
            }
        }
    }
    return 0;
}


bool Menu::ChangeOpenedItem(void *item, int delta)
{
    if (delta < 2 && delta > -2)
    {
        return false;
    }

    TypeItem::E type = TypeMenuItem(item);

    if (type == TypeItem::Page)
    {
        ((const Page *)item)->ChangeSubPage(delta);
    }
    else if (type == TypeItem::IP)
    {
        ((IPaddress *)item)->ChangeValue(delta);
    }
    else if (type == TypeItem::MAC)
    {
        ((MACaddress *)item)->ChangeValue(delta);
    }
    else if (type == TypeItem::ChoiceReg || type == TypeItem::Choice)
    {
        ((Choice *)item)->ChangeValue(delta);
    }
    else if (type == TypeItem::Governor)
    {
        ((Governor *)item)->ChangeValue(delta);
    }
    
    return true;
}


void Menu::ChangeItem(void *item, int delta)
{
    TypeItem::E type = TypeMenuItem(item);

    if (type == TypeItem::Choice || type == TypeItem::ChoiceReg)
    {
        ((Choice *)item)->StartChange(delta);
    }
    else if (type == TypeItem::Governor)
    {
        Governor *governor = (Governor*)item;
        if (Item::Opened() != governor)
        {
            governor->StartChange(delta);
        }
        else
        {
            governor->ChangeValue(delta);
        }
    }
    else if (type == TypeItem::GovernorColor)
    {
        ((GovernorColor *)item)->ChangeValue(delta);
    }
}


void Menu::ShortPressOnPageItem(Page *page, int numItem)
{
    if (TypeMenuItem(page) != TypeItem::Page)
    {
        return;
    }
    NamePage::E namePage = page->name;
    if (namePage >= NamePage::SB_Curs)
    {
        SmallButton *sb = (SmallButton*)(*page->items)[numItem];
        if (sb && sb->funcOnPress)
        {
            sb->funcOnPress();
        }
    }
}


Page* Menu::PagePointerFromName(NamePage::E)
{
    return 0;
}


bool Page::IsSB() const
{
    return (name >= NamePage::SB_Curs);
}


SmallButton* Page::GetSmallButon(int numButton)
{
    return (SmallButton *)(*items)[numButton];
}
