// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Strings.h"


struct MapElement
{
    char *key;
    uint8 value;
};


uint8 GetValueFromMap(const MapElement *map, Word *key);      // ≈сли значение не найдено, возвращеетс€ 255;
