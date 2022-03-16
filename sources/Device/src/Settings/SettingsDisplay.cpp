// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Settings/SettingsDisplay.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"


int Smoothing::ToPoints()
{
    if (SMOOTHING_IS_DISABLE)
    {
        return 0;
    }
    return (int)SMOOTHING + 1;          // WARN ����� ����� ������ �����������
}


BitSet32 SettingsDisplay::PointsOnDisplay()
{
    BitSet32 result;

    int numPointsOnScreen = 281;
    result.half_iword[0] = SHIFT_IN_MEMORY;
    result.half_iword[1] = SHIFT_IN_MEMORY + numPointsOnScreen - 1;

    return result;
}


int SettingsDisplay::TimeMenuAutoHide()
{
    static const int times[] =
    {
        0, 5, 10, 15, 30, 60
    };

    return times[MENU_AUTO_HIDE] * 1000;
}


bool SettingsDisplay::IsSeparate()
{
    return (!DISABLED_DRAW_MATH && MODE_DRAW_MATH_IS_SEPARATE) || ENABLED_FFT;
}


ModeAveraging::E ModeAveraging::Current()
{
    if (TBase::InModeRandomizer())
    {
        return ModeAveraging::Around;
    }

    return MODE_AVE;
}


int SettingsDisplay::GetNumAverages()
{
    if (TBase::InModeRandomizer() && (NUM_AVE_FOR_RAND >= NUM_AVE))
    {
        return NUM_AVE_FOR_RAND;
    }

    return NUM_AVE;
}


ENumSignalsInSec::E ENumSignalsInSec::FromNum(int num)
{
    if (num == 1)
    {
        return ENumSignalsInSec::_1;
    }
    else if (num == 2)
    {
        return ENumSignalsInSec::_2;
    }
    else if (num == 5)
    {
        return ENumSignalsInSec::_5;
    }
    else if (num == 10)
    {
        return ENumSignalsInSec::_10;
    }
    else if (num == 25)
    {
        return ENumSignalsInSec::_25;
    }

    LOG_ERROR_TRACE("����� �������� � ������� ����� %d", num);

    return ENumSignalsInSec::_1;
}


int ENumSignalsInSec::ToNum(ENumSignalsInSec::E enumSignalsInSec)
{
    static const int fps[] = {25, 10, 5, 2, 1};

    if (enumSignalsInSec <= ENumSignalsInSec::_1)
    {
        return fps[enumSignalsInSec];
    }

    LOG_ERROR_TRACE("������������ �������� %d", (int)enumSignalsInSec);

    return 1;
}
