// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/HAL/HAL.h"
#include "SCPI/SCPI.h"
#include "FPGA/FPGA.h"
#include "Menu/Pages/Definition.h"
#include "Panel/Panel.h"


pchar SCPI::COMMON::IDN(pchar buffer)
{
    SCPI::SendFormat("MNIPI, %s, V%s, %X", NAME_MODEL_EN, NUMBER_VER, HAL::CalculateCRC32());

    return buffer;
}


pchar SCPI::COMMON::RUN(pchar buffer)
{
    if (!FPGA::IsRunning())
    {
        FPGA::Start();
    }

    return buffer;
}


pchar SCPI::COMMON::STOP(pchar buffer)
{
    if (FPGA::IsRunning())
    {
        FPGA::Stop();
    }

    return buffer;
}


pchar SCPI::COMMON::RESET(pchar buffer)
{
    PageService::ResetSettings(false);

    return buffer;
}


pchar SCPI::COMMON::AUTOSCALE(pchar buffer)
{
    PageService::OnPress_AutoSearch();

    return buffer;
}


pchar SCPI::COMMON::REQUEST(pchar buffer)
{
    SCPI::SendFormat("S8-53/1");

    return buffer;
}
