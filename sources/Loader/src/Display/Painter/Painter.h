// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Display/Font/Font.h"


namespace Painter
{
    uint ReduceBrightness(uint colorValue, float newBrightness);

    bool SaveScreenToFlashDrive();
};
