#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Data/Storage.h"
#include "Hardware/FDrive/FDrive.h"
#include "Display/Screen/Grid.h"
#include "Utils/Strings.h"


static void PressSB_MemLast_Exit()
{
    MODE_WORK = ModeWork::Direct;

    if (FPGA::runningBeforeSmallButtons)
    {
        FPGA::Start();
        FPGA::runningBeforeSmallButtons = false;
    }

    Display::RemoveAddDrawFunction();
}


static const SmallButton sbExitMemLatest
(
    PageMemory::Latest::self,
    COMMON_BEGIN_SB_EXIT,
    PressSB_MemLast_Exit,
    DrawSB_Exit
);


static void PressSB_MemLast_Next()
{
    Math::CircleIncrease<int16>(&PageMemory::Latest::current, 0, Storage::NumFrames() - 1);
}


static void DrawSB_MemLast_Next(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 2, '\x64');
    Font::Set(TypeFont::_8);
}


static const SmallButton sbMemLatestNext
(
    PageMemory::Latest::self, 0,
    "Следующий", "Next",
    "Перейти к следующему сигналу",
    "Go to the next signal",
    PressSB_MemLast_Next,
    DrawSB_MemLast_Next
);


static void PressSB_MemLast_Prev()
{
    Math::CircleDecrease<int16>(&PageMemory::Latest::current, 0, Storage::NumFrames() - 1);
}


static void DrawSB_MemLast_Prev(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 2, '\x20');
    Font::Set(TypeFont::_8);
}


static const SmallButton sbMemLatestPrev
(
    PageMemory::Latest::self, 0,
    "Предыдущий", "Previous",
    "Перейти к предыдущему сигналу",
    "Go to the previous signal",
    PressSB_MemLast_Prev,
    DrawSB_MemLast_Prev
);


static void PressSB_MemLast_IntEnter()
{
    PageMemory::Internal::self->OpenAndSetCurrent();
    MODE_WORK = ModeWork::MemInt;
    HAL_ROM::Data::Get(PageMemory::Internal::currentSignal, Data::ins);
    PageMemory::Internal::exitToLast = true;
}


static void DrawSB_MemLast_IntEnter(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, '\x40');
    Font::Set(TypeFont::_8);
}


static const SmallButton sbMemLatestIntEnter
(
    PageMemory::Latest::self, 0,
    "Внутр ЗУ", "Internal storage",
    "Нажмите эту кнопку, чтобы сохранить сигнал во внутреннем запоминающем устройстве",
    "Press this button to keep a signal in an internal memory",
    PressSB_MemLast_IntEnter,
    DrawSB_MemLast_IntEnter
);


static void PressSB_MemLast_SaveToFlash()
{
    PageMemory::SetName::exitTo = RETURN_TO_LAST_MEM;
    PageMemory::SaveSignalToFlashDrive();
}


static void DrawSB_MemLast_SaveToFlash(int x, int y)
{
    if (FDrive::isConnected)
    {
        Font::Set(TypeFont::UGO2);
        PText::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Font::Set(TypeFont::_8);
    }
}


static const SmallButton sbMemLatestSaveToFlash
(
    PageMemory::Latest::self, 0,
    "Сохранить", "Save",
    "Кнопка становится доступна при присоединённом внешнем ЗУ. Позволяет сохранить сигнал на внешем ЗУ",
    "Click this button to save the signal on the external FLASH",
    PressSB_MemLast_SaveToFlash,
    DrawSB_MemLast_SaveToFlash
);


static void OnPressMemoryLatest()
{
    PageMemory::Latest::current = 0;
    FPGA::runningBeforeSmallButtons = FPGA::IsRunning();
    FPGA::Stop();
    MODE_WORK = ModeWork::Latest;
}


static void FuncDrawingAdditionSPageMemoryLast()
{
    int width = 50;
    int height = 10;
    Region(width, height).Fill(Grid::Right() - width, GRID_TOP, COLOR_BACK);
    Rectangle(width, height).Draw(Grid::Right() - width, GRID_TOP, COLOR_FILL);
    SU::Int2String(PageMemory::Latest::current + 1, false, 3).Draw(Grid::Right() - width + 2, GRID_TOP + 1);
    String("/").Draw(Grid::Right() - width + 22, GRID_TOP + 1);
    SU::Int2String(Storage::NumFrames(), false, 3).Draw(Grid::Right() - width + 28, GRID_TOP + 1);
}


static void RotateSB_MemLast(int angle)
{
    if (Math::Sign(angle) > 0)
    {
        PressSB_MemLast_Next();
    }
    else
    {
        PressSB_MemLast_Prev();
    }
}


static const arrayItems itemsMemLatest =
{
    (void *)&sbExitMemLatest,
    (void *)0,
    (void *)&sbMemLatestNext,
    (void *)&sbMemLatestPrev,
    (void *)&sbMemLatestIntEnter,
    (void *)&sbMemLatestSaveToFlash
};


static const Page mspMemLatest
(
    PageMemory::self, 0,
    "ПОСЛЕДНИЕ", "LATEST",
    "Переход в режим работы с последними полученными сигналами",
    "Transition to an operating mode with the last received signals",
    NamePage::SB_MemLatest, &itemsMemLatest, OnPressMemoryLatest, FuncDrawingAdditionSPageMemoryLast, RotateSB_MemLast
);


const Page *PageMemory::Latest::self = &mspMemLatest;
