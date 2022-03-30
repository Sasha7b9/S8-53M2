// 2022/02/15 09:14:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"


class DString
{
public:
    explicit DString();
    DString(const DString &);
    explicit DString(char t);
    explicit DString(pchar, ...);
    virtual ~DString();

    void SetFormat(pchar format, ...);
    void SetString(const DString &);
    void SetString(pchar);

    char *c_str() const { return buffer; }

    static pchar const _ERROR;

    void Free();

    void Append(pchar str);
    void Append(pchar str, int numSymbols);
    void Append(char);
    void Append(const DString &);

    int Size() const;

    bool ToInt(int *out);

    char &operator[](int i) const;

    DString &operator=(const DString &);

    int Draw(int x, int y, Color::E = Color::Count);
    int DrawInCenterRect(int x, int y, int width, int height, Color::E = Color::Count);
    void DrawInRect(int x, int y, int width, int height, int dy = 0);
    int DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight);
    int DrawInRectWithTransfers(int x, int y, int width, int height, Color::E = Color::Count);
    int DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, Color::E colorBackground, Color::E colorFill);
    void DrawRelativelyRight(int xRight, int y, Color::E = Color::Count);
    // Возвращает нижнюю координату прямоугольника.
    int DrawInBoundedRectWithTransfers(int x, int y, int width, Color::E colorBackground, Color::E colorFill);
    // Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра
    // widthBorder цвета colorBackground.
    void DrawInCenterRectOnBackground(int x, int y, int width, int height, Color::E colorText, int widthBorder, Color::E colorBackground);

private:

    bool Allocate(int size);

    int NeedMemory(int size);

    char *buffer;

    int capacity;

    static const int SIZE_SEGMENT = 32;

    int GetLenghtSubString(char *text);

    int DrawSubString(int x, int y, char *text);

    int DrawSpaces(int x, int y, char *text, int *numSymbols);
};


class Char
{
public:
    Char(char s) : symbol(s) {}
    int Draw(int x, int y, Color::E = Color::Count);
private:
    char symbol;
};


namespace PText
{
    // Возвращает высоту экрана, которую займёт текст text, при выводе от left до right в переменной height. Если
    // bool == false, то текст не влезет на экран 
    bool GetHeightTextWithTransfers(int left, int top, int right, pchar text, int *height);

    int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight);

    void Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

    void Draw4SymbolsInRect(int x, int y, char eChar, Color::E = Color::Count);

    void Draw10SymbolsInRect(int x, int y, char eChar);

    void DrawBig(int x, int y, int size, pchar text);

    DString GetWord(pchar firstSymbol, int *length);

    // Если draw == false, то рисовать символ не надо, фунция используется только для вычислений
    int DrawPartWord(char *word, int x, int y, int xRight, bool draw);
}
