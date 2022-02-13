// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "SettingsMemory.h"
#include "Settings.h"
#include "FPGA/TypesFPGA.h"


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
        return FPGA_MAX_POINTS_FOR_CALCULATE;
    }

    return numPoints[PEAKDET][ENUM_POINTS];
}


ENUM_POINTS_FPGA sMemory_IntNumPoints2FPGA_NUM_POINTS(int numPoints)
{
    if (numPoints == 1024)
    {
        return FNP_1024;
    }
    else if (numPoints == 512)
    {
        return FNP_512;
    }
    return FNP_281;
}
