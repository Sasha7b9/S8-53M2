// 2022/2/11 19:47:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsTypes.h"


void  Cursors_Update();    // Вызываем эту функцию для каждого измерения, чтобы обновить положие курсоров, если они должны обновляться автоматически.


class PageCursors
{
public:
    
    class PageSet
    {
    public:
        static void *pointer;
    };

    static void *GetPointer();
};
