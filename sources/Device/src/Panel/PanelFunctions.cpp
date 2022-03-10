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
