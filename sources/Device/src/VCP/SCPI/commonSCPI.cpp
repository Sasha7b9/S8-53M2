// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "SCPI.h"
#include "VCP/VCP.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"



void SCPI::COMMON::IDN(uint8 *)
{
    SCPI_SEND("MNIPI,S8-53/1,V%S,%X", NUM_VER, HAL::CalculateCRC32());
}



void SCPI::COMMON::RUN(uint8 *)
{

}



void SCPI::COMMON::STOP(uint8 *)
{

}



void SCPI::COMMON::RESET(uint8 *)
{
    Settings::needReset = true;
}



void SCPI::COMMON::AUTOSCALE(uint8 *)
{

}



void SCPI::COMMON::REQUEST(uint8 *)
{
    SCPI_SEND("S8-53/1");
}
