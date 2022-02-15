// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Menu.h"
#include "Menu/MenuItemsLogic.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Settings/Settings.h"
#include "Hardware/Sound.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


int8 gCurDigit = 0;


void Governor::StartChange(int delta)
{
    Sound::GovernorChangedValue();

    if (delta > 0 && ADDRESS_GOVERNOR == (uint)this && IN_MOVE_INCREASE)
    {
        *cell = NextValue();
    }
    else if (delta < 0 && ADDRESS_GOVERNOR == (uint)this && IN_MOVE_DECREASE)
    {
        *cell = PrevValue();
    }
    else
    {
        TIME_START_MS = gTimerMS;   
    }

    IN_MOVE_INCREASE = (delta > 0) ? 1U : 0U;
    IN_MOVE_DECREASE = (delta < 0) ? 1U : 0U;
}

void Governor::ChangeValue(int delta)
{
    int16 oldValue = *cell;
    LIMITATION(*cell, (int16)(oldValue + Math_Sign(delta) * Math_Pow10(gCurDigit)), (int16)minValue, (int16)maxValue);

    if (*cell != oldValue)
    {
        if (funcOfChanged)
        {
            funcOfChanged();
        }

        Sound::GovernorChangedValue();
    }
}

void IPaddress_ChangeValue(IPaddress *ip, int delta)
{
    int numByte = 0;
    int numPos = 0;

    IPaddress_GetNumPosIPvalue(&numByte, &numPos);

    int oldValue = 0;

    if (numByte < 4)
    {
        uint8 *bytes = ip->ip0;
        oldValue = bytes[numByte];
    }
    else
    {
        oldValue = *ip->port;
    }

    int newValue = oldValue + Math_Sign(delta) * Math_Pow10(numPos);
    LIMITATION(newValue, newValue, 0, numByte == 4 ? 65535 : 255);

    if (oldValue != newValue)
    {
        if (numByte == 4)
        {
            *ip->port = (uint16)newValue;
        }
        else
        {
            ip->ip0[numByte] = (uint8)newValue;
        }
        Sound::GovernorChangedValue();
        Display::ShowWarningGood(NeedRebootDevice2);
        Display::ShowWarningGood(NeedRebootDevice1);
    }
}

void MACaddress_ChangeValue(MACaddress *mac, int delta)
{
    uint8 *value = mac->mac0 + gCurDigit;
    *value += delta > 0 ? 1 : -1;
    Sound::GovernorChangedValue();
    Display::ShowWarningGood(NeedRebootDevice2);
    Display::ShowWarningGood(NeedRebootDevice1);
}

void IPaddress_GetNumPosIPvalue(int *numIP, int *selPos)
{
    if (gCurDigit < 12)
    {
        *numIP = gCurDigit / 3;
        *selPos = 2 - (gCurDigit - (*numIP * 3));
    }
    else
    {
        *numIP = 4;
        *selPos = 4 - (gCurDigit - 12);
    }


}

float Governor::Step()
{
    static const float speed = 0.05f;
    static const int numLines = 10;

    if (ADDRESS_GOVERNOR == (uint)this && IN_MOVE_DECREASE)
    {
        float delta = -speed * (gTimerMS - TIME_START_MS);

        if (delta == 0.0f)
        {
            return -0.001f;
        }

        if (delta < -numLines)
        {
            IN_MOVE_DECREASE = IN_MOVE_INCREASE = 0;
            *cell = PrevValue();

            if (funcOfChanged)
            {
                funcOfChanged();
            }

            return 0.0f;
        }

        return delta;
    }

    if (ADDRESS_GOVERNOR == (uint)this && IN_MOVE_INCREASE)
    {
        float delta = speed * (gTimerMS - TIME_START_MS);

        if (delta == 0.0f)
        {
            return 0.001f;
        }

        if (delta > numLines)
        {
            IN_MOVE_DECREASE = IN_MOVE_INCREASE = 0;
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
    if (Menu::OpenedItem() == this)
    {
        CircleIncreaseInt8(&gCurDigit, 0, NumDigits() - 1);
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
    CircleIncreaseInt8(&gCurDigit, 0, port == 0 ? 11 : 16);
}

void ItemTime_SetOpened(Time *item)
{
    PackedTime time = HAL_RTC::GetPackedTime();
    *(item->seconds) = (int8)time.seconds;
    *(item->minutes) = (int8)time.minutes;
    *(item->hours) = (int8)time.hours;
    *(item->day) = (int8)time.day;
    *(item->month) = (int8)time.month;
    *(item->year) = (int8)time.year ;
}

void ItemTime_SetNewTime(const Time *time)
{
    HAL_RTC::SetTimeAndData(*time->day, *time->month, *time->year, *time->hours, *time->minutes, *time->seconds);
}

void ItemTime_SelectNextPosition(Time *time)
{
    CircleIncreaseInt8(time->curField, 0, 7);
    Color::ResetFlash();
}

void ItemTime_IncCurrentPosition(const Time *time)
{
    Sound::GovernorChangedValue();
    int8 position = *time->curField;

    if (position != iSET && position != iEXIT)
    {
        static const int8 max[] = {0, 31, 12, 99, 23, 59, 59};
        static const int8 min[] = {0, 1, 1, 15, 0, 0, 0};

        int8* value[] = { 0, time->day, time->month, time->year, time->hours, time->minutes, time->seconds };
        *(value[position]) = (*(value[position]))++; //-V567

        if (*value[position] > max[position])
        {
            *value[position] = min[position];
        }
    }
}

void ItemTime_DecCurrentPosition(const Time *time)
{
    Sound::GovernorChangedValue();
    static const int8 max[] = {0, 31, 12, 99, 23, 59, 59};
    static const int8 min[] = {0, 1, 1, 15, 0, 0, 0};
    int8 position = *time->curField;

    if (position != iSET && position != iEXIT)
    {
        int8* value[] = { 0, time->day, time->month, time->year, time->hours, time->minutes, time->seconds };
        (*(value[position]))--;

        if (*value[position] < min[position])
        {
            *value[position] = max[position];
        }
    }
}

void GovernorColor_ChangeValue(GovernorColor *governor, int delta)
{
    ColorType *ct = governor->colorType;
    if (ct->currentField == 0)
    {
        ct->BrightnessChange(delta);
    }
    else
    {
        ct->ComponentChange(delta);
    }
}

