// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"
#include <cstdlib>
#include <cstring>


// Очередь с фиксированым размером, который задаётся при создании объекта
template<typename T, int size>
struct Queue
{
    Queue() : pointer(0) { }

    void Clear() { pointer = 0; }

    void Push(T elem)
    {
        if (pointer < size)
        {
            buffer[pointer++] = elem;
        }
        else
        {
            LOG_ERROR_TRACE("буфер слишком мал");
        }
    }

    int Size() const { return pointer; }

    bool Empty() const { return (pointer == 0); }

    T Back()
    {
        if (pointer == 0)
        {
            return T(0);
        }

        T result = buffer[0];

        std::memmove(&buffer[0], &buffer[1], sizeof(T) * pointer);

        pointer--;

        return result;
    }

    T Front()
    {
        if (pointer == 0)
        {
            return T(0);
        }

        return buffer[--pointer];
    }

    Mutex mutex;

private:

    T buffer[size];
    int pointer;                        // Здесь позиция элемента, в который будет производиться сохранение
};
