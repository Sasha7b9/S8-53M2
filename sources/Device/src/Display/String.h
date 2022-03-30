// 2022/03/30 08:25:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"


template<int capa = 32>
class String
{
public:

    String() : capacity(capa) { buffer[0] = '\0'; }

    explicit String(pchar, ...);

    char *c_str() const;

    int Size() const;

    void Append(pchar str);

    void Append(const String<capa> &);

    int Draw(int x, int y, Color::E = Color::Count);

private:

    const int capacity;

    char buffer[capa];
};
