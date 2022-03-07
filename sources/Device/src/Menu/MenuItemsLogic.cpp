// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Menu.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Settings/Settings.h"
#include "Hardware/Sound.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


void Governor::StartChange(int delta)
{
    Sound::GovernorChangedValue();

    if (delta > 0 && ADDRESS_GOVERNOR == (uint)this && inMoveIncrease)
    {
        *cell = NextValue();
    }
    else if (delta < 0 && ADDRESS_GOVERNOR == (uint)this && IN_MOVE_DECREASE)
    {
        *cell = PrevValue();
    }
    else
    {
        TIME_START_MS = TIME_MS;
    }

    inMoveIncrease = delta > 0;
    IN_MOVE_DECREASE = (delta < 0) ? 1U : 0U;
}

void Governor::ChangeValue(int delta)
{
    int16 oldValue = *cell;
    LIMITATION(*cell, (int16)(oldValue + Math::Sign(delta) * Math::Pow10(Governor::cur_digit)), (int16)minValue, (int16)maxValue);

    if (*cell != oldValue)
    {
        if (funcOfChanged)
        {
            funcOfChanged();
        }

        Sound::GovernorChangedValue();
    }
}

void IPaddress::ChangeValue(int delta)
{
    int numByte = 0;
    int numPos = 0;

    GetNumPosIPvalue(&numByte, &numPos);

    int oldValue = 0;

    if (numByte < 4)
    {
        uint8 *bytes = ip0;
        oldValue = bytes[numByte];
    }
    else
    {
        oldValue = *port;
    }

    int newValue = oldValue + Math::Sign(delta) * Math::Pow10(numPos);
    LIMITATION(newValue, newValue, 0, numByte == 4 ? 65535 : 255);

    if (oldValue != newValue)
    {
        if (numByte == 4)
        {
            *port = (uint16)newValue;
        }
        else
        {
            ip0[numByte] = (uint8)newValue;
        }
        Sound::GovernorChangedValue();
        Display::ShowWarningGood(Warning::NeedRebootDevice2);
        Display::ShowWarningGood(Warning::NeedRebootDevice1);
    }
}

void MACaddress::ChangeValue(int delta)
{
    uint8 *value = mac0 + MACaddress::cur_digit;
    *value += delta > 0 ? 1 : -1;
    Sound::GovernorChangedValue();
    Display::ShowWarningGood(Warning::NeedRebootDevice2);
    Display::ShowWarningGood(Warning::NeedRebootDevice1);
}

void IPaddress::GetNumPosIPvalue(int *numIP, int *selPos)
{
    if (IPaddress::cur_digit < 12)
    {
        *numIP = IPaddress::cur_digit / 3;
        *selPos = 2 - (IPaddress::cur_digit - (*numIP * 3));
    }
    else
    {
        *numIP = 4;
        *selPos = 4 - (IPaddress::cur_digit - 12);
    }


}

float Governor::Step()
{
    static const float speed = 0.05f;
    static const int numLines = 10;

    if (ADDRESS_GOVERNOR == (uint)this && IN_MOVE_DECREASE)
    {
        float delta = -speed * (TIME_MS - TIME_START_MS);

        if (delta == 0.0f)
        {
            return -0.001f;
        }

        if (delta < -numLines)
        {
            IN_MOVE_DECREASE = inMoveIncrease = false;
            *cell = PrevValue();

            if (funcOfChanged)
            {
                funcOfChanged();
            }

            return 0.0f;
        }

        return delta;
    }

    if (ADDRESS_GOVERNOR == (uint)this && inMoveIncrease)
    {
        float delta = speed * (TIME_MS - TIME_START_MS);

        if (delta == 0.0f)
        {
            return 0.001f;
        }

        if (delta > numLines)
        {
            IN_MOVE_DECREASE = inMoveIncrease = false;
            *cell = NextValue();

            if(funcOfChanged)
            {
                funcOfChanged();
            }

            return 0.0f;
        }

        return delta;
    }

    return 0.0f;
}

int16 Governor::NextValue() const
{
    return ((*cell) < maxValue) ? (*cell) + 1 : minValue;
}

int16 Governor::PrevValue() const
{
    return ((*cell) > minValue) ? (*cell) - 1 : maxValue;
}

void Governor::NextPosition()
{
    if (Opened() == this)
    {
        CircleIncreaseInt8(&Governor::cur_digit, 0, NumDigits() - 1);
    }
}

int Governor::NumDigits() const
{
    int min = Math_NumDigitsInNumber(Math_FabsInt(minValue));
    int max = Math_NumDigitsInNumber(Math_FabsInt(maxValue));
    if (min > max)
    {
        max = min;
    }
    return max;
}

void IPaddress::NextPosition() const
{
    CircleIncreaseInt8(&IPaddress::cur_digit, 0, port == 0 ? 11 : 16);
}

void Time::SetOpened()
{
    PackedTime time = HAL_RTC::GetPackedTime();
    *(seconds) = (int8)time.seconds;
    *(minutes) = (int8)time.minutes;
    *(hours) = (int8)time.hours;
    *(day) = (int8)time.day;
    *(month) = (int8)time.month;
    *(year) = (int8)time.year ;
}

void Time::SetNewTime() const
{
    HAL_RTC::SetTimeAndData(*day, *month, *year, *hours, *minutes, *seconds);
}

void Time::SelectNextPosition()
{
    CircleIncreaseInt8(curField, 0, 7);
    Color::ResetFlash();
}

void Time::IncCurrentPosition() const
{
    Sound::GovernorChangedValue();
    int8 position = *curField;

    if (position != iSET && position != iEXIT)
    {
        static const int8 max[] = {0, 31, 12, 99, 23, 59, 59};
        static const int8 min[] = {0, 1, 1, 15, 0, 0, 0};

        int8* value[] = { 0, day, month, year, hours, minutes, seconds };
        *(value[position]) = (*(value[position]))++; //-V567

        if (*value[position] > max[position])
        {
            *value[position] = min[position];
        }
    }
}

void Time::DecCurrentPosition() const
{
    Sound::GovernorChangedValue();
    static const int8 max[] = {0, 31, 12, 99, 23, 59, 59};
    static const int8 min[] = {0, 1, 1, 15, 0, 0, 0};
    int8 position = *curField;

    if (position != iSET && position != iEXIT)
    {
        int8* value[] = { 0, day, month, year, hours, minutes, seconds };
        (*(value[position]))--;

        if (*value[position] < min[position])
        {
            *value[position] = max[position];
        }
    }
}

void GovernorColor::ChangeValue(int delta)
{
    ColorType *ct = colorType;

    if (ct->currentField == 0)
    {
        ct->BrightnessChange(delta);
    }
    else
    {
        ct->ComponentChange(delta);
    }
}


int8 Page::PosCurrentItem() const
{
    return POS_ACT_ITEM(name) & 0x7f;
}
