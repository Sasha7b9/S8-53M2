// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Panel.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "FPGA/FPGA.h"
#include "Menu/Menu.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Math.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"
#include <stm32f4xx_hal.h>


static const uint MIN_TIME = 500;


static void ChangeRShift(int *prevTime, void(*f)(Chan::E, int16), Chan::E ch, int16 relStep);


void HelpLong()
{
    SHOW_HELP_HINTS++;
    gStringForHint = 0;
    gItemHint = 0;
}

void ChannelALong()
{
    Menu::Handlers::LongPressureButton(B_ChannelA);
}

void ChannelBLong()
{
    Menu::Handlers::LongPressureButton(B_ChannelB);
}

void TimeLong()
{
    Menu::Handlers::LongPressureButton(B_Time);
}

void TrigLong()
{
    if (MODE_LONG_PRESS_TRIG_IS_LEVEL0)
    {
        Menu::Handlers::LongPressureButton(B_Trig);
    }
    else
    {
        FPGA::FindAndSetTrigLevel();
    }
}

void StartDown()                        // B_Start
{
    if (MODE_WORK_IS_DIRECT)
    {
        Menu::Handlers::PressButton(B_Start);
    }
}

void PowerDown()                        // B_Power
{
    Menu::ShortPressOnPageItem((Page *)Menu::OpenedItem(), 0);
    Settings::Save();
    Log_DisconnectLoggerUSB();

    if (gTimerMS > 1000)
    {
        Panel::TransmitData(0x05);
    }
}

void MenuLong() 
{
    Menu::Handlers::LongPressureButton(B_Menu);
}

void F1Long()
{
    Menu::Handlers::LongPressureButton(B_F1);
}

void F2Long()
{
    Menu::Handlers::LongPressureButton(B_F2);
}

void F3Long()
{
    Menu::Handlers::LongPressureButton(B_F3);
}

void F4Long()
{
    Menu::Handlers::LongPressureButton(B_F4);
}

void F5Long()
{
    Menu::Handlers::LongPressureButton(B_F5);
}


int CalculateCount(int *prevTime)
{
    uint time = gTimerMS;
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
        time = gTimerMS;
        return true;
    }
    else if (time == 0)
    {
        return true;
    }
    else if (gTimerMS - time > MIN_TIME)
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
        time[ch] = gTimerMS;
        return true;
    }
    else if (time[ch] == 0)
    {
        return true;
    }
    else if (gTimerMS - time[ch] > MIN_TIME)
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
    int tShiftOld = TSHIFT;
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

    int16 tShift = TSHIFT + step;
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

void RShift0Left()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, Chan::A, -RShift::STEP);
}

void RShift0Right()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, Chan::A, +RShift::STEP);
}

void RShift1Left()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, Chan::B, -RShift::STEP);
}

void RShift1Right()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, Chan::B, +RShift::STEP);
}

static void SetTrigLev(TrigSource::E ch, int16 trigLev)
{
    TrigLev::Set(ch, trigLev);
}

void TrigLevLeft()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, SetTrigLev, TRIG_SOURCE, -RShift::STEP);
}

void TrigLevRight()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, SetTrigLev, TRIG_SOURCE, +RShift::STEP);
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
    if (!FPGA::IsRunning() || TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY)
    {
        if (!ENUM_POINTS_IS_281)
        {
            ChangeShiftScreen(&prevTime, ShiftScreen, 2 * delta);
        }
    }
    else
    {
        ChangeTShift(&prevTime, SetTShift, (int16)delta);
    }
}

void TShiftLeft()
{
    XShift(-1);
}

void TShiftRight()
{
    XShift(1);
}

void Range0Left()
{
    Sound::RegulatorSwitchRotate();
    Range::Increase(Chan::A);
}

void Range0Right()
{
    Sound::RegulatorSwitchRotate();
    Range::Decrease(Chan::A);
}

void Range1Left()
{
    Sound::RegulatorSwitchRotate();
    Range::Increase(Chan::B);
}

void Range1Right()
{
    Sound::RegulatorSwitchRotate();
    Range::Decrease(Chan::B);
}

void TBaseLeft()
{
    Sound::RegulatorSwitchRotate();
    TBase::Increase();
}

void TBaseRight()
{
    Sound::RegulatorSwitchRotate();
    TBase::Decrease();
}

void SetLeft()
{
    Menu::Handlers::RotateRegSetLeft();
}

void SetRight()
{
    Menu::Handlers::RotateRegSetRight();
}
