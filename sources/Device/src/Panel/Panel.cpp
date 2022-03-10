// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Panel.h"
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
#include "common/Panel/Controls.h"
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

    Key::E pressedKey = Key::None;
    volatile Key::E pressedButton = Key::None;         // Это используется для отслеживания нажатой кнопки при отключенной панели

    Queue<uint8> data_for_send;                         // Здесь данные для пересылки в панель

    Queue<KeyboardEvent> input_buffer;

    bool isRunning = true;

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

    static void (*funcOnRegulatorPress[Key::Count])() =
    {
        ChannelALong,   // 20 - RegulatorOld::RangeA
        ChannelALong,   // 21 - RegulatorOld::RShiftA
        ChannelBLong,   // 22 - RegulatorOld::RangeB
        ChannelBLong,   // 23 - RegulatorOld::RShiftB
        TimeLong,       // 24 - RegulatorOld::TBase
        TimeLong,       // 25 - RegulatorOld::TShift
        TrigLong,       // 26 - RegulatorOld::TrigLev
        MenuLong        // 27 - RegulatorOld::Set
    };

    static void (*funculatorLeft[Key::Count])() =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        RangeLeftA,     // RegulatorOld::RangeA
        RangeLeftB,     // RegulatorOld::RangeB
        RShift0Left,    // RegulatorOld::RShiftA
        RShift1Left,    // RegulatorOld::RShiftB
        TBaseLeft,      // RegulatorOld::TBase
        TShiftLeft,     // RegulatorOld::TShift
        TrigLevLeft,    // RegulatorOld::TrigLev
        SetLeft         // RegulatorOld::Set
    };

    static void (*funculatorRight[Key::Count])() =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        RangeRightA,    // RegulatorOld::RangeA
        RangeRightB,    // RegulatorOld::RangeB
        RShift0Right,   // RegulatorOld::RShiftA
        RShift1Right,   // RegulatorOld::RShiftB
        TBaseRight,     // RegulatorOld::TBase
        TShiftRight,    // RegulatorOld::TShift
        TrigLevRight,   // RegulatorOld::TrigLev
        SetRight        // RegulatorOld::Set
    };
}


Key::E Panel::WaitPressingButton()
{
    Timer::PauseOnTime(500);

    input_buffer.Clear();

    pressedButton = Key::None;

    while (pressedButton == Key::None)
    {
        Panel::Update();
    };

    input_buffer.Clear();

    return pressedButton;
}


void Panel::ProcessingKeyboardEvent(KeyboardEvent event)
{
    Key::E releaseButton = event.IsUp() ? event.key.value : Key::None;
    Key::E pressButton = event.IsDown() ? event.key.value : Key::None;
    Key::E regLeft = event.IsLeft() ? event.key.value : Key::None;
    Key::E regRight = event.IsRight() ? event.key.value : Key::None;
    Key::E regPress = event.IsDown() ? event.key.value : Key::None;

    if (pressButton != Key::None)
    {
        pressedButton = pressButton;
    }

    if(!isRunning)
    {
        return;
    }

    if(releaseButton != Key::None)
    {
        Menu::Handlers::ReleaseButton(releaseButton);

        funcOnKeyUp[releaseButton]();

        if(pressedKey != Key::None)
        {
            Menu::Handlers::ShortPressureButton(releaseButton);
            pressedKey = Key::None;
        }
    }
    else if(regLeft != Key::None)
    {
         funculatorLeft[regLeft]();
    }
    else if(regRight != Key::None)
    {
        funculatorRight[regRight]();
    }

    if (regPress != Key::None)
    {
        Sound::ButtonPress();

        funcOnRegulatorPress[event.key]();
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
    isRunning = false;
}


void Panel::Enable()
{
    isRunning = true;
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


void Panel::CallbackOnReceiveSPI5(const uint8 *data, uint)
{
    KeyboardEvent event(data);

    if (event.key != Key::None)
    {
        input_buffer.Push(event);
        Settings::NeedSave();
    }
}


void Panel::Update()
{
    if (!input_buffer.IsEmpty())
    {
        ProcessingKeyboardEvent(input_buffer.Front());
    }
}
