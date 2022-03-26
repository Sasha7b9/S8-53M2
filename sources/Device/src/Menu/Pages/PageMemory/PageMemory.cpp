// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Menu/Pages/Definition.h"
#include "Menu/FileManager.h"
#include "FPGA/FPGA.h"
#include "Data/Storage.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/font/Font.h"
#include "Display/Painter.h"
#include "Display/Screen/Grid.h"
#include "Display/Symbols.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Hardware/FDrive/FDrive.h"
#include "Hardware/Sound.h"
#include "Log.h"
#include "Display/Tables.h"
#include "Hardware/HAL/HAL.h"
#include "Data/Data.h"
#include "Data/DataExtensions.h"
#include "Data/Processing.h"
#include "Utils/Strings.h"
#include <cstring>
#include <cstdio>





void DrawSB_MemLastSelect(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 3, y + 2, set.memory.isActiveModeSelect ? '\x2a' : '\x28');
    Font::Set(TypeFont::_8);
}


void PressSB_MemLastSelect()
{
    set.memory.isActiveModeSelect = !set.memory.isActiveModeSelect;
}


static void DrawSB_MemExtSetNameSave(int x, int y)
{
    if (FDrive::isConnected)
    {
        Font::Set(TypeFont::UGO2);
        PText::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Font::Set(TypeFont::_8);
    }
}


static void PressSB_SetName_Exit()
{
    Display::RemoveAddDrawFunction();

    if (EXIT_FROM_SET_NAME_TO_DIS_MENU)
    {
        Page::FromName(NamePage::SB_MemExtSetName)->ShortPressOnItem(0);
    }
    else if (EXIT_FROM_SET_NAME_TO_LAST)
    {
        PageMemory::Latest::self->OpenAndSetCurrent();
    }
    else if (EXIT_FROM_SET_NAME_TO_INT)
    {
        PageMemory::Internal::self->OpenAndSetCurrent();
    }

    PageMemory::SetName::exitTo = RETURN_TO_DISABLE_MENU;
}


static void PressSB_MemExtSetNameSave()
{
    if (FDrive::isConnected)
    {
        PressSB_SetName_Exit();
        FDrive::needSave = true;
    }
}

static const SmallButton sbSetNameSave
(
    PageMemory::SetName::self, 0,
    "Сохранить", "Save",
    "Сохранение на флеш под заданным именем",
    "Saving to flashdrive with the specified name",
    PressSB_MemExtSetNameSave,
    DrawSB_MemExtSetNameSave
);




void DrawSB_SetName_Backspace(int x, int y) //-V524
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Font::Set(TypeFont::_8);
}



void PressSB_SetName_Backspace()
{
    int size = (int)std::strlen(FILE_NAME);
    if (size > 0)
    {
        FILE_NAME[size - 1] = '\0';
    }
}




void DrawSB_SetName_Delete(int x, int y) //-V524
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Font::Set(TypeFont::_8);
}

void PressSB_SetName_Delete()
{
    FILE_NAME[0] = '\0';
}





void DrawSB_SetName_Insert(int x, int y) //-V524
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 2, '\x26');
    Font::Set(TypeFont::_8);
}

void PressSB_SetName_Insert()
{
    int size = (int)std::strlen(FILE_NAME);

    if (size < MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        FILE_NAME[size] = Tables::symbolsAlphaBet[INDEX_SYMBOL][0];
        FILE_NAME[size + 1] = '\0';
    }
}















const SmallButton sbSetNameBackspace
(
    PageMemory::SetName::self, 0,
    "Backspace", "Backspace",
    "Удаляет последний символ",
    "Delete the last character",
    PressSB_SetName_Backspace,
    DrawSB_SetName_Backspace
);

const SmallButton sbSetNameDelete
(
    PageMemory::SetName::self, 0,
    "Удалить", "Delete",
    "Удаляет все введённые символы",
    "Deletes all entered characters",
    PressSB_SetName_Delete,
    DrawSB_SetName_Delete
);

const SmallButton sbSetNameInsert
(
    PageMemory::SetName::self, 0,
    "Вставить", "Insert",
    "Вводит очередной символ",
    "Print the next character",
    PressSB_SetName_Insert,
    DrawSB_SetName_Insert
);

void DrawSB_MemExtNewFolder(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 1, y, '\x46');
    Font::Set(TypeFont::_8);
}















extern const Page mpSetName;

// Кнопк для выхода из режима задания имени сохраняемому сигналу. Одновременно кнопка отказа от сохранения
static const SmallButton sbExitSetName
(
    &mpSetName, 0,
    EXIT_RU, EXIT_EN,
    "Отказ от сохранения",
    "Failure to save",
    PressSB_SetName_Exit,
    DrawSB_Exit
);












extern const Page pMemory;


static bool FuncActiveMemoryNumPoinst()
{
    return SET_PEAKDET_IS_DISABLED;
}


void PageMemory::OnChanged_NumPoints(bool)
{
    bool running = FPGA::IsRunning();
    FPGA::Stop();

    Storage::Clear();

    if (ENUM_POINTS_FPGA::ToNumPoints() == 281)
    {
        SHIFT_IN_MEMORY = 0;
    }
    else
    {
        if (SET_TPOS_IS_LEFT)
        {
            SHIFT_IN_MEMORY = 0;
        }
        else if (SET_TPOS_IS_CENTER)
        {
            SHIFT_IN_MEMORY = ENUM_POINTS_FPGA::ToNumPoints() / 2 - Grid::Width() / 2;
        }
        else if (SET_TPOS_IS_RIGHT)
        {
            SHIFT_IN_MEMORY = ENUM_POINTS_FPGA::ToNumPoints() - Grid::Width() - 2;
        }
    }

    TShift::Set(SET_TSHIFT);

    if (running)
    {
        FPGA::Start();
    }
}


const Choice mcMemoryNumPoints =
{
    TypeItem::Choice, &pMemory, FuncActiveMemoryNumPoinst,
    {
        "Точки", "Points"
        ,
        "Выбор количества отсчётов для сохраняемых сигналов. "
        "При увеличении количества отсчётов уменьшается количество сохранённых в памяти сигналов."
        ,
        "Choice of number of counting for the saved signals. "
        "At increase in number of counting the quantity of the signals kept in memory decreases."
    },
    {   
        {"281",     "281"},
        {"512",     "512"},
        {"1024",    "1024"}
    },
    (int8*)&SET_ENUM_POINTS, PageMemory::OnChanged_NumPoints
};











// Эта функция рисует, когда нужно задать имя файла для сохранения
void DrawSetName()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 60;
    int width = Grid::Width() - 80;
    int height = 80;

    Rectangle(width, height).Draw(x0, y0, COLOR_FILL);
    Region(width - 2, height - 2).Fill(x0 + 1, y0 + 1, COLOR_BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Рисуем большие буквы английского алфавита
    while (Tables::symbolsAlphaBet[index][0] != ' ')
    {
        Tables::DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // Теперь рисуем цифры и пробел
    position = 0;
    while (Tables::symbolsAlphaBet[index][0] != 'a')
    {
        Tables::DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // Теперь рисуем малые буквы алфавита
    position = 0;
    while (Tables::symbolsAlphaBet[index][0] != '%')
    {
        Tables::DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    int x = String(FILE_NAME).Draw(x0 + deltaX, y0 + 65, COLOR_FILL);
    Region(5, 8).Fill(x, y0 + 65, Color::FLASH_10);
}


void PageMemory::SaveSignalToFlashDrive()
{
    if (FDrive::isConnected)
    {
        if (FILE_NAMING_MODE_IS_HAND)
        {
            PageMemory::SetName::self->OpenAndSetCurrent();
            Display::SetAddDrawFunction(DrawSetName);
        }
        else
        {
            FDrive::needSave = true;
        }
    }
    else
    {
        PageMemory::SetName::exitTo = RETURN_TO_MAIN_MENU;
    }
}













void PageMemory::OnPressExtFileManager()
{
    if(FDrive::isConnected)
    {
        PageMemory::FileManager::self->OpenAndSetCurrent();
        Display::SetDrawMode(DrawMode::Hand, FM::Draw);
        FM::needRedraw = 1;
    }
}












// Страница вызывается при выбранном ручном режиме задания имени файла перед сохранением на флешку ///////////////
static void OnMemExtSetNameRegSet(int angle)
{
extern void OnMemExtSetMaskNameRegSet(int angle, int maxIndex);

    OnMemExtSetMaskNameRegSet(angle, sizeof(Tables::symbolsAlphaBet) / 4 - 7);
}

static const arrayItems itemsSetName =
{
    (void*)&sbExitSetName,
    (void*)&sbSetNameDelete,
    (void*)0,
    (void*)&sbSetNameBackspace,
    (void*)&sbSetNameInsert,
    (void*)&sbSetNameSave
};


static const Page mpSetName
(
    0, 0,
    "", "",
    "", 
    "",
    NamePage::SB_MemExtSetName, &itemsSetName, EmptyFuncVV, EmptyFuncVV, OnMemExtSetNameRegSet
);


const Page *PageMemory::SetName::self = &mpSetName;


static const arrayItems itemsMemory =
{
    (void *)&mcMemoryNumPoints,
    (void *)PageMemory::Latest::self,
    (void *)PageMemory::Internal::self,
    (void *)PageMemory::External::self
};


static const Page pMemory              // ПАМЯТЬ
(
    PageMain::self, 0,
    "ПАМЯТЬ", "MEMORY",
    "Работа с внешней и внутренней памятью.",
    "Working with external and internal memory.",
    NamePage::Memory, &itemsMemory
);


const Page *PageMemory::self = &pMemory;

