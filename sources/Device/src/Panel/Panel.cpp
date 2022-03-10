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


    namespace Processing
    {
        static const uint MIN_TIME = 500;


        static void ChangeRShift(int *prevTime, void(*f)(Chan::E, int16), Chan::E ch, int16 relStep);

        int CalculateCount(int *prevTime)
        {
            uint time = TIME_MS;
            uint delta = time - *prevTime;
            *prevTime = (int)time;

            if (delta > 75)
            {
                return 1;
            }
            else if (delta > 50)
            {
                return 2;
            }
            else if (delta > 25)
            {
                return 3;
            }
            return 4;
        }

        bool CanChangeTShift(int16 tShift)
        {
            static uint time = 0;
            if (tShift == 0)
            {
                time = TIME_MS;
                return true;
            }
            else if (time == 0)
            {
                return true;
            }
            else if (TIME_MS - time > MIN_TIME)
            {
                time = 0;
                return true;
            }
            return false;
        }

        bool CanChangeRShiftOrTrigLev(TrigSource::E ch, int16 rShift)
        {
            static uint time[3] = {0, 0, 0};
            if (rShift == RShift::ZERO)
            {
                time[ch] = TIME_MS;
                return true;
            }
            else if (time[ch] == 0)
            {
                return true;
            }
            else if (TIME_MS - time[ch] > MIN_TIME)
            {
                time[ch] = 0;
                return true;
            }
            return false;
        }

        void ChangeRShift(int *prevTime, void(*f)(Chan::E, int16), Chan::E ch, int16 relStep)
        {
            if (ENUM_ACCUM_IS_NONE)
            {
                FPGA::TemporaryPause();
            }
            int count = CalculateCount(prevTime);
            int rShiftOld = SET_RSHIFT(ch);
            int rShift = SET_RSHIFT(ch) + relStep * count;
            if ((rShiftOld > RShift::ZERO && rShift < RShift::ZERO) || (rShiftOld < RShift::ZERO && rShift > RShift::ZERO))
            {
                rShift = RShift::ZERO;
            }
            if (CanChangeRShiftOrTrigLev((TrigSource::E)ch, (int16)rShift))
            {
                Sound::RegulatorShiftRotate();
                f(ch, (int16)rShift);
            }
        }

        void ChangeTrigLev(int *prevTime, void(*f)(TrigSource::E, int16), TrigSource::E trigSource, int16 relStep)
        {
            int count = CalculateCount(prevTime);
            int trigLevOld = TRIG_LEVEL(trigSource);
            int trigLev = TRIG_LEVEL(trigSource) + relStep * count;
            if ((trigLevOld > TrigLev::ZERO && trigLev < TrigLev::ZERO) || (trigLevOld < TrigLev::ZERO && trigLev > TrigLev::ZERO))
            {
                trigLev = TrigLev::ZERO;
            }
            if (CanChangeRShiftOrTrigLev(trigSource, (int16)trigLev))
            {
                Sound::RegulatorShiftRotate();
                f(trigSource, (int16)trigLev);
            }
        }

        void ChangeTShift(int *prevTime, void(*f)(int), int16 relStep)
        {
            int count = CalculateCount(prevTime);
            int tShiftOld = SET_TSHIFT;
            float step = relStep * count;
            if (step < 0)
            {
                if (step > -1)
                {
                    step = -1;
                }
            }
            else
            {
                if (step < 1)
                {
                    step = 1;
                }
            }

            int16 tShift = SET_TSHIFT + step;
            if (((tShiftOld > 0) && (tShift < 0)) || (tShiftOld < 0 && tShift > 0))
            {
                tShift = 0;
            }
            if (CanChangeTShift(tShift))
            {
                Sound::RegulatorShiftRotate();
                f(tShift);
            }
        }

        void ChangeShiftScreen(int *prevTime, void(*f)(int), int16 relStep)
        {
            int count = CalculateCount(prevTime);
            float step = relStep * count;
            if (step < 0)
            {
                if (step > -1)
                {
                    step = -1;
                }
            }
            else if (step < 1)
            {
                step = 1;
            }
            f(step);
        }

        static void SetRShift(Chan::E ch, int16 rShift)
        {
            RShift::Set(ch, rShift);
        }



        static void SetTrigLev(TrigSource::E ch, int16 trigLev)
        {
            TrigLev::Set(ch, trigLev);
        }


        static void ShiftScreen(int shift)
        {
            Display::ShiftScreen(shift);
        }

        static void SetTShift(int tShift)
        {
            TShift::Set(tShift);
        }

        void XShift(int delta)
        {
            static int prevTime = 0;
            if (!FPGA::IsRunning() || SET_TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY)
            {
                if (!SET_ENUM_POINTS_IS_281)
                {
                    ChangeShiftScreen(&prevTime, ShiftScreen, 2 * delta);
                }
            }
            else
            {
                ChangeTShift(&prevTime, SetTShift, (int16)delta);
            }
        }
    }


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
            Processing::ChangeRShift(&prevTime, Processing::SetRShift, Chan::A, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static int prevTime = 0;
            Processing::ChangeRShift(&prevTime, Processing::SetRShift, Chan::A, +RShift::STEP);
        }
    }

    static void FuncRShiftB(Action action)
    {
        if (action.IsLeft())
        {
            static int prevTime = 0;
            Processing::ChangeRShift(&prevTime, Processing::SetRShift, Chan::B, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static int prevTime = 0;
            Processing::ChangeRShift(&prevTime, Processing::SetRShift, Chan::B, +RShift::STEP);
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
            Processing::XShift(-1);
        }
        else if (action.IsRight())
        {
            Processing::XShift(1);
        }
    }

    static void FuncTrigLev(Action action)
    {
        if (action.IsLeft())
        {
            static int prevTime = 0;
            Processing::ChangeTrigLev(&prevTime, Processing::SetTrigLev, TRIG_SOURCE, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static int prevTime = 0;
            Processing::ChangeTrigLev(&prevTime, Processing::SetTrigLev, TRIG_SOURCE, +RShift::STEP);
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
