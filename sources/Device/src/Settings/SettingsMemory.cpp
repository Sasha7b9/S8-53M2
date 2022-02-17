// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "SettingsMemory.h"
#include "Settings.h"
#include "FPGA/TypesFPGA_old.h"
#include "FPGA/FPGA.h"




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
