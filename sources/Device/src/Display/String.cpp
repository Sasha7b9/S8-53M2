// 2022/03/30 08:25:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/String.h"
#include "Display/font/Font.h"
#include "Hardware/InterCom.h"
#include "Display/Text.h"
#include "Display/Painter.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cstdlib>


template      String<(int)32>::String(pchar, ...);
template int  String<(int)32>::Draw(int, int, Color::E);
template void String<(int)32>::Append(pchar);
template int  String<(int)32>::DrawInCenterRect(int x, int y, int width, int height, Color::E);
template int  String<(int)32>::DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight);
template void String<(int)32>::DrawRelativelyRight(int xRight, int y, Color::E);
template int  String<(int)32>::DrawOnBackground(int x, int y, Color::E colorBackground);


template<int capa>
int String<capa>::Draw(int x, int y, Color::E color)
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

        CommandBuffer<SIZE_BUFFER> command(DRAW_TEXT);
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


template<int capa>
String<capa>::String(pchar format, ...)
{
    buffer[0] = '\0';

    char temp_buffer[capa];

    std::va_list args;
    va_start(args, format);
    int num_symbols = std::vsprintf(temp_buffer, format, args);
    va_end(args);

    if (capa < num_symbols - 1)
    {
        LOG_ERROR_TRACE("Very small string buffer %d, need %d", capa, num_symbols);
    }

    std::strcpy(buffer, temp_buffer);
}


template<int capacity>
void String<capacity>::Append(pchar str)
{
    int need_size = Size() + (int)std::strlen(str) + 1;

    if (need_size > capacity)
    {
        LOG_ERROR_TRACE("Very small string buffer %d, need %d", capacity, need_size);

        int pointer = 0;

        while (!Filled())
        {
            Append(str[pointer++]);
        }
    }
    else
    {
        std::strcat(buffer, str);
    }
}


template<int capacity>
void String<capacity>::Append(char symbol)
{
    if (!Filled())
    {
        int pos = (int)std::strlen(buffer);
        buffer[pos] = symbol;
        buffer[pos + 1] = '\0';
    }
    else
    {
        LOG_ERROR_TRACE("buffer is full");
    }
}


template<int capacity>
int String<capacity>::DrawInCenterRect(int eX, int eY, int width, int eHeight, Color::E color)
{
    Color::SetCurrent(color);

    int lenght = Font::GetLengthText(buffer);
    int height = Font::GetHeightSymbol(buffer[0]);
    int x = eX + (width - lenght) / 2;
    int y = eY + (eHeight - height) / 2;

    return Draw(x, y);
}


template<int capacity>
int String<capacity>::DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight)
{
    Color::SetCurrent(color);
    int retValue = x;

    char *text = buffer;

    while (*text)
    {
        x = PText::DrawCharWithLimitation(x, y, (uint8)*text, limitX, limitY, limitWidth, limitHeight);
        retValue += Font::GetLengthSymbol((uint8)*text);
        text++;
    }

    return retValue + 1;
}


template<int capacity>
void String<capacity>::DrawRelativelyRight(int xRight, int y, Color::E color)
{
    Color::SetCurrent(color);

    int lenght = Font::GetLengthText(buffer);
    Draw(xRight - lenght, y);
}


template<int capacity>
int String<capacity>::DrawOnBackground(int x, int y, Color::E colorBackground)
{
    int width = Font::GetLengthText(buffer);
    int height = Font::GetSize();

    Color::E colorText = Color::GetCurrent();
    Region(width, height).Fill(x - 1, y, colorBackground);
    Color::SetCurrent(colorText);

    return Draw(x, y);
}
