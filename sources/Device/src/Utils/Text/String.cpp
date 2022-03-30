// 2022/03/30 08:25:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/font/Font.h"
#include "Hardware/InterCom.h"
#include "Display/Painter.h"
#include "Utils/Text/String.h"
#include "Utils/Text/Text.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cstdlib>


template      String<(int)DEFAULT_SIZE_STRING>::String(pchar, ...);
template int  String<(int)DEFAULT_SIZE_STRING>::Draw(int, int, Color::E);
template void String<(int)DEFAULT_SIZE_STRING>::Append(pchar);
template int  String<(int)DEFAULT_SIZE_STRING>::DrawInCenterRect(int x, int y, int width, int height, Color::E);
template int  String<(int)DEFAULT_SIZE_STRING>::DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth,
    int limitHeight);
template void String<(int)DEFAULT_SIZE_STRING>::DrawRelativelyRight(int xRight, int y, Color::E);
template int  String<(int)DEFAULT_SIZE_STRING>::DrawOnBackground(int x, int y, Color::E colorBackground);
template void String<(int)DEFAULT_SIZE_STRING>::DrawInRect(int x, int y, int width, int height, int dy);
template void String<(int)DEFAULT_SIZE_STRING>::SetFormat(pchar format, ...);
template int  String<(int)DEFAULT_SIZE_STRING>::DrawInBoundedRectWithTransfers(int x, int y, int width, Color::E colorBackground,
    Color::E colorFill);
template int  String<(int)DEFAULT_SIZE_STRING>::DrawInRectWithTransfers(int x, int y, int width, int height, Color::E);
template int  String<(int)DEFAULT_SIZE_STRING>::DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height,
    Color::E colorBackground, Color::E colorFill);
template void String<(int)DEFAULT_SIZE_STRING>::Append(char);
template void String<(int)DEFAULT_SIZE_STRING>::DrawInCenterRectOnBackground(int x, int y, int width, int height, Color::E colorText,
    int widthBorder, Color::E colorBackground);

template      String<(int)1024>::String(pchar, ...);
template int  String<(int)1024>::Draw(int, int, Color::E);


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


template<int capacity>
String<capacity>::String(pchar format, ...)
{
    char temp_buffer[1024];

    std::va_list args;
    va_start(args, format);
    int num_symbols = std::vsprintf(temp_buffer, format, args);
    va_end(args);

    if (capacity < num_symbols - 1)
    {
        LOG_ERROR_TRACE("Very small string buffer %d, need %d:", capacity, num_symbols);
        LOG_WRITE(temp_buffer);

        temp_buffer[1023] = '\0';
    }

    std::strcpy(buffer, temp_buffer);
}


template<int capacity>
void String<capacity>::SetFormat(pchar format, ...)
{
    char temp_buffer[1024];

    std::va_list args;
    va_start(args, format);
    int num_symbols = std::vsprintf(temp_buffer, format, args);
    va_end(args);

    if (capacity < num_symbols - 1)
    {
        LOG_ERROR_TRACE("Very small string buffer %d, need %d:", capacity, num_symbols);
        LOG_WRITE(temp_buffer);

        temp_buffer[1023] = '\0';
    }

    std::strcpy(buffer, temp_buffer);
}


template<int capacity>
void String<capacity>::Append(pchar str)
{
    int need_size = Size() + (int)std::strlen(str) + 1;

    if (need_size > capacity)
    {
        LOG_ERROR_TRACE("Very small string buffer %d, need %d:", capacity, need_size);

        int pointer = 0;

        while (!Filled())
        {
            Append(str[pointer++]);
        }

        LOG_WRITE(buffer);
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
        x = Text::DrawCharWithLimitation(x, y, (uint8)*text, limitX, limitY, limitWidth, limitHeight);
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


template<int capacity>
void String<capacity>::DrawInRect(int x, int y, int width, int, int dy)
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


template<int capacity>
int String<capacity>::GetLenghtSubString(char *text)
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


template<int capacity>
int String<capacity>::DrawSubString(int x, int y, char *text)
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


template<int capacity>
int String<capacity>::DrawSpaces(int x, int y, char *text, int *numSymbols)
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


template<int capacity>
int String<capacity>::DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, Color::E colorBackground,
    Color::E colorFill)
{
    Rectangle(width, height).Draw(x, y, colorFill);
    Region(width - 2, height - 2).Fill(x + 1, y + 1, colorBackground);
    Color::SetCurrent(colorFill);
    return DrawInCenterRect(x, y, width, height);
}


template<int capacity>
int String<capacity>::DrawInBoundedRectWithTransfers(int x, int y, int width, Color::E colorBackground, Color::E colorFill)
{
    int height = 0;

    Text::GetHeightTextWithTransfers(x + 3, y + 3, x + width - 8, buffer, &height);

    Rectangle(width, height).Draw(x, y, colorFill);
    Region(width - 2, height - 2).Fill(x + 1, y + 1, colorBackground);
    DrawInRectWithTransfers(x + 3, y + 3, width - 8, height, colorFill);
    return y + height;
}


template<int capacity>
int String<capacity>::DrawInRectWithTransfers(int eX, int eY, int eWidth, int eHeight, Color::E color)
{
    Color::SetCurrent(color);

    int top = eY;
    int left = eX;
    int right = eX + eWidth;
    int bottom = eY + eHeight;

    pchar text = buffer;

    int numSymb = (int)std::strlen(text);

    int y = top - 1;
    int x = left;

    int curSymbol = 0;

    while (y < bottom && curSymbol < numSymb)
    {
        while (x < right - 1 && curSymbol < numSymb)
        {
            int length = 0;
            String<> word = Text::GetWord(text + curSymbol, &length);

            if (length <= 1)                            // ��� ��������� �������� ��� ����, �.�. ����� �� �������
            {
                char symbol = text[curSymbol++];
                if (symbol == '\n')
                {
                    x = right;
                    continue;
                }
                if (symbol == ' ' && x == left)
                {
                    continue;
                }
                x = Char(symbol).Draw(x, y);
            }
            else                                            // � ����� ������� �� ������� ���� ��� ��������� �������, �.�. ������� �����
            {
                int lengthString = Font::GetLengthText(word.c_str());
                if (x + lengthString > right + 5)
                {
                    int numSymbols = Text::DrawPartWord(word.c_str(), x, y, right, true);
                    x = right;
                    curSymbol += numSymbols;
                    continue;
                }
                else
                {
                    curSymbol += length;
                    x = String<>(word).Draw(x, y);
                }
            }
        }
        x = left;
        y += 9;
    }

    return y;
}


template<int capacity>
void String<capacity>::DrawInCenterRectOnBackground(int x, int y, int width, int height, Color::E colorText,
    int widthBorder, Color::E colorBackground)
{
    int lenght = Font::GetLengthText(buffer);
    int eX = DrawInCenterRect(x, y, width, height, colorBackground);
    int w = lenght + widthBorder * 2 - 2;
    int h = 7 + widthBorder * 2 - 1;
    Region(w, h).Fill(eX - lenght - widthBorder, y - widthBorder + 1);
    DrawInCenterRect(x, y, width, height, colorText);
}
