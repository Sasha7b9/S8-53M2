// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Controls_Old.h"
#include "Menu/Menu.h" 
#include "Settings/Settings.h"
#include "Pages/Definition.h"
#include "Display/DisplayTypes.h"
#include "MenuItems.h"
#include "Tables.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "Data/Measures.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "FPGA/FPGA.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "FDrive/FDrive.h"
#include "Menu/FileManager.h"
#include <string.h>



namespace Menu
{
    bool showDebugMenu = true;
    bool needClosePageSB = true;

    // ���� ��������� �������� ������� ������, �� ����� �������� ��� ���� ������ �� ��������� �����  �������.
    KeyOld::E shortPressureButton = KeyOld::Empty;
    // ���� ��������� ������� ������� ������, �� ����� �������� ��� ���� ������ �� ��������� ����� �������.
    KeyOld::E longPressureButton = KeyOld::Empty;
    // ��� ������� ������ � ��� ������������ � ��� ���������� � �������� ��� �� �������� ������� ������� ������.
    KeyOld::E pressButton = KeyOld::Empty;
    // ��� ���������� ������ � ��� ������������ � ��� ���������� � �������� ��� �� ���������  ������� ���������� ������.
    KeyOld::E releaseButton = KeyOld::Empty;
    // ����, �� ������� ����� ��������� ����� ��������� - �������� �������� ���������� �������, ���� - ����������� - "-" - �����, "+" - ������
    int angleRegSet = 0;

    static const int stepAngleRegSet = 2;

    static const int  SIZE_BUFFER_FOR_BUTTONS = 5;
    static KeyOld::E bufferForButtons[SIZE_BUFFER_FOR_BUTTONS] = {KeyOld::Empty, KeyOld::Empty, KeyOld::Empty, KeyOld::Empty, KeyOld::Empty};
    static const KeyOld::E sampleBufferForButtons[SIZE_BUFFER_FOR_BUTTONS] = {KeyOld::F5, KeyOld::F4, KeyOld::F3, KeyOld::F2, KeyOld::F1};

    // ��������� ��������� ������� ������.
    void ProcessingShortPressureButton();
    // ��������� �������� ������� ������.
    void ProcessingLongPressureButton();
    // ��������� ��������� ������ ����.
    void ProcessingPressButton();
    // ��������� �������� ������ �����.
    void ProcessingReleaseButton();
    // ��������� �������� ����� ���������.
    void ProcessingRegulatorSet();
    // ��������/��������� ��������� ����� ���������, ���� ����������.
    void SwitchSetLED();
    // ��������� ��������� ������� �� ������� NamePage � ������� page.
    void ShortPress_Page(Item *page);
    // ��������� ��������� ������� �� ������� Choice � ������� choice.
    void ShortPress_Choice(Item *choice);

    void ShortPress_Time(Item *time);
    // ��������� ��������� ������� �� ������� Button � ������� button.
    void ShortPress_Button(Item *button);
    // ��������� ��������� ������� �� ������� Governor � ������� governor.
    void ShortPress_Governor(Item *governor);
    // ��������� ��������� ������� �� ������� GovernorColor � ������� governorColor.
    void ShortPress_GovernorColor(Item *governorColor);

    void ShortPress_IP(Item *item);

    void ShortPress_MAC(Item *item);

    void ShortPress_ChoiceReg(Item *choice);

    void ShortPress_SmallButton(Item *smallButton);
    // ��������� �������� ������� �� ������� ���� item.
    void FuncOnLongPressItem(Item *item);

    void FuncOnLongPressItemTime(Item *item);
    // ��������� �������� ������� �� ������� Button � ������� button.
    void FuncOnLongPressItemButton(Item *button);
    // ���������� ������� ��������� ��������� ������� �� ������� ���� item.
    void ExecuteFuncForShortPressOnItem(Item *item);
    // ���������� ������� ��������� �������� ������� �� ������� ���� item.
    void ExecuteFuncForLongPressureOnItem(Item *item);

    // ���������� true, ���� �������� ��������� ������ ������
    bool NeedForFireSetLED();

    void OpenFileManager();
    // ��������� ������� ������� ��������������� �������� ����.
    void OnTimerAutoHide();

    void ProcessButtonForHint(KeyOld::E button);
}


void Menu::UpdateInput()
{
    ProcessingShortPressureButton();
    ProcessingLongPressureButton();
    ProcessingRegulatorSet();
    ProcessingPressButton();
    ProcessingReleaseButton();
    SwitchSetLED();

    if (FM::needOpen)
    {
        FM::needOpen = false;
        OpenFileManager();
    }
};


void Menu::Handlers::ShortPressureButton(KeyOld::E button)
{
    if (!Hint::show)
    {
        if(button == KeyOld::Help)
        {
            Hint::show = !Hint::show;
            Hint::string = nullptr;
            Hint::item = nullptr;
        }
        shortPressureButton = button;
    }
};



void Menu::Handlers::LongPressureButton(KeyOld::E button)
{
    if (!Hint::show)
    {
        longPressureButton = button;
        Display::Redraw();
    }
};


void Menu::ProcessButtonForHint(KeyOld::E button)
{
    if (button == KeyOld::Menu)
    {
        Hint::string = LANG_RU ?
            "������ ���� ��������� ��������� �������:\n"
            "1. ��� �������� ���� ������� ���� ������� � ���������� � ������� 0.5� ��������� ����.\n"
            "2. ��� �������� ���� ��������� ������ � ������� 0.5� ��������� ����.\n"
            "3. ��� ��������� \"������\x99����� �� ����\x99���������\" ������� ���������� �������� ����������� ������ ����. ���� ������� �������� �������� ��������, ���� �����������.\n"
            "4. ��� ��������� \"������\x99����� �� ����\x99�����������\" ������� ���������� �������� �������� ������ ����. ���� ������� �������� ��������� � ������� ������, ���������� ������� �� ���������� ������� ����.\n"
            "5. ���� ���� ��������� � ������ ����� ������, �� ������� ��������� ��������."
            :
        "MENU button performs the following functions:\n"
            "1. At the closed menu pressing or pressing with deduction during 0.5s opens the menu.\n"
            "2. At the open menu deduction of the button during 0.5s closes the menu.\n"
#ifndef WIN32
#pragma push
#pragma diag_suppress 192
#endif
            "3. At control \"SERVICE\x99Mode btn MENU\x99\x43lose\" current becomes the page of the previous level of the menu. If the root page is current, the menu is closed.\n"
#ifndef WIN32
#pragma pop
#endif
            "4. At control \"SERVICE\x99Mode btn MENU\x99Toggle\" current becomes the page of the current level of the menu. If the current page the last in the current level, happens transition to the previous level of the menu.\n"
            "5. If the menu is in the mode of small buttons, pressing closes the page.";

    } else if (button == KeyOld::Cursors)
    {
        Hint::string = LANG_RU ?
            "������ ������� ��������� ���� ��������� ���������."
            :
            "������� button to open the menu cursor measurements.";
    }
    else if (button == KeyOld::Display)
    {
        Hint::string = LANG_RU ?
            "������ ������� ��������� ���� �������� �������."
            :
            "DISPLAY button opens the display settings menu.";
    }
    else if (button == KeyOld::Memory)
    {
        Hint::string = LANG_RU ?
            "1. ��� ��������� \"������\x99����� ��\x99��� �� ������\x99����\" ��������� ���� ������ � �������.\n"
            "2. ��� ��������� \"������\x99����� ��\x99��� �� ������\x99����������\" ��������� ������ �� ����-����."
            :
            "1. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Menu\" opens a menu of memory\n"
            "2. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Save\" saves the signal to the flash drive";
    }
    else if (button == KeyOld::Measures)
    {
        Hint::string = LANG_RU ?
            "������ ����� ��������� ���� �������������� ���������."
            :
            "����� button opens a menu of automatic measurements.";
    }
    else if (button == KeyOld::Help)
    {
        Hint::string = LANG_RU ?
            "1. ������ ������ ��������� ���� ������.\n"
            "2. ������� � ��������� ������ ������ � ������� 0.5� �������� � ��������� ����� ������ ���������."
            :
            "1. ������ button opens the help menu.\n"
            "2. Pressing and holding the ������ for 0.5s enables and disables output mode hints.";
    }
    else if (button == KeyOld::Service)
    {
        Hint::string = LANG_RU ?
            "������ ������ ��������� ���� ��������� ������������."
            :
            "������ button opens a menu of service options.";
    }
    else if (button == KeyOld::Start)
    {
        Hint::string = LANG_RU ?
            "������ ����/�TO� ��������� � ������������� ������� ����� ����������."
            :
            "����/�TO� button starts and stops the process of gathering information.";
    }
    else if (button == KeyOld::ChannelA)
    {
        Hint::string = LANG_RU ?
            "1. ������ �����1 ��������� ���� �������� ������ 1.\n"
            "2. ������� � ��������� ������ �����1 � ������� 0.5� ������������� �������� ������ 1 �� ��������� 0�."
            :
            "1. �����1 button opens the settings menu of the channel 1.\n"
            "2. Pressing and holding the button �����1 for 0.5c for the offset of the vertical channel 1 0V.";
    }
    else if (button == KeyOld::ChannelB)
    {
        Hint::string = LANG_RU ?
            "1. ������ �����2 ��������� ���� �������� ������ 2.\n"
            "2. ������� � ��������� ������ �����2 � ������� 0.5� ������������� �������� ������ 2 �� ��������� 0�."
            :
            "1. �����2 button opens the settings menu of the channel 2.\n"
            "2. Pressing and holding the button �����2 for 0.5c for the offset of the vertical channel 2 0V.";
    }
    else if (button == KeyOld::Time)
    {
        Hint::string = LANG_RU ?
            "1. ������ ���� ��������� ���� �������� ���������.\n"
            "2. ������� � ��������� ������ ���� � ������� 0.5� ������������� �������� �� ����������� 0�."
            :
            "1. ���� button open the settings menu sweep.\n"
            "2. Pressing and holding the button ���� for 0.5s Is the offset horizontal 0s.";
    }
    else if (button == KeyOld::Trig)
    {
    Hint::string = LANG_RU ?
            "1. ������ ����� ��������� ���� �������� �������������.\n"
            "2. ������� � ��������� � ������� 0.5� ������ ����� ��� ��������� \"������\x99��� ���� �����\x99�����������\" ���������� �������������� ��������� ������ �������������.\n"
            "3. ������� � ��������� � ������� 0.5� ������ ����� ��� ��������� \"������\x99��� ���� �����\x99����� ������\" ������������� ������� ������������� 0�."
            :
            "1. ����� button opens a menu settings synchronization.\n"
#ifndef WIN32
#pragma push
#pragma diag_suppress 192
#endif
            "2. Pressing and holding the button ����� for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99\x41utolevel\" automatically adjust the trigger level.\n"
#ifndef WIN32
#pragma pop
#endif
            "3. Pressing and holding the button ����� for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99SReset trig level\" sets the trigger level 0V.";
    }
    else
    {
        shortPressureButton = button;
    }
}


void Menu::Handlers::PressButton(KeyOld::E button)
{
    Sound::ButtonPress();

    if (Hint::show)
    {
        ProcessButtonForHint(button);
        return;
    }

    if (pressButton == KeyOld::Start)
    {
    } 
    else if (!Menu::IsShown())
    {
        for (int i = SIZE_BUFFER_FOR_BUTTONS - 1; i > 0; i--)
        {
            bufferForButtons[i] = bufferForButtons[i - 1];
        }
        bufferForButtons[0] = button;
      
        if (memcmp(bufferForButtons, sampleBufferForButtons, SIZE_BUFFER_FOR_BUTTONS * sizeof(KeyOld::E)) == 0)
        {
            showDebugMenu = true;
            Display::ShowWarningGood(Warning::MenuDebugEnabled);
        }
    }

    pressButton = button;
};


void Menu::Handlers::ReleaseButton(KeyOld::E button)
{
    Sound::ButtonRelease();

    if (!Hint::show)
    {
        releaseButton = button;
    }
};


void Menu::Handlers::RotateRegSetRight()
{   
    if (!Hint::show)
    {
        angleRegSet++;
        Display::Redraw();
    }
};


void Menu::Handlers::RotateRegSetLeft()
{
    if (!Hint::show)
    {
        angleRegSet--;
        Display::Redraw();
    }
};


Item *Item::UnderKey()
{
    return (Item *)underKey;
};


void Menu::SetAutoHide(bool)
{
    if(!IsShown())
    {
        return;
    }
    if(SettingsDisplay::TimeMenuAutoHide() == 0)
    {
        Timer::Disable(TypeTimer::MenuAutoHide);
    }
    else
    {
        Timer::Enable(TypeTimer::MenuAutoHide, SettingsDisplay::TimeMenuAutoHide(), OnTimerAutoHide);
    }
}


char* Menu::StringNavigation(char buffer[100])
{
    if(SHOW_STRING_NAVIGATION && IsShown())
    {
        buffer[0] = 0;
        pchar titles[10] = {0};
        int numTitle = 0;
        Item *item = Item::Opened();

        if(IsMainPage(item))
        {
            return 0;
        }

        while(!IsMainPage(item))
        {
            titles[numTitle++] = item->Title();
            item = item->Keeper();
        }

        for(int i = 9; i >= 0; i--)
        {
            if(titles[i])
            {
                strcat(buffer, titles[i]);
                if(i != 0)
                {
                    strcat(buffer, " - ");
                }
            }
        }

        return buffer;
    }
    return 0;
}


void Menu::OnTimerAutoHide()
{
    Show(false);
    Timer::Disable(TypeTimer::MenuAutoHide);
}


void Menu::ProcessingShortPressureButton()
{
    if(shortPressureButton != KeyOld::Empty)
    {
        if (shortPressureButton == KeyOld::Memory && MODE_BTN_MEMORY_IS_SAVE && FDrive::isConnected)
        {
            PageMemory::SetName::exitTo = IsShown() ? RETURN_TO_MAIN_MENU : RETURN_TO_DISABLE_MENU;
            PageMemory::SaveSignalToFlashDrive();
            shortPressureButton = KeyOld::Empty;
            return;
        }
        Display::Redraw();
        Menu::SetAutoHide(true);

        KeyOld::E button = shortPressureButton;

        do
        {
            if(button == KeyOld::Menu)                                   // ���� ������ ������ ���� � �� �� ��������� � ����� ��������� ���������.
            {
                if(!IsShown())
                {
                    Show(true);
                }
                else
                {
                    Item::CloseOpened();
                }
            }
            else if (IsShown() && KeyOld::IsFunctional(button))       // ���� ���� �������� � ������ �������������� �������
            {
                Item *item = Item::UnderKey(button);

                if (Hint::show)
                {
                    Hint::SetItem(item);
                }
                else
                {
                    ExecuteFuncForShortPressOnItem(item);
                }
            }
            else                                                        // ���� ���� �� ��������.
            {
                NamePage::E name = ((const Page *)Item::Opened())->GetName();
                if(button == KeyOld::ChannelA && name == NamePage::Channel0)
                {
                    SET_ENABLED_A = !Chan::Enabled(Chan::A);
                    PageChannelA::OnChanged_Input(true);
                    break;
                }
                if(button == KeyOld::ChannelB && name == NamePage::Channel1)
                {
                    SET_ENABLED_B = !Chan::Enabled(Chan::B);
                    PageChannelB::OnChanged_Input(true);
                    break;
                }

                const Page *page = Page::ForButton(button);

                if(page)
                {
                    page->SetCurrent(true);
                    page->Open(true);
                    Show(true);
                }
            }
        } while(false);

        shortPressureButton = KeyOld::Empty;
    }
}


void Menu::ProcessingLongPressureButton()
{
    if(longPressureButton != KeyOld::Empty)
    {
        Display::Redraw();
        Menu::SetAutoHide(true);

        if(longPressureButton == KeyOld::Time)
        {
            TShift::Set(0);
        }
        else if(longPressureButton == KeyOld::Trig)
        {
            TrigLev::Set(TRIG_SOURCE, TrigLev::ZERO);
        }
        else if(longPressureButton == KeyOld::ChannelA)
        {
            RShift::Set(Chan::A, RShift::ZERO);
        }
        else if(longPressureButton == KeyOld::ChannelB)
        {
            RShift::Set(Chan::B, RShift::ZERO);
        }
        else if(longPressureButton == KeyOld::Menu)
        {
                Show(!IsShown());
        }
        else if(IsShown() && KeyOld::IsFunctional(longPressureButton))
        {
            Item *item = Item::UnderKey(longPressureButton);
            ExecuteFuncForLongPressureOnItem(item);
        }
        longPressureButton = KeyOld::Empty;
    }
}


void Menu::ProcessingRegulatorSet()
{
    if (angleRegSet == 0)
    {
        return;
    }

    if (IsShown() || Item::Opened()->GetType() != TypeItem::Page)
    {
        Item *item = Item::Current();
        TypeItem::E type = item->GetType();

        if (Item::Opened()->GetType() == TypeItem::Page && (type == TypeItem::ChoiceReg ||
            type == TypeItem::Governor || type == TypeItem::IP || type == TypeItem::MAC))
        {
            if (angleRegSet > stepAngleRegSet || angleRegSet < -stepAngleRegSet)
            {
                item->Change(angleRegSet);
                angleRegSet = 0;
            }

            return;
        }
        else
        {
            item = Item::Opened();
            type = item->GetType();
            if (IsMinimize())
            {
                Page::RotateRegSetSB(angleRegSet);
            }
            else if (type == TypeItem::Page || type == TypeItem::IP || type == TypeItem::MAC || type == TypeItem::Choice || type == TypeItem::ChoiceReg || type == TypeItem::Governor)
            {
                if (item->ChangeOpened(angleRegSet))
                {
                    angleRegSet = 0;
                }
                return;
            }
            else if (type == TypeItem::GovernorColor)
            {
                item->Change(angleRegSet);
            }
            else if (type == TypeItem::Time)
            {
                angleRegSet > 0 ? ((Time *)item)->IncCurrentPosition() : ((Time *)item)->DecCurrentPosition();
            }
        }
    }

    angleRegSet = 0;
}


void Menu::ProcessingPressButton()
{
    if (pressButton == KeyOld::Start && !MODE_WORK_IS_LATEST)
    {
        FPGA::OnPressStartStop();
    } 
    else if((pressButton >= KeyOld::F1 && pressButton <= KeyOld::F5) || pressButton == KeyOld::Menu)
    {
        if (pressButton != KeyOld::Menu)
        {
            Item::underKey = Item::UnderKey(pressButton);
        }
    }
    pressButton = KeyOld::Empty;
}


void Menu::ProcessingReleaseButton()
{
    if(releaseButton >= KeyOld::F1 && releaseButton <= KeyOld::F5 || pressButton == KeyOld::Menu)
    {
        Item::underKey = nullptr;
        releaseButton = KeyOld::Empty;
    }
}


void Menu::ShortPress_Page(Item *item)
{
    Page *page = (Page *)item;

    if (page->funcOnPress)
    {
        page->funcOnPress();
    }

    if (!page->IsActive())
    {
        return;
    }

    page->SetCurrent(true);

    ((Page*)page)->Open(!page->IsOpened());
}


void Menu::ShortPress_Choice(Item *choice)
{
    if (!choice->IsActive())
    {
        ((Choice *)choice)->FuncOnChanged(false); //-V1027
    }
    else if (!choice->IsOpened())
    {
        choice->SetCurrent(Item::Current() != choice);
        ((Choice *)choice)->StartChange(1); //-V1027
    }
    else
    {
        ((Choice *)choice)->ChangeValue(-1); //-V1027
    }
}


void Menu::ShortPress_ChoiceReg(Item *choice)
{
    if(!choice->IsActive()) 
    {
        ((Choice *)choice)->FuncOnChanged(false); //-V1027
    } 
    else if(Item::Opened() != choice)
    {
        choice->SetCurrent(Item::Current() != choice);
    }
}


void Menu::FuncOnLongPressItemButton(Item *button)
{
    ShortPress_Button(button);
}


void Menu::ShortPress_Button(Item *button)
{
    if(!button->IsActive())
    {
        return;
    }
    
    button->SetCurrent(true);
    ((Button*)button)->funcOnPress();
}


void Menu::FuncOnLongPressItem(Item *item)
{
    if (Item::Current() != item)
    {
        item->SetCurrent(true);
    }

    item->Open(!item->IsOpened());
}


void Menu::FuncOnLongPressItemTime(Item *time)
{
    if (Item::Current() != time)
    {
        time->SetCurrent(true);
    }

    if(time->IsOpened() && *((Time*)time)->curField == iSET)
    {
        ((Time *)time)->SetNewTime();
    }

    time->Open(!time->IsOpened());

    ((Time *)time)->SetOpened();
}


void Menu::ShortPress_Time(Item *time)
{
    if(!time->IsOpened())
    {
        time->SetCurrent(true);
        ((Time *)time)->SetOpened();
        time->Open(true);
    }
    else
    {
        ((Time *)time)->SelectNextPosition();
    }
}


void Menu::ShortPress_Governor(Item *governor)
{
    Governor *gov = (Governor*)governor;

    if(!governor->IsActive())
    {
        return;
    }

    if(Item::Opened() == gov)
    {
        gov->NextPosition();
    }
    else
    {
        gov->SetCurrent(Item::Current() != gov);
    }
}


void Menu::ShortPress_IP(Item *item)
{
    if (Item::Opened() == item)
    {
        ((IPaddress*)item)->NextPosition();
    }
}


void Menu::ShortPress_MAC(Item *item)
{
    if (Item::Opened() == item)
    {
        CircleIncreaseInt8(&MACaddress::cur_digit, 0, 5);
    }
}


void Menu::ShortPress_GovernorColor(Item *governorColor)
{
    if(!governorColor->IsActive())
    {
        return;
    }

    GovernorColor *governor = (GovernorColor*)governorColor; //-V1027

    if(Item::Opened() == (Item *)governor) //-V1027
    {
        CircleIncreaseInt8(&(governor->colorType->currentField), 0, 3);
    }
    else
    {
        FuncOnLongPressItem((Item *)governorColor);
    }
}


void Menu::ShortPress_SmallButton(Item *smallButton)
{
    SmallButton *sb = (SmallButton *)smallButton;

    if (sb)
    {
        pFuncVV func = sb->funcOnPress;

        if (func)
        {
            func();
            Item::underKey = (Item *)smallButton;
        }
    }
}


void Menu::ExecuteFuncForShortPressOnItem(Item *item)
{
    typedef void(*pFuncMenuVpV)(Item *);

    static const pFuncMenuVpV shortFunction[TypeItem::Count] =
    {
        nullptr,                            // TypeItem::None
        &Menu::ShortPress_Choice,           // TypeItem::Choice
        &Menu::ShortPress_Button,           // TypeItem::Button
        &Menu::ShortPress_Page,             // TypeItem::Page
        &Menu::ShortPress_Governor,         // TypeItem::Governor
        &Menu::ShortPress_Time,             // TypeItem::Time
        &Menu::ShortPress_IP,               // TypeItem::IP
        nullptr,                            // Item_SwitchButton
        &Menu::ShortPress_GovernorColor,    // TypeItem::GovernorColor
        nullptr,                            // Item_Formula
        &Menu::ShortPress_MAC,              // TypeItem::MAC
        &Menu::ShortPress_ChoiceReg,        // TypeItem::ChoiceReg
        &Menu::ShortPress_SmallButton       // TypeItem::SmallButton
    };
 
    pFuncMenuVpV func = shortFunction[item->GetType()];

    if (func)
    {
        (func)(item);
    }
}


void Menu::ExecuteFuncForLongPressureOnItem(Item *item)
{
    typedef void(*pFuncMenuVpV)(Item *);

    static const pFuncMenuVpV longFunction[TypeItem::Count] =
    {
        0,                                  // TypeItem::None
        &Menu::FuncOnLongPressItem,         // TypeItem::Choice
        &Menu::FuncOnLongPressItemButton,   // TypeItem::Button
        &Menu::FuncOnLongPressItem,         // TypeItem::Page
        &Menu::FuncOnLongPressItem,         // TypeItem::Governor
        &Menu::FuncOnLongPressItemTime,     // TypeItem::Time
        &Menu::FuncOnLongPressItem,         // TypeItem::IP
        0,                                  // Item_SwitchButton
        &Menu::FuncOnLongPressItem,         // TypeItem::GovernorColor
        0,                                  // Item_Formula
        &Menu::FuncOnLongPressItem,         // TypeItem::MAC
        &Menu::FuncOnLongPressItem,         // TypeItem::ChoiceReg
        &Menu::ShortPress_SmallButton       // TypeItem::SmallButton
    };

    if (item->IsActive())
    {
        pFuncMenuVpV func = longFunction[item->GetType()];
        if (func)
        {
            (func)(item);
        }
    }
}


void Menu::ChangeStateFlashDrive()
{
    if(!FDrive::isConnected)
    {
        if(Page::NameOpened() == NamePage::SB_FileManager)
        {
            ((Page *)Item::Opened())->ShortPressOnItem(0);
        }
    }
    else if(FLASH_AUTOCONNECT)
    {
        FM::needOpen = true;
    }
}


extern const Page pService;


void Time::Open()
{
    Display::ShowWarningGood(Warning::TimeNotSet);
    Menu::Handlers::ShortPressureButton(KeyOld::Service);
    Menu::UpdateInput();
    Display::Update();

    for (int i = 0; i < 2; i++)
    {
        Menu::Handlers::RotateRegSetRight();
        Menu::UpdateInput();
        Display::Update();
    }

    Menu::Handlers::ShortPressureButton(KeyOld::F4);
    Menu::UpdateInput();
    Display::Update();
}


bool Menu::NeedForFireSetLED()
{
    if (!IsShown())
    {
        return false;
    }
    NamePage::E name = Page::NameOpened();

    if (name == NamePage::SB_MeasTuneMeas && MEAS_NUM == MN_1 && !PageMeasures::choiceMeasuresIsActive)
    {
        return false;
    }

    if (
        name == NamePage::SB_MathCursorsFFT                             ||
        name == NamePage::SB_MeasTuneMeas                               ||
        name == NamePage::SB_MemLatest                                  || 
        name == NamePage::SB_MemInt                                     ||
        ((name == NamePage::SB_MathFunction) && !DISABLED_DRAW_MATH)    ||
        ((name == NamePage::SB_Curs) && PageCursors::NecessaryDrawCursors())
        )
    {
        return true;
    }
    
    TypeItem::E typeCurrentItem = Item::Current()->GetType();
    if (typeCurrentItem == TypeItem::Governor    ||
        typeCurrentItem == TypeItem::ChoiceReg)
    {
        return true;
    }

    TypeItem::E typeOpenedItem = Item::TypeOpened();

    if (typeOpenedItem == TypeItem::Choice       ||
        (typeOpenedItem == TypeItem::Page && ((Page *)Item::Opened())->NumSubPages() > 1)
        )
    {
        return true;
    }
    
    return false;
}


void Menu::SwitchSetLED()
{
    static bool first = true;
    static bool prevState = false;  // true - �����, false - �� �����

    bool state = NeedForFireSetLED();

    if (first)
    {
        first = false;
        Panel::EnableLEDRegSet(state);
        prevState = state;
    }
    else if (prevState != state)
    {
        Panel::EnableLEDRegSet(state);
        prevState = state;
    }
}


void Menu::OpenFileManager()
{
    angleRegSet = 0;
    for (int i = 0; i < 10; i++)
    {
        Handlers::ShortPressureButton(KeyOld::Menu);
        UpdateInput();
        Display::Update(false);
    }

    if (!IsShown())
    {
        Handlers::ShortPressureButton(KeyOld::Menu);
        UpdateInput();
        Display::Update(false);
    }

    for (int i = 0; i < 5 * stepAngleRegSet + 1; i++)
    {
        Handlers::RotateRegSetLeft();
        UpdateInput();
        Display::Update(false);
    }

    angleRegSet = 0;

    for (int i = 0; i < 2 * stepAngleRegSet + 1; i++)
    {
        Handlers::RotateRegSetRight();
        UpdateInput();
        Display::Update(false);
    }

    angleRegSet = 0;

    Handlers::ShortPressureButton(KeyOld::F2);
    UpdateInput();
    Display::Update(false);

    Handlers::ShortPressureButton(KeyOld::F4);
    UpdateInput();
    Display::Update(false);

    for (int i = 0; i < stepAngleRegSet + 1; i++)
    {
        Handlers::RotateRegSetLeft();
        UpdateInput();
        Display::Update(false);
    }

    for (int i = 0; i < 2; i++)
    {
        Handlers::ShortPressureButton(KeyOld::F1);
        UpdateInput();
        Display::Update(false);
    }
}
