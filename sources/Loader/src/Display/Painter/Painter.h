// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "common/Display/Colors_.h"
#include "common/Display/Font/Font_.h"


namespace Painter
{
    uint ReduceBrightness(uint colorValue, float newBrightness);

    bool SaveScreenToFlashDrive();
};
