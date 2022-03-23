// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Strings.h"
#include "SCPI/Map.h"



uint8 MapElement::GetValue(Word *eKey) const
{
    int numKey = 0;
    char *_key = this[numKey].key;

    while (_key != 0)
    {
        if (WordEqualZeroString(eKey, key))
        {
            return this[numKey].value;
        }
        numKey++;
        _key = this[numKey].key;
    }

    return 255;
}
