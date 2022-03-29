// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"
#include <cstdlib>
#include <cstring>


template<typename T>
class Queue
{
public:
    Queue();

    ~Queue();

    void Push(T elem)
    {
        if (pointer == nullptr)
        {
            pointer = new T[1];
            *pointer = elem;
            iFront = 0;
            iBack = 1;
        }
        else
        {
            T *temp = pointer;
            int num = iBack - iFront + 1;
            pointer = new T[(uint)(num)];
            for (int i = 0; i < num - 1; i++)
            {
                pointer[i] = temp[i + iFront];
            }
            pointer[num - 1] = elem;
            iFront = 0;
            iBack = num;
            delete[] temp;
        }
    }

    T Front();

    T Back();

    void Clear();

    int Size() const
    {
        return (iBack - iFront);
    }

    bool Empty() const;

    T &operator[](int n);

    T *Data();

private:

    void Destroy();
    
    T *pointer;         // Указатель на массив элементов
    int iFront;         // Индекс первого элемента
    int iBack;          // Индекс элемента за последним
};


// Очередь с фиксированым размером, который задаётся при создании объекта
template<typename T, int size>
struct StaticQueue
{
    StaticQueue() : SIZE(size), pointer(0) { }

    void Clear() { pointer = 0; }

    void Push(T elem)
    {
        if (pointer < size)
        {
            buffer[pointer++] = elem;
        }
    }

    bool Empty() const { return (pointer == 0); }

    T Back()
    {
        if (pointer == 0)
        {
            return T();
        }

        T result = buffer[0];

        std::memmove(&buffer[0], &buffer[1], sizeof(T) * pointer);

        pointer--;

        return result;
    }

    Mutex mutex;

private:

    const int SIZE;
    T buffer[size];
    int pointer;                        // Здесь позиция элемента, в который будет производиться сохранение
};
