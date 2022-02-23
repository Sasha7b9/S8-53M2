// 2022/02/22 16:16:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"


namespace FPGA
{
    namespace Reader
    {
        Mutex mutex_read;
    }
}


uint16 FPGA::Reader::CalculateAddressRead()
{
    return (uint16)(HAL_FMC::Read(RD_ADDR_LAST_RECORD) - ENUM_POINTS_FPGA::ToNumPoints());
}
