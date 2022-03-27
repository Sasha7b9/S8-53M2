#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Settings/Settings.h"
#include "FPGA/FPGA.h"
#include "Menu/Menu.h"



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


void PageMemory::Resolver::OnPress_MemoryInternalEnter()
{
    PageMemory::Internal::self->OpenAndSetCurrent();
    MODE_WORK = ModeWork::MemInt;
}


void PageMemory::Resolver::OnPress_MemoryInternalExit()
{
    if (PageMemory::Internal::exitToLast)
    {
        PageMemory::Latest::self->OpenAndSetCurrent();
        MODE_WORK = ModeWork::Latest;
        PageMemory::Internal::exitToLast = false;
        Menu::needClosePageSB = false;
    }
    else
    {
        MODE_WORK = ModeWork::Direct;
        Page::FromName(NamePage::SB_MemInt)->ShortPressOnItem(0);
    }
}

