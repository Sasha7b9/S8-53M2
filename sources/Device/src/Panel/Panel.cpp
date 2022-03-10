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

    static void FuncNone(Action)
    {

    }

    static void FuncF1(Action action)
    {
        if (action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::F1);
        }
    }

    static void FuncF2(Action action)
    {
        if (action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::F2);
        }
    }

    static void FuncF3(Action action)
    {
        if(action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::F3);
        }
    }

    static void FuncF4(Action action)
    {
        if(action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::F4);
        }
    }

    static void FuncF5(Action action)
    {
        if(action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::F5);
        }
    }

    static void FuncChannelA(Action action)
    {
        if (action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::ChannelA);
        }
    }

    static void FuncChannelB(Action action)
    {
        if (action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::ChannelB);
        }
    }

    static void FuncTime(Action action)
    {
        if (action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::Time);
        }
    }

    static void FuncTrig(Action action)
    {
        if (action.IsLong())
        {
            if (MODE_LONG_PRESS_TRIG_IS_LEVEL_ZERO)
            {
                Menu::Handlers::LongPressureButton(Key::Trig);
            }
            else
            {
                FPGA::FindAndSetTrigLevel();
            }
        }
    }

    static void FuncCursors(Action)
    {

    }

    static void FuncMeasures(Action)
    {

    }

    static void FuncDisplay(Action)
    {

    }

    static void FuncHelp(Action)
    {
        Hint::show = !Hint::show;
        Hint::string = nullptr;
        Hint::item = nullptr;
    }

    static void FuncStart(Action action)
    {
        if (action.IsDown())
        {
            if (MODE_WORK_IS_DIRECT)
            {
                Menu::Handlers::PressButton(Key::Start);
            }
        }
    }

    static void FuncMemory(Action)
    {

    }

    static void FuncService(Action)
    {

    }

    static void FuncMenu(Action action)
    {
        if (action.IsLong())
        {
            Menu::Handlers::LongPressureButton(Key::Menu);
        }
    }

    static void FuncPower(Action action)
    {
        if (action.IsDown())
        {
            ((Page *)Item::Opened())->ShortPressOnItem(0);
            Settings::SaveBeforePowerDown();
            Log::DisconnectLoggerUSB();

            if (TIME_MS > 1000)
            {
                Panel::TransmitData(0x05);
            }
        }
    }

    static void FuncRangeA(Action action)
    {
        Sound::RegulatorSwitchRotate();

        if (action.IsLeft())
        {
            Range::Increase(Chan::A);
        }
        else if (action.IsRight())
        {
            Range::Decrease(Chan::A);
        }
    }

    static void FuncRangeB(Action action)
    {
        Sound::RegulatorSwitchRotate();

        if (action.IsLeft())
        {
            Range::Increase(Chan::B);
        }
        else if (action.IsRight())
        {
            Range::Decrease(Chan::B);
        }
    }

    static void FuncRShiftA(Action action)
    {
        if (action.IsLeft())
        {
            static int prevTime = 0;
            ChangeRShift(&prevTime, SetRShift, Chan::A, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static int prevTime = 0;
            ChangeRShift(&prevTime, SetRShift, Chan::A, +RShift::STEP);
        }
    }

    static void FuncRShiftB(Action action)
    {
        if (action.IsLeft())
        {
            static int prevTime = 0;
            ChangeRShift(&prevTime, SetRShift, Chan::B, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static int prevTime = 0;
            ChangeRShift(&prevTime, SetRShift, Chan::B, +RShift::STEP);
        }
    }

    static void FuncTBase(Action action)
    {
        Sound::RegulatorSwitchRotate();

        if (action.IsLeft())
        {
            TBase::Increase();
        }
        else if (action.IsRight())
        {
            TBase::Decrease();
        }
    }

    static void FuncTShift(Action action)
    {
        if (action.IsLeft())
        {
            XShift(-1);
        }
        else if (action.IsRight())
        {
            XShift(1);
        }
    }

    static void FuncTrigLev(Action action)
    {
        if (action.IsLeft())
        {
            static int prevTime = 0;
            ChangeTrigLev(&prevTime, SetTrigLev, TRIG_SOURCE, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static int prevTime = 0;
            ChangeTrigLev(&prevTime, SetTrigLev, TRIG_SOURCE, +RShift::STEP);
        }
    }

    static void FuncSetting(Action action)
    {
        if (action.IsLeft())
        {
            Menu::Handlers::RotateRegSetLeft();
        }
        else if (action.IsRight())
        {
            Menu::Handlers::RotateRegSetRight();
        }
    }

    static void (* const funcOnKey[Key::Count])(Action) =
    {
        FuncNone,
        FuncF1,
        FuncF2,
        FuncF3,
        FuncF4,
        FuncF5,
        FuncChannelA,
        FuncChannelB,
        FuncTime,
        FuncTrig,
        FuncCursors,
        FuncMeasures,
        FuncDisplay,
        FuncHelp,
        FuncStart,
        FuncMemory,
        FuncService,
        FuncMenu,
        FuncPower,
        FuncRangeA,
        FuncRangeB,
        FuncRShiftA,
        FuncRShiftB,
        FuncTBase,
        FuncTShift,
        FuncTrigLev,
        FuncSetting
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
    if (!isRunning)
    {
        if (event.IsDown())
        {
            pressedButton = event.key;
        }

        return;
    }

    if (event.IsDown())
    {
        Menu::Handlers::ShortPressureButton(event.key);
    }
    else if (event.IsUp())
    {
        Menu::Handlers::ReleaseButton(event.key);
    }

    funcOnKey[event.key](event.action);
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
