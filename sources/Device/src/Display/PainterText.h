// 2022/02/14 10:28:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


class Char
{
public:

    Char(char s) : symbol(s) {}

    int Draw(int x, int y, const Color::E color = Color::COUNT) const;

    int Draw(int x, int y, TypeFont::E typeFont, const Color::E color = Color::COUNT) const;

    void Draw2SymbolsInPosition(int x, int y, char symbol2, const Color::E color1, const Color::E color2);

    void Draw4SymbolsInRect(int x, int y, const Color::E color = Color::COUNT);

    void Draw4SymbolsInRect(int x, int y, TypeFont::E typeFont, const Color::E color = Color::COUNT);

    void Draw10SymbolsInRect(int x, int y);

    void Draw10SymbolsInRect(int x, int y, TypeFont::E typeFont);

private:
    char symbol;
};

