// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Colors.h"
#include "Display/font/Font.h"
#include "Display/String.h"
#include "Utils/Containers/Buffer.h"
#include "Hardware/InterCom.h"
#include "Display/Text.h"
#include "Display/Painter.h"
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


int String::Draw(int x, int y, Color::E color)
{
    Color::SetCurrent(color);

    if (Size() == 0)
    {
        return x;
    }

    y += (8 - Font::GetSize());

    pchar text = c_str();

    if (InterCom::TransmitGUIinProcess())
    {
        const int SIZE_BUFFER = 100;

        CommandBuffer command(SIZE_BUFFER, DRAW_TEXT);
        command.PushHalfWord(x);
        command.PushByte(y + 1);
        command.PushByte(0);

        uint8 length = 0;

        int counter = 0;
        while (*text && length < (SIZE_BUFFER - 7))
        {
            command.PushByte(*text);
            text++;
            length++;
            counter++;
        }

        if (*text != 0)
        {
            LOG_WRITE("big string - %s", text);
        }

        command.PushByte(0);
        *command.GetByte(4) = length;

        command.Transmit(1 + 2 + 1 + 1 + length);
    }

    text = c_str();

    while (*text != '\0')
    {
        x = Char(*text).Draw(x, y) + 1;
        text++;
    }

    return x;
}


int String::DrawInCenterRect(int eX, int eY, int width, int eHeight, Color::E color)
{
    Color::SetCurrent(color);

    int lenght = Font::GetLengthText(buffer);
    int height = Font::GetHeightSymbol(buffer[0]);
    int x = eX + (width - lenght) / 2;
    int y = eY + (eHeight - height) / 2;

    return Draw(x, y);
}


int String::DrawOnBackground(int x, int y, Color::E colorBackground)
{
    int width = Font::GetLengthText(buffer);
    int height = Font::GetSize();

    Color::E colorText = Color::GetCurrent();
    Region(width, height).Fill(x - 1, y, colorBackground);
    Color::SetCurrent(colorText);

    return Draw(x, y);
}


void String::DrawInRect(int x, int y, int width, int, int dy)
{
    int xStart = x;
    int xEnd = xStart + width;

    char *text = buffer;

    while (*text != 0)
    {
        int length = GetLenghtSubString(text);

        if (length + x > xEnd)
        {
            x = xStart;
            y += Font::GetHeightSymbol(*text);
            y += dy;
        }

        int numSymbols = 0;
        numSymbols = DrawSubString(x, y, text);
        text += numSymbols;
        x += length;
        x = DrawSpaces(x, y, text, &numSymbols);
        text += numSymbols;
    }
}


int String::GetLenghtSubString(char *text)
{
    int result = 0;

    while (((*text) != ' ') && ((*text) != '\0'))
    {
        result += Font::GetLengthSymbol((uint8)*text);
        text++;
        result++;
    }

    return result;
}


int String::DrawSubString(int x, int y, char *text)
{
    int numSymbols = 0;

    while (((*text) != ' ') && ((*text) != '\0'))
    {
        x = Char(*text).Draw(x, y);
        numSymbols++;
        text++;
        x++;
    }

    return numSymbols;
}


int String::DrawSpaces(int x, int y, char *text, int *numSymbols)
{
    *numSymbols = 0;

    while (*text == ' ')
    {
        x = Char(*text).Draw(x, y);
        text++;
        (*numSymbols)++;
    }

    return x;
}
