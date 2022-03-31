// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "common/Display/Colors_.h"
#include "common/Display/Painter/Painter_.h"
#include "common/Utils/Math_.h"


namespace Painter
{
    static Color currentColor = Color::Count;
}


uint Painter::ReduceBrightness(uint colorValue, float newBrightness)
{
    int red = Math::Limitation((int)(R_FROM_COLOR(colorValue) * newBrightness), 0, 255);

    int green = Math::Limitation((int)(G_FROM_COLOR(colorValue) * newBrightness), 0, 255);

    int blue = Math::Limitation((int)(B_FROM_COLOR(colorValue) * newBrightness), 0, 255);

    return Color::Make((uint8)(red), (uint8)(green), (uint8)(blue));
}
