// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsDisplay.h"
#include "Display/Warnings.h"


namespace Tables
{
    // Выводит строку из таблицы symbolsAlphaBet
    void DrawStr(int index, int x, int y);

    extern pchar symbolsAlphaBet[0x48];
}

