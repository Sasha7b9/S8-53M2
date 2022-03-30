// 2022/03/30 08:25:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/String.h"
#include "Display/font/Font.h"
#include "Hardware/InterCom.h"
#include "Display/Text.h"


template int String<(int)32>::Draw(int, int, Color::E);


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
