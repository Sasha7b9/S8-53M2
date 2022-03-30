// 2022/03/30 08:25:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


template<int size_buffer = 32>
class String
{
public:

    String() : capacity(size_buffer) { }

private:

    const int capacity;

    char buffer[size_buffer];
};
