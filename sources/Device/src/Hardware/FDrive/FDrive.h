// 2022/02/11 17:48:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"

#ifdef _MS_VS
#undef _WIN32
#endif

#include <ff.h>

#ifdef _MS_VS
#define _WIN32
#endif

struct File
{
    static const int SIZE_FLASH_TEMP_BUFFER = 512;
    uint8 tempBuffer[SIZE_FLASH_TEMP_BUFFER];
    int sizeData;
    FIL fileObj;
    char name[255];
};

struct Directory
{
    char nameDir[_MAX_LFN + 1];
    char lfn[(_MAX_LFN + 1)];
    FILINFO fno;
    DIR dir;

    bool GetName(pchar fuulPath, int numDir, char *nameDirOut);
};

namespace FDrive
{
    extern bool isConnected;
    extern bool needSave;           // Если true, то нужно сохранить после отрисовки на флешку.

    void Init();

    void Update();

    void GetNumDirsAndFiles(pchar  fullPath, int *numDirs, int *numFiles);

    bool GetNextNameDir(char *nameDirOut, Directory *sfrd);

    void CloseCurrentDir(Directory *sfrd);

    bool GetNameFile(pchar fullPath, int numFile, char *nameFileOut, Directory *sfrd);

    bool GetNextNameFile(char *nameFileOut, Directory *sfrd);

    // Функция создаст файл для записи. Если такой файл уже существует, сотрёт его, заменит новым нулевой длины и откроет его
    bool OpenNewFileForWrite(pchar  fullPathToFile, File *structForWrite);

    bool WriteToFile(uint8* data, int sizeData, File *structForWrite);

    bool CloseFile(File *structForWrite);

    bool AppendStringToFile(pchar  string);
};
