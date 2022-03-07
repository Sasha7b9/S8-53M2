// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


#define SET_ENUM_POINTS             (set.memory.fpgaENumPoints)                     // SettingsMemory.fpgaNumPoints
#define SET_ENUM_POINTS_IS_281      (SET_ENUM_POINTS == ENUM_POINTS_FPGA::_281)
#define SET_ENUM_POINTS_IS_1024     (SET_ENUM_POINTS == ENUM_POINTS_FPGA::_1024)

#define MODE_WORK                   (set.memory.modeWork)                           // SettingsMemory.modeWork
#define MODE_WORK_IS_DIRECT         (MODE_WORK == ModeWork_Direct)
#define MODE_WORK_IS_LATEST         (MODE_WORK == ModeWork_Latest)
#define MODE_WORK_IS_MEMINT         (MODE_WORK == ModeWork_MemInt)

#define FILE_NAMING_MODE            (set.memory.fileNamingMode)                     // SettingsMemory.fileNamingMode
#define FILE_NAMING_MODE_IS_MASK    (FILE_NAMING_MODE == FileNamingMode_Mask)
#define FILE_NAMING_MODE_IS_HAND    (FILE_NAMING_MODE == FileNamingMode_Manually)

#define FILE_NAME_MASK              (set.memory.fileNameMask)                       // SettingsMemory.fileNameMask

#define FILE_NAME                   (set.memory.fileName)                           // SettingsMemory.fileName

#define INDEX_SYMBOL                (set.memory.indexCurSymbolNameMask)             // SettingsMemory.indexCurSymbolNameMask

#define MODE_SHOW_MEMINT            (set.memory.modeShowIntMem)                     // SettingsMemory.modeShowIntMem
#define MODE_SHOW_MEMINT_IS_SAVED   (MODE_SHOW_MEMINT == ModeShowIntMem_Saved)
#define MODE_SHOW_MEMINT_IS_DIRECT  (MODE_SHOW_MEMINT == ModeShowIntMem_Direct)

#define FLASH_AUTOCONNECT           (set.memory.flashAutoConnect)                   // SettingsMemory.flashAutoConnect

#define MODE_BTN_MEMORY             (set.memory.modeBtnMemory)                      // SettingsMemory.modeBtnMemory
#define MODE_BTN_MEMORY_IS_SAVE     (MODE_BTN_MEMORY == ModeBtnMemory_Save)

#define MODE_SAVE_SIGNAL            (set.memory.modeSaveSignal)                     // SettingsMemory.modeSaveSignal
#define MODE_SAVE_SIGNAL_IS_BMP     (MODE_SAVE_SIGNAL == ModeSaveSignal_BMP)




// Режим работы.
enum ModeWork
{
    ModeWork_Direct,        // Основной режим.
    ModeWork_Latest,        // В этом режиме можно просмотреть последние сохранённые измерения.
    ModeWork_MemInt,        // В этом режиме можно сохранять во flash-памяти измерения просматривать ранее сохранённые.
};


// Режим наименования файлов.
enum FileNamingMode
{
    FileNamingMode_Mask,        // Именовать по маске.
    FileNamingMode_Manually     // Именовать вручную.
};

// Что показывать в режиме Внутр ЗУ - считанный или записанный сигнал.
enum ModeShowIntMem
{
    ModeShowIntMem_Direct,  // Показывать данные реального времени.
    ModeShowIntMem_Saved,   // Показывать сохранённые данные.
    ModeShowIntMem_Both     // Показывать данные реального времени и сохранённые данные.
};

// Что делать при нажатии кнопки ПАМЯТЬ.
enum ModeBtnMemory
{
    ModeBtnMemory_Menu,     // Будет открывааться соответствующая страница меню.
    ModeBtnMemory_Save      // Сохранение содержимого экрана на флешку.
};

// Как сохранять данные на флешку.
enum ModeSaveSignal
{
    ModeSaveSignal_BMP,     // Сохранять данные на флешку в формате .bmp.
    ModeSaveSignal_TXT      // Сохранять данные на флешку в текствовом виде.
};

struct StructMemoryLast
{
    bool isActiveModeSelect;
};

// Настройки МЕНЮ->ПАМЯТЬ
struct SettingsMemory
{
#define MAX_SYMBOLS_IN_FILE_NAME 35
    ENUM_POINTS_FPGA::E fpgaENumPoints;                         // Число точек.
    ModeWork            modeWork;                               // Режим работы.
    FileNamingMode      fileNamingMode;                         // Режим именования файлов.
    char                fileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; // Здесь маска для автоматического именования файлов\n
         // Правила именования.\n
         // %y('\x42') - год, %m('\x43') - месяц, %d('\x44') - день, %H('\x45') - часы, %M('\x46') - минуты, %S('\x47') - секунды\n
         // %Nn('\x48''n') - порядковый номер, котрый занимает не менее n знакомест, например, 7 в %3N будет преобразовано в 007\n
         // Примеры\n
         // name_%4N_%y_%m_%d_%H_%M_%S будет генерировать файлы вида name_0043_2014_04_25_14_45_32\n
         // При этом обратите внимание, что если спецификатор %4N стоИт после временнЫх параметров, то, скорее всего, этот параметр будет всегда равен 0001, т.к. для определения номера просматриваются только символы ДО него.
    char                fileName[MAX_SYMBOLS_IN_FILE_NAME];     // Имя файла для режима ручного задания
    int8                indexCurSymbolNameMask;                 // Индекс текущего символа в режиме задания маски или выбора имени.
    StructMemoryLast    strMemoryLast;
    ModeShowIntMem      modeShowIntMem;                         // Какие сигналы показывать в режиме внутреннего ЗУ
    bool                flashAutoConnect;                       // Если true, при подлючении флеш автоматически выводится NC (Нортон Коммандер)
    ModeBtnMemory       modeBtnMemory;
    ModeSaveSignal      modeSaveSignal;                         // В каком виде сохранять сигнал.
};
