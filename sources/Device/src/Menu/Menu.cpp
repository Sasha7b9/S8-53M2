// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Controls.h"
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
#include <string.h>



namespace Menu
{
    bool showDebugMenu = true;

    // Если произошло короткое нажатие кнопки, то здесь хранится имя этой кнопки до обработки этого  нажатия.
    Key::E shortPressureButton = Key::Empty;
    // Если произошло длинное нажатие кнопки, то здесь хранится имя этой кнопки до обработки этого нажатия.
    Key::E longPressureButton = Key::Empty;
    // При нажатии кнопки её имя записывается в эту переменную и хранится там до обратоки события нажатия кнопки.
    Key::E pressButton = Key::Empty;
    // При отпускании кнопки её имя записывается в эту переменную и хранится там до обработки  события отпускания кнопки.
    Key::E releaseButton = Key::Empty;
    // Угол, на который нужно повернуть ручку УСТАНОВКА - величина означает количество щелчков, знак - направление - "-" - влево, "+" - вправо
    int angleRegSet = 0;

    static const int stepAngleRegSet = 2;

    static const int  SIZE_BUFFER_FOR_BUTTONS = 5;
    static Key::E bufferForButtons[SIZE_BUFFER_FOR_BUTTONS] = {Key::Empty, Key::Empty, Key::Empty, Key::Empty, Key::Empty};
    static const Key::E sampleBufferForButtons[SIZE_BUFFER_FOR_BUTTONS] = {Key::F5, Key::F4, Key::F3, Key::F2, Key::F1};

    // Обработка короткого нажатия кнопки.
    void ProcessingShortPressureButton();
    // Обработка длинного нажатия кнопки.
    void ProcessingLongPressureButton();
    // Обработка опускания кнопки вниз.
    void ProcessingPressButton();
    // Обработка поднятия кнопки вверх.
    void ProcessingReleaseButton();
    // Обработка поворота ручки УСТАНОВКА.
    void ProcessingRegulatorSet();
    // Включить/выключить светодиод ручки УСТАНОВКА, если необходимо.
    void SwitchSetLED();
    // Обработка короткого нажатия на элемент NamePage с адресом page.
    void ShortPress_Page(Item *page);
    // Обработка короткого нажатия на элемент Choice с адресом choice.
    void ShortPress_Choice(Item *choice);

    void ShortPress_Time(Item *time);
    // Обработка короткого нажатия на элемент Button с адресом button.
    void ShortPress_Button(Item *button);
    // Обработка короткого нажатия на элемент Governor с адресом governor.
    void ShortPress_Governor(Item *governor);
    // Обработка короткого нажатия на элемент GovernorColor с адресом governorColor.
    void ShortPress_GovernorColor(Item *governorColor);

    void ShortPress_IP(Item *item);

    void ShortPress_MAC(Item *item);

    void ShortPress_ChoiceReg(Item *choice);

    void ShortPress_SmallButton(Item *smallButton);
    // Обработка длинного нажатия на элемент меню item.
    void FuncOnLongPressItem(Item *item);

    void FuncOnLongPressItemTime(Item *item);
    // Обработка длинного нажатия на элемент Button с адресом button.
    void FuncOnLongPressItemButton(Item *button);
    // Возвращает функцию обработки короткого нажатия на элемент меню item.
    void ExecuteFuncForShortPressOnItem(Item *item);
    // Возвращает функцию обработки длинного нажатия на элемент меню item.
    void ExecuteFuncForLongPressureOnItem(Item *item);

    // Возвращает true, если лампочка УСТАНОВКА должна гореть
    bool NeedForFireSetLED();

    void OpenFileManager();
    // Обработка события таймера автоматического сокрытия меню.
    void OnTimerAutoHide();

    void ProcessButtonForHint(Key::E button);
}


void Menu::UpdateInput()
{
    ProcessingShortPressureButton();
    ProcessingLongPressureButton();
    ProcessingRegulatorSet();
    ProcessingPressButton();
    ProcessingReleaseButton();
    SwitchSetLED();
    
    if(NEED_OPEN_FILE_MANAGER)
    {
        NEED_OPEN_FILE_MANAGER = 0;
        OpenFileManager();       
    }
};


void Menu::Handlers::ShortPressureButton(Key::E button)
{
    if (!Hint::show)
    {
        if(button == Key::Help)
        {
            Hint::show = !Hint::show;
            Hint::string = nullptr;
            Hint::item = nullptr;
        }
        shortPressureButton = button;
    }
};



void Menu::Handlers::LongPressureButton(Key::E button)
{
    if (!Hint::show)
    {
        longPressureButton = button;
        Display::Redraw();
    }
};


void Menu::ProcessButtonForHint(Key::E button)
{
    if (button == Key::Menu)
    {
        Hint::string = LANG_RU ?
            "Кнопка МЕНЮ выполняет следующие функции:\n"
            "1. При закрытом меню нажатие либо нажатие с удержанием в течение 0.5с открывает меню.\n"
            "2. При открытом меню удержание кнопки в течение 0.5с закрывает меню.\n"
            "3. При настройке \"СЕРВИС\x99Режим кн МЕНЮ\x99Закрывать\" текущей становится страница предыдущего уровня меню. Если текущей является корневая страница, меню закрывается.\n"
            "4. При настройке \"СЕРВИС\x99Режим кн МЕНЮ\x99Переключать\" текущей становится страница текущего уровня меню. Если текущая страница последняя в текущем уровне, происходит переход на предыдущий уровень меню.\n"
            "5. Если меню находится в режиме малых кнопок, то нажатие закрывает страницу."
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

    } else if (button == Key::Cursors)
    {
        Hint::string = LANG_RU ?
            "Кнопка КУРСОРЫ открывает меню курсорных измерений."
            :
            "КУРСОРЫ button to open the menu cursor measurements.";
    }
    else if (button == Key::Display)
    {
        Hint::string = LANG_RU ?
            "Кнопка ДИСПЛЕЙ открывает меню настроек дисплея."
            :
            "DISPLAY button opens the display settings menu.";
    }
    else if (button == Key::Memory)
    {
        Hint::string = LANG_RU ?
            "1. При настройке \"ПАМЯТЬ\x99ВНЕШН ЗУ\x99Реж кн ПАМЯТЬ\x99Меню\" открывает меню работы с памятью.\n"
            "2. При настройке \"ПАМЯТь\x99ВНЕШН ЗУ\x99Реж кн ПАМЯТЬ\x99Сохранение\" сохраняет сигнал на флеш-диск."
            :
            "1. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Menu\" opens a menu of memory\n"
            "2. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Save\" saves the signal to the flash drive";
    }
    else if (button == Key::Measures)
    {
        Hint::string = LANG_RU ?
            "Кнопка ИЗМЕР открывает меню автоматических измерений."
            :
            "ИЗМЕР button opens a menu of automatic measurements.";
    }
    else if (button == Key::Help)
    {
        Hint::string = LANG_RU ?
            "1. Кнопка ПОМОЩЬ открывает меню помощи.\n"
            "2. Нажатие и удержание кнопки ПОМОЩЬ в течение 0.5с включает и отключает режим вывода подсказок."
            :
            "1. ПОМОЩЬ button opens the help menu.\n"
            "2. Pressing and holding the ПОМОЩЬ for 0.5s enables and disables output mode hints.";
    }
    else if (button == Key::Service)
    {
        Hint::string = LANG_RU ?
            "Кнопка СЕРВИС открывает меню сервисных возможностей."
            :
            "СЕРВИС button opens a menu of service options.";
    }
    else if (button == Key::Start)
    {
        Hint::string = LANG_RU ?
            "Кнопка ПУСК/СTOП запускает и останавливает процесс сбора информации."
            :
            "ПУСК/СTOП button starts and stops the process of gathering information.";
    }
    else if (button == Key::ChannelA)
    {
        Hint::string = LANG_RU ?
            "1. Кнопка КАНАЛ1 открывает меню настроек канала 1.\n"
            "2. Нажатие и удержание кнопки КАНАЛ1 в течение 0.5с устанавливает смещение канала 1 по вертикали 0В."
            :
            "1. КАНАЛ1 button opens the settings menu of the channel 1.\n"
            "2. Pressing and holding the button КАНАЛ1 for 0.5c for the offset of the vertical channel 1 0V.";
    }
    else if (button == Key::ChannelB)
    {
        Hint::string = LANG_RU ?
            "1. Кнопка КАНАЛ2 открывает меню настроек канала 2.\n"
            "2. Нажатие и удержание кнопки КАНАЛ2 в течение 0.5с устанавливает смещение канала 2 по вертикали 0В."
            :
            "1. КАНАЛ2 button opens the settings menu of the channel 2.\n"
            "2. Pressing and holding the button КАНАЛ2 for 0.5c for the offset of the vertical channel 2 0V.";
    }
    else if (button == Key::Time)
    {
        Hint::string = LANG_RU ?
            "1. Кнопка РАЗВ открывает меню настроек развертки.\n"
            "2. Нажатие и удержание кнопки РАЗВ в течение 0.5с устанавливает смещение по горизонтали 0с."
            :
            "1. РАЗВ button open the settings menu sweep.\n"
            "2. Pressing and holding the button РАЗВ for 0.5s Is the offset horizontal 0s.";
    }
    else if (button == Key::Trig)
    {
    Hint::string = LANG_RU ?
            "1. Кнопка СИНХР открывает меню настроек синхронизации.\n"
            "2. Нажатие и удержание в течение 0.5с кнопки СИНХР при настройке \"СЕРВИС\x99Реж длит СИНХР\x99Автоуровень\" производит автоматическую настройку уровня синхронизации.\n"
            "3. Нажатие и удержание в течение 0.5с кнопки СИНХР при настройке \"СЕРВИС\x99Реж длит СИНХР\x99Сброс уровня\" устанавливает уровень синхронизации 0В."
            :
            "1. СИНХР button opens a menu settings synchronization.\n"
#ifndef WIN32
#pragma push
#pragma diag_suppress 192
#endif
            "2. Pressing and holding the button СИНХР for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99\x41utolevel\" automatically adjust the trigger level.\n"
#ifndef WIN32
#pragma pop
#endif
            "3. Pressing and holding the button СИНХР for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99SReset trig level\" sets the trigger level 0V.";
    }
    else
    {
        shortPressureButton = button;
    }
}


void Menu::Handlers::PressButton(Key::E button)
{
    Sound::ButtonPress();

    if (Hint::show)
    {
        ProcessButtonForHint(button);
        return;
    }

    if (pressButton == Key::Start)
    {
    } 
    else if (!Menu::IsShown())
    {
        for (int i = SIZE_BUFFER_FOR_BUTTONS - 1; i > 0; i--)
        {
            bufferForButtons[i] = bufferForButtons[i - 1];
        }
        bufferForButtons[0] = button;
      
        if (memcmp(bufferForButtons, sampleBufferForButtons, SIZE_BUFFER_FOR_BUTTONS * sizeof(Key::E)) == 0)
        {
            showDebugMenu = true;
            Display::ShowWarningGood(Warning::MenuDebugEnabled);
        }
    }

    pressButton = button;
};


void Menu::Handlers::ReleaseButton(Key::E button)
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
    if(shortPressureButton != Key::Empty)
    {
        if (shortPressureButton == Key::Memory && MODE_BTN_MEMORY_IS_SAVE && FDrive::isConnected)
        {
            EXIT_FROM_SET_NAME_TO = IsShown() ? RETURN_TO_MAIN_MENU : RETURN_TO_DISABLE_MENU;
            PageMemory::SaveSignalToFlashDrive();
            shortPressureButton = Key::Empty;
            return;
        }
        Display::Redraw();
        Menu::SetAutoHide(true);

        Key::E button = shortPressureButton;

        do
        {
            if(button == Key::Menu)                                   // Если нажата кнопка МЕНЮ и мы не находимся в режме настройки измерений.
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
            else if (IsShown() && Key::IsFunctional(button))       // Если меню показано и нажата функциональная клавиша
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
            else                                                        // Если меню не показано.
            {
                NamePage::E name = ((const Page *)Item::Opened())->GetName();
                if(button == Key::ChannelA && name == NamePage::Channel0)
                {
                    SET_ENABLED_A = !Chan::Enabled(Chan::A);
                    PageChannelA::OnChanged_Input(true);
                    break;
                }
                if(button == Key::ChannelB && name == NamePage::Channel1)
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

        shortPressureButton = Key::Empty;
    }
}


void Menu::ProcessingLongPressureButton()
{
    if(longPressureButton != Key::Empty)
    {
        Display::Redraw();
        Menu::SetAutoHide(true);

        if(longPressureButton == Key::Time)
        {
            TShift::Set(0);
        }
        else if(longPressureButton == Key::Trig)
        {
            TrigLev::Set(TRIG_SOURCE, TrigLev::ZERO);
        }
        else if(longPressureButton == Key::ChannelA)
        {
            RShift::Set(Chan::A, RShift::ZERO);
        }
        else if(longPressureButton == Key::ChannelB)
        {
            RShift::Set(Chan::B, RShift::ZERO);
        }
        else if(longPressureButton == Key::Menu)
        {
                Show(!IsShown());
        }
        else if(IsShown() && Key::IsFunctional(longPressureButton))
        {
            Item *item = Item::UnderKey(longPressureButton);
            ExecuteFuncForLongPressureOnItem(item);
        }
        longPressureButton = Key::Empty;
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
    if (pressButton == Key::Start && !MODE_WORK_IS_LATEST)
    {
        FPGA::OnPressStartStop();
    } 
    else if((pressButton >= Key::F1 && pressButton <= Key::F5) || pressButton == Key::Menu)
    {
        if (pressButton != Key::Menu)
        {
            Item::underKey = Item::UnderKey(pressButton);
        }
    }
    pressButton = Key::Empty;
}


void Menu::ProcessingReleaseButton()
{
    if(releaseButton >= Key::F1 && releaseButton <= Key::F5 || pressButton == Key::Menu)
    {
        Item::underKey = nullptr;
        releaseButton = Key::Empty;
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
        ((Choice *)choice)->FuncOnChanged(false);
    }
    else if (!choice->IsOpened())
    {
        choice->SetCurrent(Item::Current() != choice);
        ((Choice *)choice)->StartChange(1);
    }
    else
    {
        ((Choice *)choice)->ChangeValue(-1);
    }
}


void Menu::ShortPress_ChoiceReg(Item *choice)
{
    if(!choice->IsActive()) 
    {
        ((Choice *)choice)->FuncOnChanged(false);
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

    GovernorColor *governor = (GovernorColor*)governorColor;

    if(Item::Opened() == (Item *)governor)
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
        NEED_OPEN_FILE_MANAGER = 1;
    }
}


extern const Page pService;


void Time::Open()
{
    Display::ShowWarningGood(Warning::TimeNotSet);
    Menu::Handlers::ShortPressureButton(Key::Service);
    Menu::UpdateInput();
    Display::Update();

    for (int i = 0; i < 2; i++)
    {
        Menu::Handlers::RotateRegSetRight();
        Menu::UpdateInput();
        Display::Update();
    }

    Menu::Handlers::ShortPressureButton(Key::F4);
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
    static bool prevState = false;  // true - горит, false - не горит

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
        Handlers::ShortPressureButton(Key::Menu);
        UpdateInput();
        Display::Update(false);
    }

    if (!IsShown())
    {
        Handlers::ShortPressureButton(Key::Menu);
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

    Handlers::ShortPressureButton(Key::F2);
    UpdateInput();
    Display::Update(false);

    Handlers::ShortPressureButton(Key::F4);
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
        Handlers::ShortPressureButton(Key::F1);
        UpdateInput();
        Display::Update(false);
    }
}
