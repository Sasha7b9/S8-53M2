// 2021/04/27 11:49:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Memory/Sector.h"


const Sector &Sector::Get(Sector::E number)
{
    return HAL_ROM::sectors[number];
}
