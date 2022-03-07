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

struct StructForWrite
{
    static const int SIZE_FLASH_TEMP_BUFFER = 512;
    uint8 tempBuffer[SIZE_FLASH_TEMP_BUFFER];
    int sizeData;
    FIL fileObj;
    char name[255];
};

struct StructForReadDir
{
    char nameDir[_MAX_LFN + 1];
    char lfn[(_MAX_LFN + 1)];
    FILINFO fno;
    DIR dir;
};

namespace FDrive
{
    extern bool isConnected;

    void Init();

    void Update();

    void GetNumDirsAndFiles(pchar  fullPath, int *numDirs, int *numFiles);

    bool GetNameDir(pchar  fuulPath, int numDir, char *nameDirOut, StructForReadDir *sfrd);

    bool GetNextNameDir(char *nameDirOut, StructForReadDir *sfrd);

    void CloseCurrentDir(StructForReadDir *sfrd);

    bool GetNameFile(pchar fullPath, int numFile, char *nameFileOut, StructForReadDir *sfrd);

    bool GetNextNameFile(char *nameFileOut, StructForReadDir *sfrd);

    // Функция создаст файл для записи. Если такой файл уже существует, сотрёт его, заменит новым нулевой длины и откроет его
    bool OpenNewFileForWrite(pchar  fullPathToFile, StructForWrite *structForWrite);

    bool WriteToFile(uint8* data, int sizeData, StructForWrite *structForWrite);

    bool CloseFile(StructForWrite *structForWrite);

    bool AppendStringToFile(pchar  string);
};
