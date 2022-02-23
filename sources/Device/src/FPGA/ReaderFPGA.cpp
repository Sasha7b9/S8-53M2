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
    int shift = TBase::InRandomizeMode() ?
        ENUM_POINTS_FPGA::ToNumBytes() / TBase::StepRand() :
        ENUM_POINTS_FPGA::ToNumBytes();

    int result = HAL_FMC::Read(RD_ADDR_LAST_RECORD) - shift;

    return (uint16)(result + FPGA::Launch::DeltaReadAddress() - 1);
}
