// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


class PageMemory
{
public:

    class Latest
    {
    public:
        static void *GetPointer();
    };

    class Internal
    {
    public:
        static void *GetPointer();
    };

    class SetMask
    {
    public:
        static void *GetPointer();
    };

    class SetName
    {
    public:
        static void *GetPointer();
    };

    class FileManager
    {
    public:
        static void *GetPointer();
    };

    static void *GetPointer();
};


void ChangeC_Memory_NumPoints(bool active);
void OnPressMemoryExtFileManager();
void Memory_SaveSignalToFlashDrive();
extern const void *pMspFileManager;
