// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Strings.h"
#include "Settings/Settings.h"
#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>


namespace SU
{
    static bool ChooseSymbols(const uint8 **string);    // ¬озвращает false, если выбор невозможен - строка кончилась.
    static bool ChooseSpaces(const uint8 **string);     // ¬озвращает false, если выбор невозможен - строка кончилась.

    static int NumDigitsInIntPart(float value);
}


static int SU::NumDigitsInIntPart(float value)
{
    float fabsValue = std::fabs(value);

    int numDigitsInInt = 0;

    if (fabsValue >= 10000)
    {
        numDigitsInInt = 5;
    }
    else if (fabsValue >= 1000)
    {
        numDigitsInInt = 4;
    }
    else if (fabsValue >= 100)
    {
        numDigitsInInt = 3;
    }
    else if (fabsValue >= 10)
    {
        numDigitsInInt = 2;
    }
    else
    {
        numDigitsInInt = 1;
    }

    return numDigitsInInt;
}



int GetNumWordsInString(const uint8 *string)
{
    SU::ChooseSpaces(&string);

    while (true)
    {
        int numWords = 0;
        
        if (SU::ChooseSymbols(&string))
        {
            numWords++;
        }
        else
        {
            return numWords;
        }
        SU::ChooseSpaces(&string);
    }
}

bool GetWord(const uint8 *string, Word *word, const int numWord)
{
    SU::ChooseSpaces(&string);

    int currentWord = 0;

    while (true)
    {
        if (currentWord == numWord)
        {
            word->address = (uint8*)string;
            SU::ChooseSymbols(&string);
            word->numSymbols = string - word->address;
            
            uint8 *pointer = word->address;
            int numSymbols = word->numSymbols;
            for (int i = 0; i < numSymbols; i++)
            {
                *pointer = (uint8)std::toupper((int8)*pointer);
                pointer++;
            }
            return true;
        }
        if (SU::ChooseSymbols(&string))
        {
            currentWord++;
        }
        else
        {
            return false;
        }
        SU::ChooseSpaces(&string);
    }
}

bool WordEqualZeroString(Word *word, char* string)
{
    char *ch = string;
    char *w = (char*)(word->address);

    while (*ch != 0)
    {
        if (*ch++ != *w++)
        {
            return false;
        }
    }

    return (ch - string) == word->numSymbols;
}

#define  SYMBOL(x) (*(*(x)))

bool SU::ChooseSymbols(const uint8 **string)
{
    if (SYMBOL(string) == 0x0d && SYMBOL(string + 1) == 0x0a)
    {
        return false;
    }

    while (SYMBOL(string) != ' ' && SYMBOL(string) != 0x0d && SYMBOL(string + 1) != 0x0a)
    {
        (*string)++;
    }

    return true;
}

bool SU::ChooseSpaces(const uint8 **string)
{
    if (SYMBOL(string) == 0x0d && SYMBOL(string + 1) == 0x0a)
    {
        return false;
    }

    while (SYMBOL(string) == ' ')
    {
        (*string)++;
    }

    return true;
}

#undef SYMBOL

bool EqualsStrings(char *str1, char *str2, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}


bool EqualsZeroStrings(char *str1, char *str2)
{
    while ((*str1) == (*str2))
    {
        if ((*str1) == '\0')
        {
            return true;
        }
        str1++;
        str2++;
    }
    return false;
}


void SU::LogBufferU8(const uint8 *data, int num)
{
    DString result;

    for (int i = 0; i < num; i++)
    {
        result.Append(SU::Int2String(data[i], false, 1));
        result.Append(" ");
    }

    LOG_WRITE(result.c_str());
}


void SU::LogBufferU8(pchar label, const uint8 *data, int num)
{
    DString result(label);
    result.Append(" ");

    for (int i = 0; i < num; i++)
    {
        result.Append(SU::Int2String(data[i], false, 1));
        result.Append(" ");
    }

    LOG_WRITE(result.c_str());
}


void SU::LogBufferF(pchar label, const float *data, int num)
{
    DString string(label);
    string.Append(" ");

    for (int i = 0; i < num; i++)
    {
        string.Append(SU::Float2String(data[i], false, 4));
        string.Append(" ");
    }

    LOG_WRITE(string.c_str());
}


DString SU::FloatFract2String(float value, bool alwaysSign)
{
    return Float2String(value, alwaysSign, 4);
}


DString SU::Bin2String(uint8 value)
{
    char buffer[9];

    for (int bit = 0; bit < 8; bit++)
    {
        buffer[7 - bit] = _GET_BIT(value, bit) ? '1' : '0';
    }

    return DString(buffer);
}


DString SU::Bin2String16(uint16 value)
{
    char buffer[19];

    std::strcpy(buffer, Bin2String((uint8)(value >> 8)).c_str());
    std::strcpy((buffer[8] = ' ', buffer + 9), Bin2String((uint8)value).c_str());

    return DString(buffer);
}


DString SU::Float2String(float value, bool alwaysSign, int numDigits)
{
    char bufferOut[20];
    char *pBuffer = bufferOut;

    if (value == ERROR_VALUE_FLOAT)
    {
        return DString(ERROR_STRING_VALUE);
    }

    if (!alwaysSign)
    {
        if (value < 0)
        {
            *pBuffer = '-';
            pBuffer++;
        }
    }
    else
    {
        *pBuffer = value < 0 ? '-' : '+';
        pBuffer++;
    }

    char format[] = "%4.2f\0\0";

    format[1] = (char)numDigits + 0x30;

    int numDigitsInInt = NumDigitsInIntPart(value);

    format[3] = (numDigits - numDigitsInInt) + 0x30;
    if (numDigits == numDigitsInInt)
    {
        format[5] = '.';
    }

    std::snprintf(pBuffer, 19, format, std::fabs(value));

    float val = std::atof(pBuffer);

    if (NumDigitsInIntPart(val) != numDigitsInInt)
    {
        numDigitsInInt = NumDigitsInIntPart(val);
        format[3] = (numDigits - numDigitsInInt) + 0x30;

        if (numDigits == numDigitsInInt)
        {
            format[5] = '.';
        }

        std::sprintf(pBuffer, format, value);
    }

    bool signExist = alwaysSign || value < 0;

    while ((int)std::strlen(bufferOut) < numDigits + (signExist ? 2 : 1))
    {
        std::strcat(bufferOut, "0");
    }

    return DString(bufferOut);
}


DString SU::Int2String(int value, bool alwaysSign, int numMinFields)
{
    char buffer[20];

    char format[20] = "%";
    std::sprintf(&(format[1]), "0%d", numMinFields);
    std::strcat(format, "d");

    if (alwaysSign && value >= 0)
    {
        buffer[0] = '+';
        std::sprintf(buffer + 1, format, value);
    }
    else
    {
        std::sprintf(buffer, format, value);
    }

    return DString(buffer);
}


DString SU::Hex8toString(uint8 value)
{
    char buffer[3];
    std::sprintf(value < 16 ? (buffer[0] = '0', buffer + 1) : (buffer), "%x", value);
    return DString(buffer);
}


bool SU::String2Int(char *str, int *value)
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

    for (int i = length - 1; i >= 0; i--)
    {
        int val = str[i] & (~(0x30));
        if (val < 0 || val > 9)
        {
            return false;
        }
        *value += val * pow;
        pow *= 10;
    }

    if (sign == -1)
    {
        *value *= -1;
    }

    return true;
}


DString SU::Voltage2String(float voltage, bool alwaysSign)
{
    char *suffix;

    if (voltage == ERROR_VALUE_FLOAT)
    {
        return DString(ERROR_STRING_VALUE);
    }
    else if (std::fabs(voltage) + 0.5e-4f < 1e-3f)
    {
        suffix = LANG_RU ? "\x10мк¬" : "\x10uV";
        voltage *= 1e6f;
    }
    else if (std::fabs(voltage) + 0.5e-4f < 1)
    {
        suffix = LANG_RU ? "\x10м¬" : "\x10mV";
        voltage *= 1e3f;
    }
    else if (std::fabs(voltage) + 0.5e-4f < 1000)
    {
        suffix = LANG_RU ? "\x10¬" : "\x10V";
    }
    else
    {
        suffix = LANG_RU ? "\x10к¬" : "\x10kV";
        voltage *= 1e-3f;
    }

    DString result = SU::Float2String(voltage, alwaysSign, 4);
    result.Append(suffix);

    return result;
}


DString SU::Time2String(float time, bool alwaysSign)
{
    char *suffix = 0;

    if (time == ERROR_VALUE_FLOAT)
    {
        return DString(ERROR_STRING_VALUE);
    }
    else if (std::fabs(time) + 0.5e-10f < 1e-6f)
    {
        suffix = LANG_RU ? "нс" : "ns";
        time *= 1e9f;
    }
    else if (std::fabs(time) + 0.5e-7f < 1e-3f)
    {
        suffix = LANG_RU ? "мкс" : "us";
        time *= 1e6f;
    }
    else if (std::fabs(time) + 0.5e-3f < 1)
    {
        suffix = LANG_RU ? "мс" : "ms";
        time *= 1e3f;
    }
    else
    {
        suffix = LANG_RU ? "с" : "s";
    }

    DString result = SU::Float2String(time, alwaysSign, 4);
    result.Append(suffix);

    return result;
}


DString SU::Freq2String(float freq, bool)
{
    DString result;

    char *suffix = 0;

    if (freq == ERROR_VALUE_FLOAT)
    {
        result.Append(ERROR_STRING_VALUE);
        return result;
    }

    if (freq >= 1e6f)
    {
        suffix = LANG_RU ? "ћ√ц" : "MHz";
        freq /= 1e6f;
    }
    else if (freq >= 1e3f)
    {
        suffix = LANG_RU ? "к√ц" : "kHz";
        freq /= 1e3f;
    }
    else
    {
        suffix = LANG_RU ? "√ц" : "Hz";
    }

    result.Append(SU::Float2String(freq, false, 4));
    result.Append(suffix);

    return result;
}


DString SU::Phase2String(float phase, bool)
{
    DString result = SU::Float2String(phase, false, 4);
    result.Append("\xa8");
    return result;
}


DString SU::Float2Db(float value, int numDigits)
{
    DString result = SU::Float2String(value, false, numDigits);

    result.Append(LANG_RU ? "дЅ" : "dB");

    return result;
}
