// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "common/Display/Colors_.h"


void Color::SetAsCurrent() const
{
    if (index != Count)
    {
        current = *this;
    }
}
