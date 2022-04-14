#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Settings/Settings.h"
#include "FPGA/FPGA.h"
#include "Menu/Menu.h"


static bool runningFPGAbeforeOpen;      // Здесь сохраняется информация о том, работала ли ПЛИС перед переходом
                                        // в режим работы с памятью

bool PageMemory::Resolver::exitFromIntToLast = false;


void PageMemory::Resolver::OnPress_MemoryLatestEnter()
{
    PageMemory::Latest::current = 0;
    runningFPGAbeforeOpen = FPGA::IsRunning();
    FPGA::Stop();
    MODE_WORK = ModeWork::Latest;
}


void PageMemory::Resolver::OnPress_MemoryLatestExit()
{
    MODE_WORK = ModeWork::Direct;

    if (runningFPGAbeforeOpen)
    {
        FPGA::Start();
        runningFPGAbeforeOpen = false;
    }
}


void PageMemory::Resolver::OnPress_MemoryLatestEnterToInternal()
{
    PageMemory::Internal::self->OpenAndSetCurrent();
    MODE_WORK = ModeWork::Internal;
    exitFromIntToLast = true;
}


void PageMemory::Resolver::OnPress_MemoryInternalEnter()
{
    PageMemory::Internal::self->OpenAndSetCurrent();
    MODE_WORK = ModeWork::Internal;
}


void PageMemory::Resolver::OnPress_MemoryInternalExit()
{
    if (exitFromIntToLast)
    {
        PageMemory::Latest::self->OpenAndSetCurrent();
        MODE_WORK = ModeWork::Latest;
        exitFromIntToLast = false;
        Menu::needClosePageSB = false;
    }
    else
    {
        MODE_WORK = ModeWork::Direct;
    }
}

