// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


#define SET_ENUM_POINTS             (gset.memory.fpgaENumPoints)
#define SET_ENUM_POINTS_IS_281      (SET_ENUM_POINTS == ENUM_POINTS_FPGA::_281)
#define SET_ENUM_POINTS_IS_1024     (SET_ENUM_POINTS == ENUM_POINTS_FPGA::_1024)

#define MODE_WORK                   (gset.memory.modeWork)
#define MODE_WORK_IS_DIRECT         (MODE_WORK == ModeWork::Direct)
#define MODE_WORK_IS_LATEST         (MODE_WORK == ModeWork::Latest)
#define MODE_WORK_IS_MEMINT         (MODE_WORK == ModeWork::Internal)

#define FILE_NAMING_MODE            (gset.memory.fileNamingMode)
#define FILE_NAMING_MODE_IS_MASK    (FILE_NAMING_MODE == FileNamingMode::Mask)
#define FILE_NAMING_MODE_IS_HAND    (FILE_NAMING_MODE == FileNamingMode::Manually)

#define FILE_NAME_MASK              (gset.memory.fileNameMask)

#define FILE_NAME                   (gset.memory.fileName)

#define INDEX_SYMBOL                (gset.memory.indexCurSymbolNameMask)

#define MODE_SHOW_MEMINT            (gset.memory.modeShowIntMem)
#define MODE_SHOW_MEMINT_IS_SAVED   (MODE_SHOW_MEMINT == ModeShowIntMem::Saved)
#define MODE_SHOW_MEMINT_IS_DIRECT  (MODE_SHOW_MEMINT == ModeShowIntMem::Direct)
#define MODE_SHOW_MEMINT_IS_BOTH    (MODE_SHOW_MEMINT == ModeShowIntMem::Both)

#define FLASH_AUTOCONNECT           (gset.memory.flashAutoConnect)

#define MODE_BTN_MEMORY             (gset.memory.modeBtnMemory)
#define MODE_BTN_MEMORY_IS_SAVE     (MODE_BTN_MEMORY == ModeBtnMemory::Save)

#define MODE_SAVE_SIGNAL            (gset.memory.modeSaveSignal)
#define MODE_SAVE_SIGNAL_IS_BMP     (MODE_SAVE_SIGNAL == ModeSaveSignal::BMP)


// ����� ������.
struct ModeWork
{
    enum E
    {
        Direct,     // �������� �����.
        Latest,     // � ���� ������ ����� ����������� ��������� ���������� ���������.
        Internal,   // � ���� ������ ����� ��������� �� flash-������ ��������� ������������� ����� ����������.
        Count
    };
};


// ����� ������������ ������.
struct FileNamingMode
{
    enum E
    {
        Mask,        // ��������� �� �����.
        Manually     // ��������� �������.
    };
};

// ��� ���������� � ������ ����� �� - ��������� ��� ���������� ������.
struct ModeShowIntMem
{
    enum E
    {
        Direct,  // ���������� ������ ��������� �������.
        Saved,   // ���������� ���������� ������.
        Both     // ���������� ������ ��������� ������� � ���������� ������.
    };
};

// ��� ������ ��� ������� ������ ������.
struct ModeBtnMemory
{
    enum E
    {
        Menu,     // ����� ������������ ��������������� �������� ����.
        Save      // ���������� ����������� ������ �� ������.
    };
};

// ��� ��������� ������ �� ������.
struct ModeSaveSignal
{
    enum E
    {
        BMP,     // ��������� ������ �� ������ � ������� .bmp.
        TXT      // ��������� ������ �� ������ � ���������� ����.
    };
};

// ��������� ����->������
struct SettingsMemory
{
#define MAX_SYMBOLS_IN_FILE_NAME 35
    ENUM_POINTS_FPGA::E fpgaENumPoints;                         // ����� �����.
    ModeWork::E         modeWork;                               // ����� ������.
    FileNamingMode::E   fileNamingMode;                         // ����� ���������� ������.
    char                fileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; // ����� ����� ��� ��������������� ���������� ������\n
         // ������� ����������.\n
         // %y('\x42') - ���, %m('\x43') - �����, %d('\x44') - ����, %H('\x45') - ����, %M('\x46') - ������, %S('\x47') - �������\n
         // %Nn('\x48''n') - ���������� �����, ������ �������� �� ����� n ���������, ��������, 7 � %3N ����� ������������� � 007\n
         // �������\n
         // name_%4N_%y_%m_%d_%H_%M_%S ����� ������������ ����� ���� name_0043_2014_04_25_14_45_32\n
         // ��� ���� �������� ��������, ��� ���� ������������ %4N ����� ����� ��������� ����������, ��, ������ �����, ���� �������� ����� ������ ����� 0001, �.�. ��� ����������� ������ ��������������� ������ ������� �� ����.
    char                fileName[MAX_SYMBOLS_IN_FILE_NAME];     // ��� ����� ��� ������ ������� �������
    int8                indexCurSymbolNameMask;                 // ������ �������� ������� � ������ ������� ����� ��� ������ �����.
    bool                isActiveModeSelect;
    ModeShowIntMem::E   modeShowIntMem;                         // ����� ������� ���������� � ������ ����������� ��
    bool                flashAutoConnect;                       // ���� true, ��� ���������� ���� ������������� ��������� NC (������ ���������)
    ModeBtnMemory::E    modeBtnMemory;
    ModeSaveSignal::E   modeSaveSignal;                         // � ����� ���� ��������� ������.
};
