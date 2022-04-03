// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/HAL/HAL.h"
#include "device.h"
#include "SCPI/SCPI.h"


pchar SCPI::COMMON::IDN(pchar buffer)
{
    SCPI::SendFormat("MNIPI, %s, V%s", NAME_MODEL_EN, NUMBER_VER);

    return buffer;
}


pchar SCPI::COMMON::RUN(pchar buffer)
{
    return buffer;
}


pchar SCPI::COMMON::STOP(pchar buffer)
{
    return buffer;
}


pchar SCPI::COMMON::RESET(pchar buffer)
{
    return buffer;
}


pchar SCPI::COMMON::AUTOSCALE(pchar buffer)
{
    return buffer;
}


pchar SCPI::COMMON::REQUEST(pchar buffer)
{
    if (*buffer == 0)
    {
        SCPI::SendFormat("S8-53/1");
    }

    return buffer;
}
