// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "DisplayTypes.h"

#define MAKE_COLOR(r, g, b)  ((uint)(b + (g << 8) + (r << 16)))

extern Color::E gColorFill;
extern Color::E gColorBack;
