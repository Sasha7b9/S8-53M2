// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Strings.h"
#include "Utils/GlobalFunctions.h"
#include <cctype>
#include <cstring>


template void SU::LogBuffer<uint8>(const uint8 *buffer, int num);


static bool ChooseSymbols(const uint8 **string);    // Возвращает false, если выбор невозможен - строка кончилась.
static bool ChooseSpaces(const uint8 **string);     // Возвращает false, если выбор невозможен - строка кончилась.


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


template<class T>
void SU::LogBuffer(const T *data, int num)
{
    char buffer[1024];
    char buffer_number[20];

    buffer[0] = '\0';

    for (int i = 0; i < num; i++)
    {
        std::strcat(buffer, Int2String(data[i], false, 1, buffer_number));
        std::strcat(buffer, " ");
    }

    LOG_WRITE(buffer);
}
