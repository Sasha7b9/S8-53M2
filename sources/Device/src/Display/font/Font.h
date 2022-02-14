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

    static const Font *font;
    static const Font *fonts[TypeFont::Count];

    static int GetSize();
    static int GetLengthText(pchar text);
    static int GetHeightSymbol(char symbol);
    static int GetLengthSymbol(uchar symbol);

    static void Set(TypeFont::E);

    // Загрузить шрифта в дисплей.
    static void Load(TypeFont::E);

    static TypeFont::E current;
};

extern const uchar font5display[3080];
extern const uchar font8display[3080];
extern const uchar fontUGOdisplay[3080];
extern const uchar fontUGO2display[3080];
