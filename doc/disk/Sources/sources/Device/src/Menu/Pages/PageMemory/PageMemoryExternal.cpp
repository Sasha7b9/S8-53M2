#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"


static const Choice mcMemoryExtName =
{
    TypeItem::Choice, PageMemory::External::self, 0,
    {
        "Имя файла", "File name"
        ,
        "Задаёт режим наименования файлов при сохранении на внешний накопитель:\n"
        "\"По маске\" - файлы именуются автоматически по заранее введённой маске (след. пункт меню),\n"
        "\"Вручную\" - каждый раз имя файла нужно задавать вручную"
        ,
        "Sets the mode name when saving files to an external drive:\n"
        "\"By mask\" - files are named automatically advance to the entered mask(seq.Menu),\n"
        "\"Manual\" - each time the file name must be specified manually"
    },
    {
        {"По маске",    "Mask"},
        {"Вручную",     "Manually"}
    },
    (int8 *)&FILE_NAMING_MODE
};


static const Choice mcMemoryExtModeSave =
{
    TypeItem::Choice, PageMemory::External::self, 0,
    {
        "Сохранять как", "Save as"
        ,
        "Если выбран вариант \"Изображение\", сигнал будет сохранён в текущем каталоге в графическом файле с расширением BMP\n"
        "Если выбран вариант \"Текст\", сигнал будет сохранён в текущем каталоге в текстовом виде в файле с раширением TXT"
        ,
        "If you select \"Image\", the signal will be stored in the current directory in graphic file with the extension BMP\n"
        "If you select \"Text\", the signal will be stored in the current directory as a text file with the extension TXT"
    },
    {
        {"Изображение", "Image"},
        {"Текст",       "Text"}
    },
    (int8 *)&MODE_SAVE_SIGNAL
};


static const Choice mcMemoryExtModeBtnMemory =
{
    TypeItem::Choice, PageMemory::External::self, 0,
    {
        "Реж кн ПАМЯТЬ", "Mode btn MEMORY",
        "",
        ""
    },
    {
        {"Меню",        "Menu"},
        {"Сохранение",  "Save"}
    },
    (int8 *)&MODE_BTN_MEMORY
};


static const Choice mcMemoryExtAutoConnectFlash =
{
    TypeItem::Choice, PageMemory::External::self, 0,
    {
        "Автоподкл.", "AutoConnect",
        "Eсли \"Вкл\", при подключении внешнего накопителя происходит автоматический переход на страницу ПАМЯТЬ - Внешн ЗУ",
        "If \"Enable\", when you connect an external drive is automatically transferred to the page MEMORY - Ext.Storage"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&FLASH_AUTOCONNECT
};


static const arrayItems itemsMemoryExt =
{
    (void *)PageMemory::FileManager::self,
    (void *)&mcMemoryExtName,
    (void *)PageMemory::SetMask::self,
    (void *)&mcMemoryExtModeSave,
    (void *)&mcMemoryExtModeBtnMemory,
    (void *)&mcMemoryExtAutoConnectFlash
};


static const Page mspMemoryExt
(
    PageMemory::self, 0,
    "ВНЕШН ЗУ", "EXT STORAGE",
    "Работа с внешним запоминающим устройством.",
    "Work with external storage device.",
    NamePage::MemoryExt, &itemsMemoryExt
);


const Page *PageMemory::External::self = &mspMemoryExt;
