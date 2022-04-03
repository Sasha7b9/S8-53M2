// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/HAL/HAL.h"
#include "device.h"
#include "SCPI/SCPI.h"


bool SCPI::COMMON::IDN(pchar)
{
    SCPI::SendFormat("MNIPI, %s, V%s", NAME_MODEL_EN, NUMBER_VER);

    return true;
}


bool SCPI::COMMON::RUN(pchar)
{
    return true;
}


bool SCPI::COMMON::STOP(pchar)
{
    return true;
}


bool SCPI::COMMON::RESET(pchar)
{
    return true;
}


bool SCPI::COMMON::AUTOSCALE(pchar)
{
    return true;
}


bool SCPI::COMMON::REQUEST(pchar)
{
    SCPI::SendFormat("S8-53/1");

    return true;
}
