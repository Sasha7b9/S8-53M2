// 2022/02/22 16:16:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"


namespace FPGA
{
    namespace Reader
    {
        // ¬озвращает адрес, с которого нужно читать первую точку
        uint16 CalculateAddressRead();
    }

    // —труктура дл€ управлени€ пред/после- запусками
    namespace LaunchFPGA
    {
        static const int8 d_read[TBase::Count] =   // ƒополнительное смещение дл€ чтени€ адреса
        {// 2    5   10   20   50  100  200
            0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

        // Ќа сколько мен€ть адрес первого чтени€
        int DeltaReadAddress();
    }
}


uint16 FPGA::Reader::CalculateAddressRead()
{
    int shift = TBase::InRandomizeMode() ?
        ENUM_POINTS_FPGA::ToNumBytes() / TBase::StepRand() :
        ENUM_POINTS_FPGA::ToNumBytes() / 2;

    int result = HAL_FMC::Read(RD_ADDR_LAST_RECORD) - shift;

    return (uint16)(result + LaunchFPGA::DeltaReadAddress() - 1);
}


int FPGA::LaunchFPGA::DeltaReadAddress()
{
    int result = 0;

    if (TShift::ForLaunchFPGA() < 0)
    {
        result += TShift::ForLaunchFPGA();
    }

    return result - d_read[SET_TBASE];
}
