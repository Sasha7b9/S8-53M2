// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "SettingsMemory.h"
#include "Settings.h"
#include "FPGA/TypesFPGA_old.h"
#include "FPGA/FPGA.h"


int sMemory_GetNumPoints(bool forCalculate)
{
    static const int numPoints[3][3] =
    {
        {281, 512, 1024},
        {281, 512, 512},
        {281, 512, 512}
    };

    if (ENUM_POINTS_IS_1024 && forCalculate)
    {
        return FPGA::MAX_POINTS_FOR_CALCULATE;
    }

    return numPoints[PEAKDET][ENUM_POINTS];
}


ENUM_POINTS_FPGA::E sMemory_IntNumPoints2FPGA_NUM_POINTS(int numPoints)
{
    if (numPoints == 1024)
    {
        return ENUM_POINTS_FPGA::_1024;
    }
    else if (numPoints == 512)
    {
        return ENUM_POINTS_FPGA::_512;
    }
    return ENUM_POINTS_FPGA::_281;
}
