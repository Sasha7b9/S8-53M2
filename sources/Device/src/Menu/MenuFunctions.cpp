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
    Item *RetLastOpened(Page *, TypeItem::E *);
}



TypeItem::E Item::GetType() const
{
    return (*((TypeItem::E *)this));
}


bool Page::CurrentItemIsOpened()
{
    bool retValue = _GET_BIT(POS_ACT_ITEM(name), 7) == 1;
    return retValue;
}


void Item::SetCurrent(bool active) const
{
    Page *page = Keeper();

    if (!active)
    {
        POS_ACT_ITEM(page->name) = 0x7f;
    }
    else
    {
        for (int8 i = 0; i < page->NumItems(); i++)
        {
            if (page->GetItem(i) == this)
            {
                POS_ACT_ITEM(page->name) = i;
                return;
            }
        }
    }
}


TypeItem::E Menu::TypeOpenedItem()
{
    return Item::Opened()->GetType();
}


Item* Item::Opened()
{
    TypeItem::E type = TypeItem::None;
    return (Item *)Menu::RetLastOpened((Page*)&mainPage, &type);
}


Item *Page::GetItem(int numElement) const
{
    const arrayItems &array = (*items);
    return (Item *)array[numElement + (IsSB() ? 1 : 0)];
}


Item *Item::Current()
{
    TypeItem::E type = TypeItem::None;
    Item *lastOpened = Menu::RetLastOpened((Page*)&mainPage, &type);
    int8 pos = ((const Page *)lastOpened)->PosCurrentItem();

    if(type == TypeItem::Page && pos != 0x7f)
    {
        return ((const Page *)lastOpened)->GetItem(pos);
    }

    return lastOpened;
}


int Item::HeightOpened() const
{
    TypeItem::E _type = GetType();

    if(_type == TypeItem::Page)
    {
        int numItems = ((const Page *)this)->NumItems() - ((Page *)this)->NumCurrentSubPage() * MENU_ITEMS_ON_DISPLAY;
        LIMITATION(numItems, numItems, 0, MENU_ITEMS_ON_DISPLAY);
        return MP_TITLE_HEIGHT + MI_HEIGHT * numItems;
    } 
    else if(_type == TypeItem::Choice || _type == TypeItem::ChoiceReg)
    {
        return MOI_HEIGHT_TITLE + ((Choice *)this)->NumSubItems() * MOSI_HEIGHT - 1;
    }

    return MI_HEIGHT;
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
    if (delta > 0 && NumCurrentSubPage() < NumSubPages() - 1)
    {
        Sound::RegulatorSwitchRotate();
        SetCurrentSubPage(NumCurrentSubPage() + 1);
    }
    else if (delta < 0 && NumCurrentSubPage() > 0)
    {
        Sound::RegulatorSwitchRotate();
        SetCurrentSubPage(NumCurrentSubPage() - 1);
    }
}


int Page::NumSubPages() const
{
    return (NumItems() - 1) / MENU_ITEMS_ON_DISPLAY + 1;
}


Item* Menu::RetLastOpened(Page *page, TypeItem::E *type)
{
    if(page->CurrentItemIsOpened())
    {
        int8 posActItem = page->PosCurrentItem();
        Item *item = page->GetItem(posActItem);
        TypeItem::E typeLocal = page->GetItem(posActItem)->GetType();

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
    Item *item = Item::Opened();

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
            Page *page = item->Keeper();
            POS_ACT_ITEM(page->name) &= 0x7f;                       // Сбрасываем бит 7 - "закрываем" активный пункт страницы namePage
        }

        NEED_CLOSE_PAGE_SB = 1;

        if(item == &mainPage)
        {
            Show(false);
        }
    }
    else
    {
        item->Open(false);
    } 
}


void Item::Open(bool open) const
{
    Page *page = Keeper();

    if (open)
    {
        POS_ACT_ITEM(page->name) |= 0x80;
    }
    else
    {
        POS_ACT_ITEM(page->name) &= 0x7f;
    }
}


bool Item::IsOpened() const
{
    Page *page = Keeper();

    if(GetType() == TypeItem::Page)
    {
        return Keeper()->CurrentItemIsOpened();
    }

    return (POS_ACT_ITEM(page->name) & 0x80) != 0;
}


Page* Item::Keeper() const
{
    const Page* page = ((Page*)(this))->keeper;
    return (Page *)page;
}


NamePage::E Page::GetName() const
{
    if(GetType() != TypeItem::Page)
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
    SetCurrent(true);
    Open(!IsOpened());
}


bool Item::IsActive() const
{
    TypeItem::E _type = GetType();

    if (_type == TypeItem::Choice || _type == TypeItem::Page || _type == TypeItem::Button || _type == TypeItem::Governor ||
        _type == TypeItem::SmallButton)
    {
        pFuncBV func = funcOfActive;

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


bool Item::ChangeOpened(int delta)
{
    if (delta < 2 && delta > -2)
    {
        return false;
    }

    TypeItem::E _type = GetType();

    if (_type == TypeItem::Page)
    {
        ((const Page *)this)->ChangeSubPage(delta);
    }
    else if (_type == TypeItem::IP)
    {
        ((IPaddress *)this)->ChangeValue(delta);
    }
    else if (_type == TypeItem::MAC)
    {
        ((MACaddress *)this)->ChangeValue(delta);
    }
    else if (_type == TypeItem::ChoiceReg || _type == TypeItem::Choice)
    {
        ((Choice *)this)->ChangeValue(delta);
    }
    else if (_type == TypeItem::Governor)
    {
        ((Governor *)this)->ChangeValue(delta);
    }
    
    return true;
}


void Item::Change(int delta)
{
    TypeItem::E _type = GetType();

    if (_type == TypeItem::Choice || _type == TypeItem::ChoiceReg)
    {
        ((Choice *)this)->StartChange(delta);
    }
    else if (_type == TypeItem::Governor)
    {
        Governor *governor = (Governor*)this;
        if (Item::Opened() != governor)
        {
            governor->StartChange(delta);
        }
        else
        {
            governor->ChangeValue(delta);
        }
    }
    else if (_type == TypeItem::GovernorColor)
    {
        ((GovernorColor *)this)->ChangeValue(delta);
    }
}


void Page::ShortPressOnItem(int numItem)
{
    if (GetType() != TypeItem::Page)
    {
        return;
    }

    if (name >= NamePage::SB_Curs)
    {
        SmallButton *sb = (SmallButton*)(*items)[numItem];
        if (sb && sb->funcOnPress)
        {
            sb->funcOnPress();
        }
    }
}


bool Page::IsSB() const
{
    return (name >= NamePage::SB_Curs);
}


SmallButton* Page::GetSmallButon(int numButton)
{
    return (SmallButton *)(*items)[numButton];
}
