// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "SettingsTime.h"
#include "Settings.h"
#include "Log.h"
#include "Utils/Math.h"
#include "FPGA/FPGA.h"


void sTime_SetTShift(int16 shift)
{
    TSHIFT = shift;
    FPGA::ClearData();
}


bool sTime_RandomizeModeEnabled()
{
    return SET_TBASE < TBase::_100ns;
}
