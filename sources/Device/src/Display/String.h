// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"


struct Color;


class String
{
public:
    explicit String();
    String(const String &);
    explicit String(char t);
    explicit String(pchar, ...);
    virtual ~String();

    void SetFormat(pchar format, ...);
    void SetString(const String &);
    void SetString(pchar);

    char *c_str() const { return buffer; }

    static pchar const _ERROR;

    void Free();

    void Append(pchar str);
    void Append(pchar str, int numSymbols);
    void Append(char);
    void Append(const String &);

    int Size() const;

    bool ToInt(int *out);

    char &operator[](int i) const;

    String &operator=(const String &);

    int Draw(int x, int y, Color::E = Color::Count);
    int DrawInCenterRect(int x, int y, int width, int height, Color::E = Color::Count);
    void DrawInRect(int x, int y, int width, int height, int dy = 0);
    int DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight);
    int DrawInRectWithTransfers(int x, int y, int width, int height, Color::E = Color::Count);
    // Выводит текст на прямоугольнике цвета colorBackgound
    int DrawOnBackground(int x, int y, Color::E colorBackground);

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
