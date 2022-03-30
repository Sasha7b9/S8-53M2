#include "defines.h"
#include "Menu/MenuItems.h"
#include "Menu/Pages/Definition.h"
#include "Settings/Settings.h"
#include "Hardware/FDrive/FDrive.h"
#include "Utils/Text/Symbols.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Tables.h"
#include <cstring>


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

    if (size < MAX_SYMBOLS_IN_FILE_NAME - 1)
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


static const Page mpSetName
(
    0, 0,
    "", "",
    "",
    "",
    NamePage::SB_MemExtSetName, &itemsSetName, EmptyFuncVV, EmptyFuncVV, OnMemExtSetNameRegSet
);


const Page *PageMemory::SetName::self = &mpSetName;
