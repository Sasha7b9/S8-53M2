// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Strings.h"
#include "Utils/GlobalFunctions.h"
#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>


static bool ChooseSymbols(const uint8 **string);    // Возвращает false, если выбор невозможен - строка кончилась.
static bool ChooseSpaces(const uint8 **string);     // Возвращает false, если выбор невозможен - строка кончилась.


namespace SU
{
    static int NumDigitsInIntPart(float value)
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
}


int GetNumWordsInString(const uint8 *string)
{

    ChooseSpaces(&string);

    while (true)
    {
        int numWords = 0;
        
        if (ChooseSymbols(&string))
        {
            numWords++;
        }
        else
        {
            return numWords;
        }
        ChooseSpaces(&string);
    }
}

bool GetWord(const uint8 *string, Word *word, const int numWord)
{
    ChooseSpaces(&string);

    int currentWord = 0;

    while (true)
    {
        if (currentWord == numWord)
        {
            word->address = (uint8*)string;
            ChooseSymbols(&string);
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
        if (ChooseSymbols(&string))
        {
            currentWord++;
        }
        else
        {
            return false;
        }
        ChooseSpaces(&string);
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

bool ChooseSymbols(const uint8 **string)
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

bool ChooseSpaces(const uint8 **string)
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
    char buffer[1024];
    buffer[0] = '\0';

    for (int i = 0; i < num; i++)
    {
        std::strcat(buffer, SU::Int2String(data[i], false, 1).c_str());
        std::strcat(buffer, " ");
    }

    LOG_WRITE(buffer);
}


void SU::LogBufferU8(pchar label, const uint8 *data, int num)
{
    char buffer[1024];

    buffer[0] = '\0';

    std::strcat(buffer, label);
    std::strcat(buffer, " ");

    for (int i = 0; i < num; i++)
    {
        std::strcat(buffer, SU::Int2String(data[i], false, 1).c_str());
        std::strcat(buffer, " ");
    }

    LOG_WRITE(buffer);
}


void SU::LogBufferF(pchar label, const float *data, int num)
{
    char buffer[1024];

    buffer[0] = '\0';

    std::strcat(buffer, label);
    std::strcat(buffer, " ");

    for (int i = 0; i < num; i++)
    {
        std::strcat(buffer, SU::Float2String(data[i], false, 1).c_str());
        std::strcat(buffer, " ");
    }

    LOG_WRITE(buffer);
}


String SU::FloatFract2String(float value, bool alwaysSign)
{
    return Float2String(value, alwaysSign, 4);
}


String SU::Bin2String(uint8 value)
{
    char buffer[9];

    for (int bit = 0; bit < 8; bit++)
    {
        buffer[7 - bit] = _GET_BIT(value, bit) ? '1' : '0';
    }

    return String(buffer);
}


String SU::Bin2String16(uint16 value)
{
    char buffer[19];

    std::strcpy(buffer, Bin2String((uint8)(value >> 8)).c_str());
    std::strcpy((buffer[8] = ' ', buffer + 9), Bin2String((uint8)value).c_str());

    return String(buffer);
}


String SU::Float2String(float value, bool alwaysSign, int numDigits)
{
    char bufferOut[20];
    char *pBuffer = bufferOut;

    if (value == ERROR_VALUE_FLOAT)
    {
        return String(ERROR_STRING_VALUE);
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

    return String(bufferOut);
}


String SU::Int2String(int value, bool alwaysSign, int numMinFields)
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

    return String(buffer);
}
