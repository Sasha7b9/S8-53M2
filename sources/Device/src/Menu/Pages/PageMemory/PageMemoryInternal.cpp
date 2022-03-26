#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Menu/MenuItems.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Menu/Menu.h"
#include "Utils/Math.h"
#include "Display/Symbols.h"
#include "Data/Storage.h"
#include "Hardware/FDrive/FDrive.h"
#include "Display/Screen/Grid.h"
#include "Utils/Strings.h"


static void PressSB_MemInt_Exit()
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


static const SmallButton sbExitMemInt    // Кнопка для выхода из режима малых кнопок.
(
    PageMemory::Internal::self,
    COMMON_BEGIN_SB_EXIT,
    PressSB_MemInt_Exit,
    DrawSB_Exit
);


static void PressSB_MemInt_ShowSignalAlways()
{
    PageMemory::Internal::showAlways = !PageMemory::Internal::showAlways;
}


static void DrawSB_MemInt_ShowSignalAllways_No(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, '\x68');
    Font::Set(TypeFont::_8);
}


static void DrawSB_MemInt_ShowSignalAllways_Yes(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, '\x66');
    Font::Set(TypeFont::_8);
}


static void DrawSB_MemInt_ShowSignalAlways(int x, int y)
{
    if (!PageMemory::Internal::showAlways)
    {
        DrawSB_MemInt_ShowSignalAllways_No(x, y);
    }
    else
    {
        DrawSB_MemInt_ShowSignalAllways_Yes(x, y);
    }
}


static const arrayHints hintsMemIntShowSignalAlways =
{
    {DrawSB_MemInt_ShowSignalAllways_Yes, "показывать выбранный сигнал из внутренней памяти поверх текущего",
                                          "to show the chosen signal from internal memory over the current"},
    {DrawSB_MemInt_ShowSignalAllways_No,  "сигнал из внутренней памяти виден только в режиме работы с внутренним запоминающим устройством",
                                          "the signal from internal memory is visible only in an operating mode with an internal memory"}
};


static const SmallButton sbMemIntShowSignalAlways
(
    PageMemory::Internal::self, 0,
    "Показывать всегда", "To show always",
    "Позволяет всегда показывать выбранный сохранённый сигнал поверх текущего",
    "Allows to show always the chosen kept signal over the current",
    PressSB_MemInt_ShowSignalAlways,
    DrawSB_MemInt_ShowSignalAlways,
    &hintsMemIntShowSignalAlways
);


static void PressSB_MemInt_ModeShow()
{
    Math::CircleIncrease<int8>((int8 *)&MODE_SHOW_MEMINT, 0, 2);
}


static void DrawSB_MemInt_ModeShow_Direct(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_MEM_INT_SHOW_DIRECT);
    Font::Set(TypeFont::_8);
}


static void DrawSB_MemInt_ModeShow_Saved(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_MEM_INT_SHOW_SAVED);
    Font::Set(TypeFont::_8);
}


static void DrawSB_MemInt_ModeShow_Both(int x, int y)
{
    ++y;

    Rectangle(4, 7).Draw(x + 2, y + 5);

    Rectangle(4, 4).Draw(x + 8, y + 8);
    Painter::DrawVLine(x + 8, y + 8, y + 5);
    Painter::DrawHLine(y + 5, x + 8, x + 12);

    Painter::DrawVLine(x + 14, y + 6, y + 12);
    Painter::DrawVLine(x + 17, y + 5, y + 12);
    Painter::DrawHLine(y + 9, x + 14, x + 17);
    Painter::DrawHLine(y + 5, x + 15, x + 17);
}


static void DrawSB_MemInt_ModeShow(int x, int y)
{
    ModeShowIntMem::E mode = MODE_SHOW_MEMINT;

    if (mode == ModeShowIntMem::Direct)
    {
        DrawSB_MemInt_ModeShow_Direct(x, y);
    }
    else if (mode == ModeShowIntMem::Saved)
    {
        DrawSB_MemInt_ModeShow_Saved(x, y);
    }
    else if (mode == ModeShowIntMem::Both)
    {
        DrawSB_MemInt_ModeShow_Both(x, y);
    }
}


static const arrayHints hintsMemIntModeShow =
{
    { DrawSB_MemInt_ModeShow_Direct, "на дисплее текущий сигнал",     "on the display current signal" },
    { DrawSB_MemInt_ModeShow_Saved,  "на дисплее сохранённый сигнал", "on the display the kept signal" },
    { DrawSB_MemInt_ModeShow_Both,   "на дисплее оба сигнала",        "on the display the both signals" }
};


static const SmallButton sbMemIntModeShow
(
    PageMemory::Internal::self, 0,
    "Вид сигнала", "Type of a signal",
    "Показывать записанный или текущий сигнал в режиме ВНУТР ЗУ",
    "Show recorded or current signal in mode Internal Memory",
    PressSB_MemInt_ModeShow,
    DrawSB_MemInt_ModeShow,
    &hintsMemIntModeShow
);


static void PressSB_MemInt_Delete()
{
    HAL_ROM::Data::Delete(PageMemory::Internal::currentSignal);
}


static void DrawSB_MemInt_Delete(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Font::Set(TypeFont::_8);
}


static const SmallButton sbMemIntDelete
(
    PageMemory::Internal::self, 0,
    "Удалить сигнал", "Delete signal",
    "Удалить сигнал",
    "Delete signal",
    PressSB_MemInt_Delete,
    DrawSB_MemInt_Delete
);


static void SaveSignalToIntMemory()
{
//    if (PageMemory::Internal::exitToLast)          // Если перешли во ВНУТР ЗУ из ПОСЛЕДНИЕ
//    {
//        if (Data::last.ds->valid)
//        {                               // то сохраняем сигнал из последних
//            HAL_ROM::Data::Save(PageMemory::Internal::currentSignal, Data::last);
//            HAL_ROM::Data::Get(PageMemory::Internal::currentSignal, Data::ins);
//            Warning::ShowGood(Warning::SignalIsSaved);
//        }
//    }
//    else                                // Иначе сохраняем текущий сигнал
//    {
//        if (Storage::NumFrames())
//        {
//            HAL_ROM::Data::Save(PageMemory::Internal::currentSignal, Storage::GetData(0));
//            Warning::ShowGood(Warning::SignalIsSaved);
//        }
//    }
}


static void PressSB_MemInt_SaveToIntMemory()
{
    SaveSignalToIntMemory();
}


static void DrawSB_MemInt_SaveToIntMemory(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Font::Set(TypeFont::_8);
}


static const SmallButton sbMemIntSave
(
    PageMemory::Internal::self, 0,
    "Сохранить", "Save",
    "Сохранить сигнал во внутреннем запоминующем устройстве",
    "To keep a signal in an internal memory",
    PressSB_MemInt_SaveToIntMemory,
    DrawSB_MemInt_SaveToIntMemory
);


static void PressSB_MemInt_SaveToFlashDrive()
{
    PageMemory::SetName::exitTo = RETURN_TO_INT_MEM;
    PageMemory::SaveSignalToFlashDrive();
}


static void DrawSB_MemInt_SaveToFlashDrive(int x, int y) //-V524
{
    if (FDrive::isConnected)
    {
        Font::Set(TypeFont::UGO2);
        PText::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Font::Set(TypeFont::_8);
    }
}


static const SmallButton sbMemIntSaveToFlash
(
    PageMemory::Internal::self, 0,
    "Сохранить", "Save",
    "Сохраняет сигнал на флешку",
    "Save signal to flash drive",
    PressSB_MemInt_SaveToFlashDrive,
    DrawSB_MemInt_SaveToFlashDrive
);


static const arrayItems itemsMemInt =
{
    (void *)&sbExitMemInt,
    (void *)&sbMemIntShowSignalAlways,
    (void *)&sbMemIntModeShow,
    (void *)&sbMemIntDelete,
    (void *)&sbMemIntSave,
    (void *)&sbMemIntSaveToFlash
};


static void OnPressMemoryInt()
{
    PageMemory::Internal::self->OpenAndSetCurrent();
    MODE_WORK = ModeWork::MemInt;
}


static void DrawMemoryWave(int num, bool exist)
{
    int x = Grid::Left() + 2 + num * 12;
    int y = Grid::FullBottom() - 10;
    int width = 12;
    Region(width, 10).Fill(x, y, (num == PageMemory::Internal::currentSignal) ? Color::FLASH_10 : COLOR_BACK);
    Rectangle(width, 10).Draw(x, y, COLOR_FILL);
    Color::SetCurrent(num == PageMemory::Internal::currentSignal ? Color::FLASH_01 : COLOR_FILL);
    if (exist)
    {
        SU::Int2String(num + 1, false, 2).Draw(x + 2, y + 1);
    }
    else
    {
        String("\x88").Draw(x + 3, y + 1);
    }
}


static void FuncAdditionDrawingSPageMemoryInt()
{
    // Теперь нарисуем состояние памяти

    bool exist[MAX_NUM_SAVED_WAVES] = {false};

    HAL_ROM::Data::GetInfo(exist);

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        DrawMemoryWave(i, exist[i]);
    }
}


static void FuncOnRegSetMemInt(int delta)
{
    if (delta < 0)
    {
        Math::CircleDecrease<int8>(&PageMemory::Internal::currentSignal, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    else if (delta > 0)
    {
        Math::CircleIncrease<int8>(&PageMemory::Internal::currentSignal, 0, MAX_NUM_SAVED_WAVES - 1);
    }

    Color::ResetFlash();
}


static const Page mspMemInt
(
    PageMemory::self, 0,
    "ВНУТР ЗУ", "INT STORAGE",
    "Переход в режим работы с внутренней памятью",
    "Transition to an operating mode with internal memory",
    NamePage::SB_MemInt, &itemsMemInt, OnPressMemoryInt, FuncAdditionDrawingSPageMemoryInt, FuncOnRegSetMemInt
);


const Page *PageMemory::Internal::self = &mspMemInt;
