// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Panel.h"
#include "Controls.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Containers/Queue.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#include <stdio.h>
#include <string.h>
#include "Panel/PanelFunctions.cpp"


namespace Panel
{
    const uint8 LED_TRIG     = 1;
    const uint8 LED_REG_SET  = 2;
    const uint8 LED_CHAN_A   = 3;
    const uint8 LED_CHAN_B   = 4;

    Key::E pressedKey = Key::Empty;
    volatile Key::E pressedButton = Key::Empty;         // Это используется для отслеживания нажатой кнопки при отключенной панели

    Queue<uint8> data_for_send;                         // Здесь данные для пересылки в панель

    uint timePrevPressButton = 0;
    uint timePrevReleaseButton = 0;

    Queue<uint16> input_buffer;

    // Преобразует данные из новой панели в данные, опознаваемые старой прошивкой
    uint16 TranslateCommand(const uint8 *data, uint size);

    void OnTimerPressedKey();

    Key::E ButtonIsRelease(uint16 command);

    Key::E ButtonIsPress(uint16 command);

    Regulator RegulatorLeft(uint16 command);

    Regulator RegulatorRight(uint16 command);

    Regulator RegulatorIsPress(uint16 command);

    static void(*funcOnKeyDown[Key::Count])() =
    {
        0,
        EmptyFuncVV,    // Key::ChannelA
        EmptyFuncVV,    // Key::Service
        EmptyFuncVV,    // Key::ChannelB
        EmptyFuncVV,    // Key::Display
        EmptyFuncVV,    // Key::Time
        EmptyFuncVV,    // Key::Memory                                         
        EmptyFuncVV,    // B_Sinchro
        StartDown,      // Key::Start
        EmptyFuncVV,    // Key::Cursors
        EmptyFuncVV,    // Key::Measures
        PowerDown,      // Key::Power
        EmptyFuncVV,    // Key::Help
        EmptyFuncVV,    // Key::Menu
        EmptyFuncVV,    // Key::F1
        EmptyFuncVV,    // Key::F2
        EmptyFuncVV,    // Key::F3
        EmptyFuncVV,    // Key::F4
        EmptyFuncVV     // Key::F5
    };

    static void (*funcOnKeyUp[Key::Count])() =
    {
        0,
        EmptyFuncVV,    // Key::ChannelA
        EmptyFuncVV,    // Key::Service
        EmptyFuncVV,    // Key::ChannelB
        EmptyFuncVV,    // Key::Display
        EmptyFuncVV,    // Key::Time
        EmptyFuncVV,    // Key::Memory
        EmptyFuncVV,    // B_Sinchro
        EmptyFuncVV,    // Key::Start
        EmptyFuncVV,    // Key::Cursors
        EmptyFuncVV,    // Key::Measures
        EmptyFuncVV,    // Key::Power
        EmptyFuncVV,    // Key::Help
        EmptyFuncVV,    // Key::Menu
        EmptyFuncVV,    // Key::F1
        EmptyFuncVV,    // Key::F2
        EmptyFuncVV,    // Key::F3
        EmptyFuncVV,    // Key::F4
        EmptyFuncVV     // Key::F5
    };

    static void (*funcOnLongPressure[Key::Count])() =
    {
        0,
        ChannelALong,   // Key::ChannelA
        EmptyFuncVV,    // Key::Service
        ChannelBLong,   // Key::ChannelB
        EmptyFuncVV,    // Key::Display
        TimeLong,       // Key::Time
        EmptyFuncVV,    // Key::Memory
        TrigLong,       // B_Sinchro
        EmptyFuncVV,    // Key::Start
        EmptyFuncVV,    // Key::Cursors
        EmptyFuncVV,    // Key::Measures
        EmptyFuncVV,    // Key::Power
        HelpLong,       // Key::Help
        MenuLong,       // Key::Menu
        F1Long,         // Key::F1
        F2Long,         // Key::F2
        F3Long,         // Key::F3
        F4Long,         // Key::F4
        F5Long          // Key::F5
    };

    static void (*funcOnRegulatorPress[8])() =
    {
        ChannelALong,   // 20 - R_RangeA
        ChannelALong,   // 21 - R_RShiftA
        ChannelBLong,   // 22 - R_RangeB
        ChannelBLong,   // 23 - R_RShiftB
        TimeLong,       // 24 - R_TBase
        TimeLong,       // 25 - R_TShift
        TrigLong,       // 26 - R_TrigLev
        MenuLong        // 27 - R_Set
    };

    static void (*funculatorLeft[R_Set + 1])() =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        RangeLeftA,     // R_RangeA
        RShift0Left,    // R_RShiftA
        RangeLeftB,     // R_RangeB
        RShift1Left,    // R_RShiftB
        TBaseLeft,      // R_TBase
        TShiftLeft,     // R_TShift
        TrigLevLeft,    // R_TrigLev
        SetLeft         // R_Set
    };

    static void (*funculatorRight[R_Set + 1])() =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        RangeRightA,    // R_RangeA
        RShift0Right,   // R_RShiftA
        RangeRightB,    // R_RangeB
        RShift1Right,   // R_RShiftB
        TBaseRight,     // R_TBase
        TShiftRight,    // R_TShift
        TrigLevRight,   // R_TrigLev
        SetRight        // R_Set
    };
}



Key::E Panel::ButtonIsRelease(uint16 command)
{
    Key::E button = Key::Empty;

    if(command < Key::Count && command > Key::Empty)
    {
        if(TIME_MS - timePrevReleaseButton > 100)
        {
            button = (Key::E)command;
            timePrevReleaseButton = TIME_MS;
        }
    }

    return button;
}


Key::E Panel::ButtonIsPress(uint16 command)
{
    Key::E button = Key::Empty;

    if (((command & 0x7f) < Key::Count) && ((command & 0x7f) > Key::Empty))
    {
        if(TIME_MS - timePrevPressButton > 100)
        {
            button = (Key::E)(command & 0x7f);
            timePrevPressButton = TIME_MS;
        }
    }

    return button;
}


Regulator Panel::RegulatorIsPress(uint16 command)
{
    if ((command & 0xC0) != 0xC0)
    {
        return R_Empty;
    }

    return (Regulator)(command & 0x3F);
}


Regulator Panel::RegulatorLeft(uint16 command)
{
    if(command >= 20 && command <= 27)
    {
        return (Regulator)command;
    }

    return R_Empty;
}


Regulator Panel::RegulatorRight(uint16 command)
{
    if(((command & 0x7f) >= 20) && ((command & 0x7f) <= 27))
    {
        return (Regulator)(command & 0x7f);
    }
    return R_Empty;
}


void Panel::OnTimerPressedKey()
{
    if(pressedKey != Key::Empty)
    {
        void (*func)() = funcOnLongPressure[pressedKey];
        Menu::Handlers::ReleaseButton(pressedKey);
        if(func != 0)
        {
            func();
        }
        pressedKey = Key::Empty;
    }
    Timer::Disable(TypeTimer::PressKey);
}


void Panel::ProcessingCommandFromPIC(uint16 command)
{
    Key::E releaseButton = ButtonIsRelease(command);
    Key::E pressButton = ButtonIsPress(command);
    Regulator regLeft = RegulatorLeft(command);
    Regulator regRight = RegulatorRight(command);
    Regulator regPress = RegulatorIsPress(command);

    if (pressButton != Key::Empty)
    {
        pressedButton = pressButton;
    }

    if(PANEL_IS_RUNNING == 0)
    {
        return;
    }

    if(releaseButton != Key::Empty)
    {
        Menu::Handlers::ReleaseButton(releaseButton);

        funcOnKeyUp[releaseButton]();

        if(pressedKey != Key::Empty)
        {
            Menu::Handlers::ShortPressureButton(releaseButton);
            pressedKey = Key::Empty;
        }
    }
    else if(pressButton != Key::Empty)
    {
        funcOnKeyDown[pressButton]();
        Menu::Handlers::PressButton(pressButton);
        pressedKey = pressButton;
        Timer::Enable(TypeTimer::PressKey, 500, OnTimerPressedKey);
    }
    else if(regLeft != R_Empty)
    {
         funculatorLeft[regLeft]();
    }
    else if(regRight != R_Empty)
    {
        funculatorRight[regRight]();
    }

    if (regPress != R_Empty)
    {
        int index = regPress - R_RangeA;

        funcOnRegulatorPress[index]();
    }
}


void Panel::EnableLEDChannelA(bool enable)
{
    uint8 data = LED_CHAN_A;

    if (enable)
    {
        data |= 0x80;
    }

    TransmitData(data);
}


void Panel::EnableLEDChannelB(bool enable)
{
    uint8 data = LED_CHAN_B;

    if (enable)
    {
        data |= 0x80;
    }

    TransmitData(data);
}


void Panel::EnableLEDTrig(bool enable)
{
    static uint timeEnable = 0;
    static bool first = true;
    static bool fired = false;

    if(first)
    {
        Panel::TransmitData(LED_TRIG);
        Display::EnableTrigLabel(false);
        timeEnable = TIME_MS;
        first = false;
    }

    if(enable)
    {
        timeEnable = TIME_MS;
    }

    if(enable != fired)
    {
        if(enable)
        {
            Panel::TransmitData(LED_TRIG | 0x80);
            Display::EnableTrigLabel(true);
            fired = true;
        }
        else if(TIME_MS - timeEnable > 100)
        {
            Panel::TransmitData(LED_TRIG);
            Display::EnableTrigLabel(false);
            fired = false;
        }
    }
}


void Panel::TransmitData(uint8 data)
{
    if (data_for_send.Size() < 20)
    {
        data_for_send.Push(data);
    }
}


uint16 Panel::NextData()
{
    return data_for_send.Front();
}


void Panel::Disable()
{
    PANEL_IS_RUNNING = 0;
}


void Panel::Enable()
{
    PANEL_IS_RUNNING = 1;
}


void Panel::Init()
{
    EnableLEDRegSet(false);
    EnableLEDTrig(false);
}


void Panel::EnableLEDRegSet(bool enable)
{
    uint8 data = LED_REG_SET;

    if (enable)
    {
        data |= 0x80;
    }

    TransmitData(data);
}


Key::E Panel::WaitPressingButton()
{
    pressedButton = Key::Empty;
    while (pressedButton == Key::Empty) {};
    return pressedButton;
}


uint16 Panel::TranslateCommand(const uint8 *data, uint size)
{
    static const int NUM_BUTTONS = 27;

    static const uint16 commands[NUM_BUTTONS] =
    {
        Key::Empty,
        Key::F1,
        Key::F2,
        Key::F3,
        Key::F4,
        Key::F5,
        Key::ChannelA,
        Key::ChannelB,
        Key::Time,
        Key::Trig,
        Key::Cursors,
        Key::Measures,
        Key::Display,
        Key::Help,
        Key::Start,
        Key::Memory,
        Key::Service,
        Key::Menu,
        Key::Power,
        R_RangeA,
        R_RangeB,
        R_RShiftA,
        R_RShiftB,
        R_TBase,
        R_TShift,
        R_TrigLev,
        R_Set
    };

    uint16 command = 0;

    uint8 key = data[1];

    if (key > 0 && key < NUM_BUTTONS)
    {
        command = commands[key];

        if (command >= 20)
        {
            if (data[2] == 1)       { command |= 0xC0; }        // Нажатие ручки
            else if (data[2] == 3)  { }                         // Поворот влево
            else if (data[2] == 4)  { command |= 0x80; }        // Поворот вправо
            else                    { command = 0; }
        }
        else
        {
            if (data[2] == 0)       { command |= 0x80; }
        }
    }

    return command;
}


void Panel::CallbackOnReceiveSPI5(const uint8 *data, uint size)
{
    uint16 command = TranslateCommand(data, size);

    if (command != 0)
    {
        input_buffer.Push(command);
        Settings::NeedSave();
    }
}


void Panel::Update()
{
    if (!input_buffer.IsEmpty())
    {
        uint16 command = input_buffer.Front();
        ProcessingCommandFromPIC(command);
    }
}
