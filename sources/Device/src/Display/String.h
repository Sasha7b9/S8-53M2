// 2022/03/30 08:25:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include <cstring>


template<int capacity = 32>
class String
{
public:

    String() { buffer[0] = '\0'; }

    explicit String(pchar, ...);

    void SetFormat(pchar format, ...);

    char *c_str() const { return (char *)&buffer[0]; }

    int Size() const { return (int)std::strlen(buffer); }

    void Append(pchar);

    void Append(char);

    void Append(const String<capacity> &rhs) { Append(rhs.c_str()); }

    bool Filled() const { return (Size() == capacity - 1); }

    int Draw(int x, int y, Color::E = Color::Count);

    void DrawInRect(int x, int y, int width, int height, int dy = 0);

    int DrawInCenterRect(int x, int y, int width, int height, Color::E = Color::Count);

    int DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight);

    void DrawRelativelyRight(int xRight, int y, Color::E = Color::Count);

    // Выводит текст на прямоугольнике цвета colorBackgound
    int DrawOnBackground(int x, int y, Color::E colorBackground);

    int DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, Color::E colorBackground, Color::E colorFill);

    // Возвращает нижнюю координату прямоугольника.
    int DrawInBoundedRectWithTransfers(int x, int y, int width, Color::E colorBackground, Color::E colorFill);

    int DrawInRectWithTransfers(int x, int y, int width, int height, Color::E = Color::Count);

private:

    char buffer[capacity];

    int GetLenghtSubString(char *text);

    int DrawSubString(int x, int y, char *text);

    int DrawSpaces(int x, int y, char *text, int *numSymbols);
};
