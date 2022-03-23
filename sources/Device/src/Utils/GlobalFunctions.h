// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Display/Text.h"

char    GetSymbolForGovernor(int value);                        // Возвращает изображение регулятора, соответствующее его текущему положению.
void    EmptyFuncVV();                                      // Функция-заглушка. Ничего не делает.
void    EmptyFuncVI(int);
void    EmptyFuncpVII(void *, int, int);                        // Функция-заглушка. Ничего не делает.
bool    EmptyFuncBV();
