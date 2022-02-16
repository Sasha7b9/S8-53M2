// 2022/02/11 15:58:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/DisplayTypes.h"
#include "font.h"

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

TypeFont::E Font::current = TypeFont::None;



int Font::GetSize()
{
    return Font::font->height;
}



int Font::GetLengthText(pchar text)
{
    int retValue = 0;
    while (*text)
    {
        retValue += Font::GetLengthSymbol((uint8)*text);
        text++;
    }
    return retValue;
}



int Font::GetHeightSymbol(char symbol)
{
    return 9;
}



int Font::GetLengthSymbol(uchar symbol)
{
    return Font::font->symbol[symbol].width;
}
