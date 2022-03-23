// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsDisplay.h"
#include "Display/Warnings.h"


enum WarningWithNumber
{
    ExcessValues        // Превышение значения количества сигналов в "НАКОПЛЕНИЕ", "УСРЕДНЕНИЕ", "Мин Макс"
};


namespace Tables
{
    pchar GetWarning(Warning::E);

    // Выводит строку из таблицы symbolsAlphaBet
    void DrawStr(int index, int x, int y);

    extern pchar symbolsAlphaBet[0x48];
}

