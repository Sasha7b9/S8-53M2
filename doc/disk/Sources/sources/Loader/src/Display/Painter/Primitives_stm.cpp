// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Painter/Primitives.h"
#include "Utils/Math.h"
#include "Display/Display.h"


namespace Display
{
    extern uint8 *display_back_buffer;
    extern uint8 *display_back_buffer_end;
}


void Point::Draw(int x, int y) const
{
    if (x < 0 || y < 0 || (x > Display::WIDTH - 1) || (y > Display::HEIGHT - 1))
    {
        return;
    }

    uint8 *address = Display::display_back_buffer + Display::WIDTH * y + x;

    if (address < Display::display_back_buffer_end)
    {
        *address = Color::Get().ValueForDraw();
    }
}


void VLine::Draw(int x, int y0, int y1) const
{
    if (x < 0 || x >= Display::WIDTH)
    {
        return;
    }

    BoundingY(y0);
    BoundingY(y1);

    Math::Sort(&y0, &y1);

    uint8 *address = Display::display_back_buffer + Display::WIDTH * y0 + x;

    int counter = y1 - y0 + 1;

    uint8 value = Color::Get().index;

    do 
    {
        *address = value;
        address += Display::WIDTH;

    } while (--counter > 0);
}


void VLine::Draw(int x, int y) const
{
    if (x < 0 || x >= Display::WIDTH)
    {
        return;
    }

    BoundingY(y);

    uint8 *address = Display::display_back_buffer + Display::WIDTH * y + x;

    uint8 value = Color::Get().index;

    for (int i = 0; i < height; i++)
    {
        *address = value;
        address += Display::WIDTH;
    }
}


void Region::Fill(int x, int y) const
{
    if (width == 0 || height == 0)
    {
        return;
    }

    for (int i = y; i <= y + height - 1; i++)
    {
        HLine().Draw(i, x, x + width - 1);
    }
}
