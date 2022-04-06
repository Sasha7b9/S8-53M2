#include "defines.h"
#include "Menu/MenuItems.h"
#include "Menu/Pages/Definition.h"
#include "Settings/Settings.h"
#include "Hardware/FDrive/FDrive.h"
#include "Utils/Text/Symbols.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Tables.h"
#include "Display/Screen/Grid.h"
#include <cstring>


static void PressSB_SetName_Exit()
{
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


// Кнопк для выхода из режима задания имени сохраняемому сигналу. Одновременно кнопка отказа от сохранения
static const SmallButton sbExitSetName
(
    PageMemory::SetName::self, 0,
    EXIT_RU, EXIT_EN,
    "Отказ от сохранения",
    "Failure to save",
    PressSB_SetName_Exit,
    DrawSB_Exit
);


static void PressSB_SetName_Delete()
{
    FILE_NAME[0] = '\0';
}


static void DrawSB_SetName_Delete(int x, int y) //-V524
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Font::Set(TypeFont::_8);
}


static const SmallButton sbSetNameDelete
(
    PageMemory::SetName::self, 0,
    "Удалить", "Delete",
    "Удаляет все введённые символы",
    "Deletes all entered characters",
    PressSB_SetName_Delete,
    DrawSB_SetName_Delete
);


static void PressSB_SetName_Backspace()
{
    int size = (int)std::strlen(FILE_NAME);
    if (size > 0)
    {
        FILE_NAME[size - 1] = '\0';
    }
}


static void DrawSB_SetName_Backspace(int x, int y) //-V524
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Font::Set(TypeFont::_8);
}


static const SmallButton sbSetNameBackspace
(
    PageMemory::SetName::self, 0,
    "Backspace", "Backspace",
    "Удаляет последний символ",
    "Delete the last character",
    PressSB_SetName_Backspace,
    DrawSB_SetName_Backspace
);


static void PressSB_SetName_Insert()
{
    int size = (int)std::strlen(FILE_NAME);

    if (size < SettingsMemory::MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        FILE_NAME[size] = Tables::symbolsAlphaBet[INDEX_SYMBOL][0];
        FILE_NAME[size + 1] = '\0';
    }
}


static void DrawSB_SetName_Insert(int x, int y) //-V524
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 2, '\x26');
    Font::Set(TypeFont::_8);
}


static const SmallButton sbSetNameInsert
(
    PageMemory::SetName::self, 0,
    "Вставить", "Insert",
    "Вводит очередной символ",
    "Print the next character",
    PressSB_SetName_Insert,
    DrawSB_SetName_Insert
);


static void PressSB_MemExtSetNameSave()
{
    if (FDrive::isConnected)
    {
        PressSB_SetName_Exit();
        FDrive::needSave = true;
    }
}


static void DrawSB_MemExtSetNameSave(int x, int y)
{
    if (FDrive::isConnected)
    {
        Font::Set(TypeFont::UGO2);
        Text::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Font::Set(TypeFont::_8);
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


static const arrayItems itemsSetName =
{
    (void *)&sbExitSetName,
    (void *)&sbSetNameDelete,
    (void *)0,
    (void *)&sbSetNameBackspace,
    (void *)&sbSetNameInsert,
    (void *)&sbSetNameSave
};


// Страница вызывается при выбранном ручном режиме задания имени файла перед сохранением на флешку ///////////////
static void OnMemExtSetNameRegSet(int angle)
{
    extern void OnMemExtSetMaskNameRegSet(int angle, int maxIndex);

    OnMemExtSetMaskNameRegSet(angle, sizeof(Tables::symbolsAlphaBet) / 4 - 7);
}


static void DrawSetName()
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

    int x = String<>(FILE_NAME).Draw(x0 + deltaX, y0 + 65, COLOR_FILL);
    Region(5, 8).Fill(x, y0 + 65, Color::FLASH_10);
}


static const Page mpSetName
(
    PageMemory::External::self, 0,
    "", "",
    "",
    "",
    NamePage::SB_MemExtSetName, &itemsSetName, EmptyFuncVV, DrawSetName, OnMemExtSetNameRegSet
);


const Page *PageMemory::SetName::self = &mpSetName;
