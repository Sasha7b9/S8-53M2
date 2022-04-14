// 2021/07/02 9:35:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Painter/Animated.h"
#include "Display/Painter/Primitives.h"
#include "Hardware/HAL/HAL.h"


ABorder::ABorder(int _w, int _h, DirectionRotate::E _rotate) : width(_w), height(_h), rotate(_rotate)
{

}


void ABorder::Draw(int x, int y)
{
    static const int numPoints = 38;

    for (int i = 0; i < numPoints; i++)
    {
        int value = (int)(TIMER_MS / 10);

        for (int num = 0; num < 13; num++)
        {
            BitSet64 coord = CalculateCoord(value + i * (GetPerimeter() / numPoints) + num);

            Point().Draw(x + coord.first, y + coord.second, Color::FILL);
        }
    }
}


BitSet64 ABorder::CalculateCoord(int value)
{
    value %= (GetPerimeter());

    if (rotate == DirectionRotate::Left)
    {
        value = GetPerimeter() - value;
    }

    if (value < width)
    {
        return BitSet64(value, 0);
    }
    else if (value < width + height)
    {
        return BitSet64(width - 1, value - width);
    }
    else if (value < width * 2 + height)
    {
        return BitSet64(width - (value - width - height) - 1, height - 1);
    }

    return BitSet64(0, height - (value - width * 2 - height) - 1);
}


int ABorder::GetPerimeter()
{
    return width * 2 + height * 2;
}

