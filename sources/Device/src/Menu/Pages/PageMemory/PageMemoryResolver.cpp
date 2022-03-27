#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Settings/Settings.h"
#include "FPGA/FPGA.h"


void PageMemory::Resolver::OnPress_ExitMemoryLatest()
{
    MODE_WORK = ModeWork::Direct;

    if (FPGA::runningBeforeSmallButtons)
    {
        FPGA::Start();
        FPGA::runningBeforeSmallButtons = false;
    }

    Display::RemoveAddDrawFunction();
}

