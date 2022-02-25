// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Colors.h"
#include "Display/font/Font.h"
//#include "Display/Painter/Primitives.h"
//#include "Display/Painter/Text.h"
#include "Utils/Containers/String.h"
#include "Utils/Containers/Buffer.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>


pchar const String::_ERROR = "---.---";


String::String() : buffer(nullptr), capacity(0)
{
    SetString("");
}


String::String(const String &rhs) : buffer(nullptr), capacity(0)
{
    SetString(rhs);
}


String::String(char symbol) : buffer(nullptr), capacity(0)
{
    if (Allocate(2))
    {
        buffer[0] = symbol;
        buffer[1] = 0;
    }
}


String::String(pchar format, ...) : buffer(nullptr), capacity(0)
{
    char temp_buffer[1024];

    std::va_list args;
    va_start(args, format);
    int num_symbols = std::vsprintf(temp_buffer, format, args);
    va_end(args);

    if (capacity < num_symbols)
    {
        Allocate(num_symbols);
    }

    if (buffer)
    {
        std::strcpy(buffer, temp_buffer);
    }
}


void String::SetFormat(pchar format, ...)
{
    char temp_buffer[1024];

    std::va_list args;
    va_start(args, format);
    int num_symbols = std::vsprintf(temp_buffer, format, args);
    va_end(args);

    if (num_symbols == 0)
    {
        num_symbols = 2;
    }

    if (capacity < num_symbols)
    {
        Allocate(num_symbols);
    }

    if (buffer)
    {
        std::strcpy(buffer, temp_buffer);
    }
}


void String::SetString(pchar string)
{
    Allocate((int)std::strlen(string) + 1);

    std::strcpy(buffer, string);
}


void String::SetString(const String &str)
{
    SetString(str.c_str());
}


void String::Append(pchar str)
{
    Append(str, (int)std::strlen(str));
}


void String::Append(const String &str)
{
    Append(str.c_str());
}


void String::Append(pchar str, int num_symbols)
{
    if (!str || *str == '\0')
    {
        return;
    }

    int need_size = Size() + num_symbols + 1;

    if (capacity < need_size)
    {
        String old(*this);
        Allocate(need_size);
        std::strcpy(buffer, old.c_str());
    }

    std::memcpy(buffer + std::strlen(buffer), str, (uint)num_symbols);
    buffer[need_size - 1] = '\0';
}


void String::Append(char symbol)
{
    char b[2] = { symbol, '\0' };
    Append(b);
}


String::~String()
{
    Free();
}


void String::Free()
{
    if(buffer)
    {
        std::free(buffer);
        buffer = nullptr;
        capacity = 0;
    }
}


bool String::Allocate(int size)
{
    std::free(buffer);

    capacity = NeedMemory(size);

    buffer = (char *)(std::malloc((uint)capacity));

    if (buffer)
    {
        return true;
    }

    capacity = 0;

    return false;
}


int String::NeedMemory(int size)
{
    if (size % SIZE_SEGMENT == 0)
    {
        return size;
    }
    else
    {
        return (size / SIZE_SEGMENT) * SIZE_SEGMENT + SIZE_SEGMENT;
    }
}


int String::Size() const
{
    if (buffer == nullptr)
    {
        return 0;
    }

    return (int)std::strlen(buffer);
}


char &String::operator[](int i) const
{
    static char result = 0;

    if (buffer == nullptr || Size() < (int)i)
    {
        return result;
    }

    return buffer[i];
}


bool String::ToInt(int *out)
{
    char *str = buffer;

    int sign = str[0] == '-' ? -1 : 1;

    if (str[0] < '0' || str[0] > '9')
    {
        ++str;
    }

    int length = (int)(std::strlen(str));

    if (length == 0)
    {
        return false;
    }

    *out = 0;

    int pow = 1;

    for (int i = length - 1; i >= 0; i--)
    {
        int val = str[i] & (~(0x30));
        if (val < 0 || val > 9)
        {
            return false;
        }
        *out += val * pow;
        pow *= 10;
    }

    if (sign == -1)
    {
        *out *= -1;
    }

    return true;
}


String &String::operator=(const String &rhs)
{
    SetString(rhs);

    return *this;
}
