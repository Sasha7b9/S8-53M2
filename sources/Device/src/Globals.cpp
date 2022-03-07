// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Globals.h"
#include "Display/DisplayTypes.h"
#include "Menu/MenuItems.h"


void *extraMEM = 0;


StateFPGA gStateFPGA = 
{
    false,
    StateWorkFPGA::Stop,
    StateCalibration_None
};


int transmitBytes = 0;
