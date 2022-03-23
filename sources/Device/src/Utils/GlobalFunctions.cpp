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


bool String2Int(char *str, int *value)  
{
    int sign = str[0] == '-' ? -1 : 1;

    if (str[0] < '0' || str[0] > '9')
    {
        str++;
    }

    int length = (int)std::strlen(str);

    if (length == 0)
    {
        return false;
    }

    *value = 0;
    int pow = 1;

    for(int i = length - 1; i >= 0; i--)
    {
        int val = str[i] & (~(0x30));
        if(val < 0 || val > 9)
        {
            return false;
        }
        *value += val * pow;
        pow *= 10;
    }

    if(sign == -1)
    {
        *value *= -1;
    }

    return true;
}


String Hex8toString(uint8 value)
{
    char buffer[3];
    std::sprintf(value < 16 ? (buffer[0] = '0', buffer + 1) :  (buffer), "%x", value);
    return String(buffer);
}

String Voltage2String(float voltage, bool alwaysSign)
{
    char *suffix;

    if(voltage == ERROR_VALUE_FLOAT)
    {
        return String(ERROR_STRING_VALUE);
    }
    else if(std::fabs(voltage) + 0.5e-4f < 1e-3f)
    {
        suffix = LANG_RU ? "\x10ìêÂ" : "\x10uV";
        voltage *= 1e6f;
    }
    else if(std::fabs(voltage) + 0.5e-4f < 1)
    {
        suffix = LANG_RU ? "\x10ìÂ" : "\x10mV" ;
        voltage *= 1e3f;
    }
    else if(std::fabs(voltage) + 0.5e-4f < 1000)
    {
        suffix = LANG_RU ? "\x10Â" : "\x10V";
    }
    else
    {
        suffix = LANG_RU ? "\x10êÂ" : "\x10kV";
        voltage *= 1e-3f;
    }

    String result = SU::Float2String(voltage, alwaysSign, 4);
    result.Append(suffix);

    return result;
}

String Time2String(float time, bool alwaysSign)
{
    char *suffix = 0;

    if(time == ERROR_VALUE_FLOAT)
    {
        return String(ERROR_STRING_VALUE);
    }
    else if(std::fabs(time) + 0.5e-10f < 1e-6f)
    {
        suffix = LANG_RU ? "íñ" : "ns";
        time *= 1e9f;
    }
    else if(std::fabs(time) + 0.5e-7f < 1e-3f)
    {
        suffix = LANG_RU ? "ìêñ" : "us";
        time *= 1e6f;
    }
    else if(std::fabs(time) + 0.5e-3f < 1)
    {
        suffix = LANG_RU ? "ìñ" : "ms";
        time *= 1e3f;
    }
    else
    {
        suffix = LANG_RU ? "ñ" : "s";
    }

    String result = SU::Float2String(time, alwaysSign, 4);
    result.Append(suffix);

    return result;
}

String Phase2String(float phase, bool)
{
    String result = SU::Float2String(phase, false, 4);
    result.Append("\xa8");
    return result;
}

String Freq2String(float freq, bool)
{
    String result;

    char *suffix = 0;

    if(freq == ERROR_VALUE_FLOAT)
    {
        result.Append(ERROR_STRING_VALUE);
        return result;
    }

    if(freq >= 1e6f)
    {
        suffix = LANG_RU ? "ÌÃö" : "MHz";
        freq /= 1e6f;
    }
    else if (freq >= 1e3f)
    {
        suffix = LANG_RU ? "êÃö" : "kHz";
        freq /= 1e3f;
    }
    else
    {
        suffix = LANG_RU ? "Ãö" : "Hz";
    }

    result.Append(SU::Float2String(freq, false, 4));
    result.Append(suffix);

    return result;
}

String Float2Db(float value, int numDigits)
{
    String result = SU::Float2String(value, false, numDigits);

    result.Append(LANG_RU ? "äÁ" : "dB");

    return result;
}

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
