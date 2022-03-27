#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Settings/Settings.h"
#include "FPGA/FPGA.h"



void PageMemory::Resolver::OnPress_MemoryLatestEnter()
{
    PageMemory::Latest::current = 0;
    FPGA::runningBeforeSmallButtons = FPGA::IsRunning();
    FPGA::Stop();
    MODE_WORK = ModeWork::Latest;
}


void PageMemory::Resolver::OnPress_MemoryLatestExit()
{
    MODE_WORK = ModeWork::Direct;

    if (FPGA::runningBeforeSmallButtons)
    {
        FPGA::Start();
        FPGA::runningBeforeSmallButtons = false;
    }

    Display::RemoveAddDrawFunction();
}

