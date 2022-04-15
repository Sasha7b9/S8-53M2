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
    if (buffer == nullptr)
    {
        SCPI::SendFormat("MNIPI, %s, V%s, %X", NAME_MODEL_EN, NUMBER_VER, HAL::CalculateCRC32());
    }

    return buffer;
}


pchar SCPI::COMMON::RUN(pchar buffer)
{
    if (buffer == nullptr)
    {
        if (!FPGA::IsRunning())
        {
            FPGA::Start();
        }
    }

    return buffer;
}


pchar SCPI::COMMON::STOP(pchar buffer)
{
    if (buffer == nullptr)
    {
        if (FPGA::IsRunning())
        {
            FPGA::Stop();
        }
    }

    return buffer;
}


pchar SCPI::COMMON::RESET(pchar buffer)
{
    if (buffer == nullptr)
    {
        PageService::ResetSettings(false);
    }

    return buffer;
}


bool SCPI::COMMON::AUTOSCALE(pchar)
{
    PageService::OnPress_AutoSearch();

    return true;
}


bool SCPI::COMMON::REQUEST(pchar)
{
    SCPI::SendFormat("S8-53/1");

    return true;
}
