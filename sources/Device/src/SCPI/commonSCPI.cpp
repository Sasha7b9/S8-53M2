// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/HAL/HAL.h"
#include "SCPI/SCPI.h"
#include "FPGA/FPGA.h"
#include "Menu/Pages/Definition.h"
#include "Panel/Panel.h"


bool SCPI::COMMON::IDN(pchar)
{
    SCPI::SendFormat("MNIPI, %s, V%s", NAME_MODEL_EN, NUMBER_VER);

    return true;
}


bool SCPI::COMMON::RUN(pchar)
{
    if (!FPGA::IsRunning())
    {
        FPGA::Start();
    }

    return true;
}


bool SCPI::COMMON::STOP(pchar)
{
    if (FPGA::IsRunning())
    {
        FPGA::Stop();
    }

    return true;
}


bool SCPI::COMMON::RESET(pchar)
{
    PageService::OnPress_ResetSettings();

    uint8 buffer[3] = {0xFF, Key::Start, Action::Down };

    Panel::Callback::OnReceiveSPI5(buffer, 3);

    buffer[2] = Action::Up;

    Panel::Callback::OnReceiveSPI5(buffer, 3);

    return true;
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
