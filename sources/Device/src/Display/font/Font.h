// 2022/02/11 15:58:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


struct Symbol
{
    uchar width;
    uchar bytes[8];
};


struct Font
{
    int height;
    Symbol symbol[256];
};


extern const Font *font;
extern const Font *fonts[TypeFont::Count];

extern const uchar font5display[3080];
extern const uchar font8display[3080];
extern const uchar fontUGOdisplay[3080];
extern const uchar fontUGO2display[3080];


int Font_GetSize();
int Font_GetLengthText(pchar text);
int Font_GetHeightSymbol(char symbol);
int Font_GetLengthSymbol(uchar symbol);
