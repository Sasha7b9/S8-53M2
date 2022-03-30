// 2022/02/15 09:14:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"


class Char
{
public:
    Char(char s) : symbol(s) {}
    int Draw(int x, int y, Color::E = Color::Count);
private:
    char symbol;
};


namespace Text
{
    // Возвращает высоту экрана, которую займёт текст text, при выводе от left до right в переменной height. Если
    // bool == false, то текст не влезет на экран 
    bool GetHeightTextWithTransfers(int left, int top, int right, pchar text, int *height);

    int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight);

    void Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

    void Draw4SymbolsInRect(int x, int y, char eChar, Color::E = Color::Count);

    void Draw10SymbolsInRect(int x, int y, char eChar);

    void DrawBig(int x, int y, int size, pchar text);

    String<> GetWord(pchar firstSymbol, int *length);

    // Если draw == false, то рисовать символ не надо, фунция используется только для вычислений
    int DrawPartWord(char *word, int x, int y, int xRight, bool draw);
}
