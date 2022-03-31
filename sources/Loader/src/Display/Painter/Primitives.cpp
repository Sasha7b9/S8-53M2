// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Painter/Primitives.h"


void MultiVPointLine::DrawVPointLine(int x, int y, int count, int delta)
{
    for (int i = 0; i < count; i++)
    {
        Point().Draw(x, y);
        y += delta;
    }
}


void MultiHPointLine::DrawHPointLine(int x, int y, int count, int delta)
{
    for (int i = 0; i < count; i++)
    {
        Point().Draw(x, y);
        x += delta;
    }
}


void VPointLine::Draw(int x, int y0, int y1) const
{
    for (int y = y0; y <= y1; y += (int)delta)
    {
        Point().Draw(x, y);
    }
}


void HPointLine::Draw(int y, int x0, int x1) const
{
    for (int x = x0; x <= x1; x += (int)delta)
    {
        Point().Draw(x, y);
    }
}


void Line::Draw(int x0, int y0, int x1, int y1, const Color &color) const
{
    color.SetAsCurrent();

    Draw(x0, y0, x1, y1);
}


void Line::Draw(int x0, int y0, int x1, int y1) const
{
    if (x0 == x1)
    {
        HLine().Draw(x0, y0, y1);
    }
    else if (y0 == y1)
    {
        HLine().Draw(y0, x0, x1);
    }
}


void DashedHLine::Draw(int y, int x0, int x1, int deltaStart) const
{
    if (deltaStart < 0 || deltaStart >= (fill + empty))
    {
        LOG_ERROR("Неправильный аргумент deltaStart = %d", deltaStart);
        return;
    }
    int x = x0;
    if (deltaStart != 0)                // Если линию нужно рисовать не с начала штриха
    {
        x += (fill + empty - deltaStart);
        if (deltaStart < fill)     // Если начало линии приходится на штрих
        {
            HLine().Draw(y, x0, x - 1);
        }
    }

    while (x < x1)
    {
        HLine().Draw(y, x, x + fill - 1);
        x += (fill + empty);
    }
}


void DashedVLine::Draw(int x, int y0, int y1, int deltaStart) const
{
    if (deltaStart < 0 || deltaStart >= (fill + empty))
    {
        LOG_ERROR("Неправильный аргумент deltaStart = %d", deltaStart);
        return;
    }
    int y = y0;
    if (deltaStart != 0)                 // Если линию нужно рисовать не с начала штриха
    {
        y += (fill + empty - deltaStart);
        if (deltaStart < fill)     // Если начало линии приходится на штрих
        {
            VLine().Draw(x, y0, y - 1);
        }
    }

    while (y < y1)
    {
        VLine().Draw(x, y, y + fill - 1);
        y += (fill + empty);
    }
}


void VLineArray::Draw(int x, int numLines, uint8 *y0y1, const Color &color) const
{
    color.SetAsCurrent();

    for (; numLines > 0; numLines--)
    {
        int y0 = *y0y1++;
        int y1 = *y0y1++;

        VLine().Draw(x++, y0, y1);
    }
}


void VLine::Draw(int x, int y0, int y1, const Color &color) const
{
    color.SetAsCurrent();

    Draw(x, y0, y1);
}


void VLine::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    Draw(x, y);
}


void MultiVPointLine::Draw(int y) const
{
    for (int i = 0; i < numLines; i++)
    {
        DrawVPointLine(x0[i], y, count, delta);
    }
}


void Rectangle::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    Draw(x, y);
}


void Rectangle::Draw(int x, int y) const
{
    if (width == 0 || height == 0)
    {
        return;
    }

    HLine h_line(width);
    h_line.Draw(x, y);
    h_line.Draw(x, y + height - 1);

    VLine v_line(height);
    v_line.Draw(x, y);
    v_line.Draw(x + width - 1, y);
}


void MultiHPointLine::Draw(int x) const
{
    for (int i = 0; i < numLines; i++)
    {
        DrawHPointLine(x, y[i], count, delta);
    }
}


void Point::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    Draw(x, y);
}


void Region::Fill(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    Fill(x, y);
}


void Region::FillBounded(int x, int y, const Color &fill, const Color &bounded)
{
    Fill(x, y, fill);
    Rectangle(width, height).Draw(x, y, bounded);
}


void HLine::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    Draw(x, y);
}


void HLine::Draw(int y, int x1, int x2, const Color &color) const
{
    color.SetAsCurrent();

    Draw(y, x1, x2);
}
