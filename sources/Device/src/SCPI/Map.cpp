// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Strings.h"
#include "SCPI/Map.h"



uint8 GetValueFromMap(const MapElement *map, Word *eKey)
{
    int numKey = 0;
    char *key = map[numKey].key;
    while (key != 0)
    {
        if (WordEqualZeroString(eKey, key))
        {
            return map[numKey].value;
        }
        numKey++;
        key = map[numKey].key;
    }
    return 255;
}
