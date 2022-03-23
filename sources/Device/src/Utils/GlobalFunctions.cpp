// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/GlobalFunctions.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Display/Symbols.h"
#include "Utils/Math.h"
#include "Utils/Strings.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>




char GetSymbolForGovernor(int value)
{
    static const char chars[] =
    {
        SYMBOL_GOVERNOR_SHIFT_0,
        SYMBOL_GOVERNOR_SHIFT_1,
        SYMBOL_GOVERNOR_SHIFT_2,
        SYMBOL_GOVERNOR_SHIFT_3
    };
    while(value < 0)
    {
        value += 4;
    }
    return chars[value % 4];
}

void EmptyFuncVV() { }

void EmptyFuncVI(int) { }

void EmptyFuncVpV(void *) { }

void EmptyFuncpVII(void *, int, int) { }

void EmptyFuncVI16(int16) {}

void EmptyFuncVB(bool) {}

bool EmptyFuncBV()
{
    return true;
}

