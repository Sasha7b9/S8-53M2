#include "defines.h"
#include "Menu/Pages/Definition.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"


static const Choice mcMemoryExtName =
{
    TypeItem::Choice, PageMemory::External::self, 0,
    {
        "��� �����", "File name"
        ,
        "����� ����� ������������ ������ ��� ���������� �� ������� ����������:\n"
        "\"�� �����\" - ����� ��������� ������������� �� ������� �������� ����� (����. ����� ����),\n"
        "\"�������\" - ������ ��� ��� ����� ����� �������� �������"
        ,
        "Sets the mode name when saving files to an external drive:\n"
        "\"By mask\" - files are named automatically advance to the entered mask(seq.Menu),\n"
        "\"Manual\" - each time the file name must be specified manually"
    },
    {
        {"�� �����",    "Mask"},
        {"�������",     "Manually"}
    },
    (int8 *)&FILE_NAMING_MODE
};


static const Choice mcMemoryExtModeSave =
{
    TypeItem::Choice, PageMemory::External::self, 0,
    {
        "��������� ���", "Save as"
        ,
        "���� ������ ������� \"�����������\", ������ ����� ������� � ������� �������� � ����������� ����� � ����������� BMP\n"
        "���� ������ ������� \"�����\", ������ ����� ������� � ������� �������� � ��������� ���� � ����� � ���������� TXT"
        ,
        "If you select \"Image\", the signal will be stored in the current directory in graphic file with the extension BMP\n"
        "If you select \"Text\", the signal will be stored in the current directory as a text file with the extension TXT"
    },
    {
        {"�����������", "Image"},
        {"�����",       "Text"}
    },
    (int8 *)&MODE_SAVE_SIGNAL
};


static const Choice mcMemoryExtModeBtnMemory =
{
    TypeItem::Choice, PageMemory::External::self, 0,
    {
        "��� �� ������", "Mode btn MEMORY",
        "",
        ""
    },
    {
        {"����",        "Menu"},
        {"����������",  "Save"}
    },
    (int8 *)&MODE_BTN_MEMORY
};


static const Choice mcMemoryExtAutoConnectFlash =
{
    TypeItem::Choice, PageMemory::External::self, 0,
    {
        "���������.", "AutoConnect",
        "E��� \"���\", ��� ����������� �������� ���������� ���������� �������������� ������� �� �������� ������ - ����� ��",
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
    "����� ��", "EXT STORAGE",
    "������ � ������� ������������ �����������.",
    "Work with external storage device.",
    NamePage::MemoryExt, &itemsMemoryExt
);


const Page *PageMemory::External::self = &mspMemoryExt;
