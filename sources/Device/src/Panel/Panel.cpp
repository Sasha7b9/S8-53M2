// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Containers/Queue.h"
#include "common/Panel/Controls.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/Pages/Definition.h"
#include "Utils/PasswordResolver.h"


namespace Panel
{
    const uint8 LED_TRIG     = 1;
    const uint8 LED_REG_SET  = 2;
    const uint8 LED_CHAN_A   = 3;
    const uint8 LED_CHAN_B   = 4;

    Key::E pressedKey = Key::None;
    volatile Key::E pressedButton = Key::None;      // Это используется для отслеживания нажатой кнопки при отключенной
                                                    // панели

    Queue<uint8> data_for_send;                     // Здесь данные для пересылки в панель

    StaticQueue<KeyboardEvent> input_buffer(10);   // Основной буфер событий
    StaticQueue<KeyboardEvent> aux_buffer(10);     // Вспомогательный буфер - сюда помещаются события, если во время прерывания идёт
                                // работа с основным буфером input_buffer
    bool isRunning = true;

    uint timeLastEvent = 0;

    namespace Processing
    {
        static const uint MIN_TIME = 500;


        static void ChangeRShift(TimeMeterMS *tMeter, void(*f)(Chan::E, int16), Chan::E ch, int16 relStep);

        int CalculateCount(TimeMeterMS *tMeter)
        {
            uint delta = tMeter->ElapsedTime();
            tMeter->Reset();

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

        void ChangeRShift(TimeMeterMS *tMeter, void(*f)(Chan::E, int16), Chan::E ch, int16 relStep)
        {
            if (ENUM_ACCUM_IS_NONE)
            {
                FPGA::TemporaryPause();
            }
            int count = CalculateCount(tMeter);
            int rShiftOld = SET_RSHIFT(ch);
            int rShift = SET_RSHIFT(ch) + relStep * count;
            if ((rShiftOld > RShift::ZERO && rShift < RShift::ZERO) || (rShiftOld < RShift::ZERO && rShift > RShift::ZERO))
            {
                rShift = RShift::ZERO;
            }
            if (CanChangeRShiftOrTrigLev((TrigSource::E)ch, (int16)rShift))
            {
                f(ch, (int16)rShift);
            }
        }

        void ChangeTrigLev(TimeMeterMS *tMeter, void(*f)(TrigSource::E, int16), TrigSource::E trigSource, int16 relStep)
        {
            int count = CalculateCount(tMeter);
            int trigLevOld = TRIG_LEVEL(trigSource);
            int trigLev = TRIG_LEVEL(trigSource) + relStep * count;
            if ((trigLevOld > TrigLev::ZERO && trigLev < TrigLev::ZERO) || (trigLevOld < TrigLev::ZERO && trigLev > TrigLev::ZERO))
            {
                trigLev = TrigLev::ZERO;
            }
            if (CanChangeRShiftOrTrigLev(trigSource, (int16)trigLev))
            {
                f(trigSource, (int16)trigLev);
            }
        }

        void ChangeTShift(TimeMeterMS *tMeter, void(*f)(int), int16 relStep)
        {
            int count = CalculateCount(tMeter);
            int tshift_old = SET_TSHIFT;
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

            int16 tshift = SET_TSHIFT + step;
            if (((tshift_old > 0) && (tshift < 0)) || (tshift_old < 0 && tshift > 0))
            {
                tshift = 0;
            }
            if (CanChangeTShift(tshift))
            {
                f(tshift);
            }
        }

        void ChangeShiftScreen(TimeMeterMS *tMeter, void(*f)(int), int16 relStep)
        {
            int count = CalculateCount(tMeter);

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

            LOG_WRITE("%d", SHIFT_IN_MEMORY);
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
            static TimeMeterMS tMeter;

            if (!FPGA::IsRunning() || SET_TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY)
            {
                if (!SET_ENUM_POINTS_IS_281)
                {
                    ChangeShiftScreen(&tMeter, ShiftScreen, 2 * delta);
                }
            }
            else
            {
                ChangeTShift(&tMeter, SetTShift, (int16)delta);
            }
        }
    }


    static void OnKeyboardEvent(KeyboardEvent event)
    {
        if (event.IsDown())
        {
            Sound::ButtonPress();

            if (Hint::show)
            {
                Menu::ProcessButtonForHint(event.key);
            }
        }
        else if (event.IsUp() || event.IsLong())
        {
            Sound::ButtonRelease();
        }
        else if (event.IsLeft() || event.IsRight())
        {
            if (event.key == Key::RShiftA || event.key == Key::RShiftB || event.key == Key::TShift || event.key == Key::TrigLev)
            {
                Sound::RegulatorShiftRotate();
            }
            else
            {
                if (event.key != Key::Setting)
                {
                    Sound::RegulatorSwitchRotate();
                }
            }
        }
    }


    static void FuncNone(Action)
    {

    }

    static void OnFunctionalKey(Action action, Key::E key)
    {
        if (action.IsDown())
        {
            Item::now_pressed = ItemsUnderKey::Get(key);
        }
        else if (action.IsUp() || action.IsLong())
        {
            if (Menu::IsShown())
            {
                if (Hint::show)
                {
                    Hint::SetItem(ItemsUnderKey::Get(key));
                }
                else
                {
                    if (action.IsUp())
                    {
                        Menu::ExecuteFuncForShortPressOnItem(Item::now_pressed);
                    }
                    else if (action.IsLong())
                    {
                        Menu::ExecuteFuncForLongPressureOnItem(Item::now_pressed);
                    }

                    Item::now_pressed = nullptr;
                }
            }
        }
    }

    static void FuncF1(Action action)
    {
        OnFunctionalKey(action, Key::F1);
    }

    static void FuncF2(Action action)
    {
        OnFunctionalKey(action, Key::F2);
    }

    static void FuncF3(Action action)
    {
        OnFunctionalKey(action, Key::F3);
    }

    static void FuncF4(Action action)
    {
        OnFunctionalKey(action, Key::F4);
    }

    static void FuncF5(Action action)
    {
        OnFunctionalKey(action, Key::F5);
    }

    static void FuncChannelA(Action action)
    {
        if (action.IsUp())
        {
            if (Item::Opened() == PageChannelA::self)
            {
                SET_ENABLED_A = !SET_ENABLED_A;
                PageChannelA::OnChanged_Input(true);
            }
            else
            {
                PageChannelA::self->SetCurrent(true);
                PageChannelA::self->Open(true);
                Menu::Show(true);
            }
        }
        else if (action.IsLong())
        {
            Menu::SetAutoHide(true);
            Flags::needFinishDraw = true;

            RShift::Set(Chan::A, RShift::ZERO);
        }
    }

    static void FuncChannelB(Action action)
    {
        if (action.IsUp())
        {
            if (Item::Opened() == PageChannelB::self)
            {
                SET_ENABLED_B = !SET_ENABLED_B;
                PageChannelB::OnChanged_Input(true);
            }
            else
            {
                PageChannelB::self->SetCurrent(true);
                PageChannelB::self->Open(true);
                Menu::Show(true);
            }
        }
        else if (action.IsLong())
        {
            Menu::SetAutoHide(true);
            Flags::needFinishDraw = true;

            RShift::Set(Chan::B, RShift::ZERO);
        }
    }

    static void FuncTime(Action action)
    {
        if (action.IsUp())
        {
            PageTime::self->SetCurrent(true);
            PageTime::self->Open(true);
            Menu::Show(true);
        }
        else if (action.IsLong())
        {
            Menu::SetAutoHide(true);
            Flags::needFinishDraw = true;

            TShift::Set(0);
        }
    }

    static void FuncTrig(Action action)
    {
        if (action.IsUp())
        {
            PageTrig::self->SetCurrent(true);
            PageTrig::self->Open(true);
            Menu::Show(true);
        }
        else if (action.IsLong())
        {
            Flags::needFinishDraw = true;
            Menu::SetAutoHide(true);

            if (MODE_LONG_PRESS_TRIG_IS_LEVEL_ZERO)
            {
                TrigLev::Set(TRIG_SOURCE, TrigLev::ZERO);
            }
            else
            {
                FPGA::FindAndSetTrigLevel();
            }
        }
    }

    static void FuncCursors(Action action)
    {
        if (action.IsUp())
        {
            PageCursors::self->SetCurrent(true);
            PageCursors::self->Open(true);
            Menu::Show(true);
        }
    }

    static void FuncMeasures(Action action)
    {
        if (action.IsUp())
        {
            PageMeasures::self->SetCurrent(true);
            PageMeasures::self->Open(true);
            Menu::Show(true);
        }
    }

    static void FuncDisplay(Action action)
    {
        if (action.IsUp())
        {
            PageDisplay::self->SetCurrent(true);
            PageDisplay::self->Open(true);
            Menu::Show(true);
        }

    }

    static void FuncHelp(Action action)
    {
//        Hint::show = !Hint::show;
//        Hint::string = nullptr;
//        Hint::item = nullptr;

        if (action.IsUp())
        {
            if (Hint::show)
            {

            }
            else
            {
                Hint::show = !Hint::show;
                Hint::string = nullptr;
                Hint::item = nullptr;
            }
        }
    }

    static void FuncStart(Action action)
    {
        if (action.IsDown())
        {
            if (MODE_WORK_IS_DIRECT)
            {
                FPGA::OnPressStartStop();
            }
        }
    }

    static void FuncMemory(Action action)
    {
        if (action.IsUp())
        {
            if (MODE_BTN_MEMORY_IS_SAVE && FDrive::isConnected)
            {
                PageMemory::SetName::exitTo = Menu::IsShown() ? RETURN_TO_MAIN_MENU : RETURN_TO_DISABLE_MENU;
                PageMemory::SaveSignalToFlashDrive();
            }
            else
            {
                PageMemory::self->SetCurrent(true);
                PageMemory::self->Open(true);
                Menu::Show(true);
            }
        }
    }

    static void FuncService(Action action)
    {
        if (action.IsUp())
        {
            PageService::self->SetCurrent(true);
            PageService::self->Open(true);
            Menu::Show(true);
        }
    }

    static void FuncMenu(Action action)
    {
        if (action.IsUp())
        {
            if (!Menu::IsShown())
            {
                Menu::Show(true);
            }
            else
            {
                Item::CloseOpened();
            }
        }
        else if (action.IsLong())
        {
            Menu::SetAutoHide(true);
            Flags::needFinishDraw = true;

            Menu::Show(!Menu::IsShown());
        }
    }

    static void FuncPower(Action action)
    {
        if (action.IsDown())
        {
            ((Page *)Item::Opened())->ShortPressOnItem(0);
            Settings::SaveBeforePowerDown();

            if (TIME_MS > 1000)
            {
                Panel::TransmitData(0x05);
            }
        }
    }

    static void FuncRangeA(Action action)
    {
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
            static TimeMeterMS tMeter;
            Processing::ChangeRShift(&tMeter, Processing::SetRShift, Chan::A, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeRShift(&tMeter, Processing::SetRShift, Chan::A, +RShift::STEP);
        }
    }

    static void FuncRShiftB(Action action)
    {
        if (action.IsLeft())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeRShift(&tMeter, Processing::SetRShift, Chan::B, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeRShift(&tMeter, Processing::SetRShift, Chan::B, +RShift::STEP);
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
            static TimeMeterMS tMeter;
            Processing::ChangeTrigLev(&tMeter, Processing::SetTrigLev, TRIG_SOURCE, -RShift::STEP);
        }
        else if (action.IsRight())
        {
            static TimeMeterMS tMeter;
            Processing::ChangeTrigLev(&tMeter, Processing::SetTrigLev, TRIG_SOURCE, +RShift::STEP);
        }
    }

    static void FuncSetting(Action action)
    { 
        static const int step = 3;
        static int angle = 0;

        if (!Hint::show)
        {
            if (action.IsLeft() || action.IsRight())
            {
                angle = action.IsLeft() ? (angle - 1) : (angle + 1);

                if (angle != 0)
                {
                    if (Menu::IsShown() || !Item::Opened()->IsPage())
                    {
                        Item *item = Item::Current();
                        TypeItem::E type = item->GetType();

                        if (Item::Opened()->IsPage() && (type == TypeItem::ChoiceReg ||
                            type == TypeItem::Governor || type == TypeItem::IP || type == TypeItem::MAC))
                        {
                            if (angle > step || angle < -step)
                            {
                                Flags::needFinishDraw = true;
                                item->Change(angle);
                                Sound::RegulatorSwitchRotate();
                                angle = 0;
                            }
                        }
                        else
                        {
                            item = Item::Opened();
                            type = item->GetType();

                            if (Menu::IsMinimize())
                            {
                                Flags::needFinishDraw = true;
                                Page::RotateRegSetSB(angle);
                                Sound::RegulatorSwitchRotate();
                            }
                            else if (type == TypeItem::Page || type == TypeItem::IP || type == TypeItem::MAC ||
                                type == TypeItem::Choice || type == TypeItem::ChoiceReg || type == TypeItem::Governor)
                            {
                                if (angle > step || angle < -step)
                                {
                                    item->ChangeOpened(angle);
                                    Sound::RegulatorSwitchRotate();
                                    Flags::needFinishDraw = true;
                                    angle = 0;
                                }
                            }
                            else if (type == TypeItem::GovernorColor)
                            {
                                Flags::needFinishDraw = true;
                                item->Change(angle);
                                Sound::RegulatorSwitchRotate();
                                angle = 0;
                            }
                            else if (type == TypeItem::Time)
                            {
                                Flags::needFinishDraw = true;
                                (angle > 0) ? ((Time *)item)->IncCurrentPosition() : ((Time *)item)->DecCurrentPosition();
                                Sound::RegulatorSwitchRotate();
                            }
                        }
                    }
                }
            }
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


void Panel::ProcessEvent(KeyboardEvent event)
{
    DEBUG_POINT_0;

    if (!isRunning)
    {
        if (event.IsDown())
        {
            pressedButton = event.key;
        }

        return;
    }

    DEBUG_POINT_0;

    OnKeyboardEvent(event);

    DEBUG_POINT_0;

    funcOnKey[event.key](event.action);

    DEBUG_POINT_0;

    Flags::needFinishDraw = true;

    DEBUG_POINT_0;
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


void Panel::Callback::OnReceiveSPI5(const uint8 *data, uint)
{
    KeyboardEvent event(data);

    if (event.key != Key::None)
    {
        if (input_buffer.mutex.Locked())
        {
            aux_buffer.Push(event);
        }
        else
        {
            input_buffer.Push(event);
        }

        Settings::NeedSave();
        timeLastEvent = TIME_MS;
    }
}


uint Panel::TimePassedAfterLastEvent()
{
    return TIME_MS - timeLastEvent;
}


void Panel::Update()
{
    DEBUG_POINT_0;

    input_buffer.mutex.Lock();

    DEBUG_POINT_0;

    while (!aux_buffer.Empty())
    {
        DEBUG_POINT_0;

        input_buffer.Push(aux_buffer.Back());

        DEBUG_POINT_0;
    }

    DEBUG_POINT_0;

    while (!input_buffer.Empty())
    {
        DEBUG_POINT_0;

        KeyboardEvent event = input_buffer.Back();

        DEBUG_POINT_0;

        PasswordResolver::ProcessEvent(event);

        DEBUG_POINT_0;

        ProcessEvent(event);

        DEBUG_POINT_0;
    }

    DEBUG_POINT_0;

    input_buffer.mutex.Unlock();
}
