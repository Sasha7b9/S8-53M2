#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Hardware/FDrive/FDrive.h"
#include "Menu/MenuItems.h"
#include "Menu/FileManager.h"
#include "Utils/Text/Symbols.h"
#include "Utils/Text/Text.h"


static bool FuncOfActiveExtMemFolder()
{
    return FDrive::isConnected;
}


static void PressSB_FM_Exit()
{
    Display::SetDrawMode(DrawMode::Normal);
}


static const SmallButton sbExitFileManager
(
    PageMemory::FileManager::self, 0,
    EXIT_RU, EXIT_EN,
    EXIT_ILLUSTRATION_RU,
    EXIT_ILLUSTRATION_EN,
    PressSB_FM_Exit,
    DrawSB_Exit
);


static void DrawSB_FM_Tab(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, '\x6e');
    Font::Set(TypeFont::_8);
}


static const SmallButton sbFileManagerTab
(
    PageMemory::FileManager::self, 0,
    "Tab", "Tab",
    "Переход между каталогами и файлами",
    "The transition between the directories and files",
    FM::PressTab,
    DrawSB_FM_Tab
);


static void DrawSB_FM_LevelUp(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    Font::Set(TypeFont::_8);
}


static const SmallButton sbFileManagerLevelUp
(
    PageMemory::FileManager::self, 0,
    "Выйти", "Leave",
    "Переход в родительский каталог",
    "Transition to the parental catalog",
    FM::PressLevelUp,
    DrawSB_FM_LevelUp
);


static void DrawSB_FM_LevelDown(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    Font::Set(TypeFont::_8);
}


static const SmallButton sbFileManagerLevelDown
(
    PageMemory::FileManager::self, 0,
    "Войти", "Enter",
    "Переход в выбранный каталог",
    "Transition to the chosen catalog",
    FM::PressLevelDown,
    DrawSB_FM_LevelDown
);


static const arrayItems itemsFileManager =
{
    (void *)&sbExitFileManager,
    (void *)&sbFileManagerTab,
    (void *)0,
    (void *)0,
    (void *)&sbFileManagerLevelUp,
    (void *)&sbFileManagerLevelDown
};


static const Page mspFileManager
(
    PageMemory::External::self, FuncOfActiveExtMemFolder,
    "КАТАЛОГ", "DIRECTORY",
    "Открывает доступ к файловой системе подключенного накопителя",
    "Provides access to the file system of the connected drive",
    NamePage::SB_FileManager, &itemsFileManager, PageMemory::OnPressExtFileManager, EmptyFuncVV, FM::RotateRegSet
);


const Page *PageMemory::FileManager::self = &mspFileManager;
