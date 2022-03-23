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



bool IntInRange(int value, int min, int max)
{
    return (value >= min) && (value <= max);
}

float MaxFloat(float val1, float val2, float val3)
{
    float retValue = val1;
    if(val2 > retValue)
    {
        retValue = val2;
    }
    if(val3 > retValue)
    {
        retValue = val3;
    }
    return retValue;
}

int8 CircleIncreaseInt8(int8 *val, int8 min, int8 max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}

int16 CircleIncreaseInt16(int16 *val, int16 min, int16 max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}

int CircleIncreaseInt(int *val, int min, int max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}

int8 CircleDecreaseInt8(int8 *val, int8 min, int8 max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return *val;
}

int16 CircleDecreaseInt16(int16 *val, int16 min, int16 max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return (*val);
}

int CircleDecreaseInt(int *val, int min, int max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return (*val);
}

float CircleAddFloat(float *val, float delta, float min, float max)
{
    *val += delta;
    if(*val > max)
    {
        *val = min;
    }
    return *val;
}

float CircleSubFloat(float *val, float delta, float min, float max)
{
    *val -= delta;
    if(*val < min)
    {
        *val = max;
    }
    return *val;
}


void SwapInt(int *value0, int *value1)
{
    int temp = *value0;
    *value0 = *value1;
    *value1 = temp;
}

void SortInt(int *value0, int *value1)
{
    if(*value1 < *value0)
    {
        SwapInt(value0,  value1);
    }
}

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

void IntToStrCat(char *_buffer, int _value)
{
    static const int LENGHT = 10;
    char buffer[LENGHT];
    for(int i = 0; i < LENGHT; i++)
    {
        buffer[i] = 0;
    }
    int pointer = LENGHT -1;

    while(_value > 0)
    {
        buffer[pointer] = (int8)(_value % 10);
        _value /= 10;
        pointer--;
    }

    while(*_buffer)
    {
        _buffer++;
    }
    int i = 0;

    for(; i < LENGHT; i++)
    {
        if(buffer[i] > 0)
        {
            break;
        }
    }

    for(; i < LENGHT; i++)
    {
        *_buffer = 0x30 | buffer[i];
        _buffer++;
    }

    *_buffer = 0;
}
