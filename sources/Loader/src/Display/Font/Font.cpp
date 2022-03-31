// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "common/Display/Font/Font_.h"

#include "font8display.inc"
#include "font5display.inc"
#include "fontUGOdisplay.inc"
#include "fontUGO2display.inc"

#include "font8.inc"
#include "font5.inc"
#include "fontUGO.inc"
#include "fontUGO2.inc"



const Font *Font::fonts[TypeFont::Count] = {&font5, &font8, &fontUGO, &fontUGO2};
const Font *Font::font = &font8;



int Font::GetSize()
{
    return font->height;
}



int Font::GetLengthText(pchar text)
{
    int retValue = 0;

    while (*text)
    {
        retValue += Font::GetLengthSymbol((uint8)(*text));
        text++;
    }

    return retValue;
}



int Font::GetHeightSymbol()
{
    return 9;
}



int Font::GetLengthSymbol(uchar symbol)
{
    return font->symbol[symbol].width + 1;
}


void Font::Set(TypeFont::E typeFont)
{
    font = fonts[typeFont];
}
