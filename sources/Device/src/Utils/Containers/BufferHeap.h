// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Containers/MemoryBlock.h"
#include <cstring>


/*
    Класс единственно предназначен для автоматического выделения/освобождения памяти из кучи
*/

template<class T>
class BufferHeap
{
public:

    BufferHeap(int size = 0U) : block(32)
    {
        Malloc(size);
    }

    BufferHeap(int size, T value);

    BufferHeap(const BufferHeap<T> &);

    ~BufferHeap();

    void Realloc(int size);
    void ReallocAndFill(int size, T value);
    // Перевыделить память и заполнить её из buffer
    void ReallocFromBuffer(const T *buffer, int size);

    void Free();

    void Fill(T value);
    // Записать в буфер size байт из buffer. Если памяти выделено меньше, заполнить только выделенную память
    void FillFromBuffer(const T *buffer, int size);

    // Возвращает количество элементов в буфере
    inline int Size() const { return block.Size() / (int)sizeof(T); }

    inline char *DataChar() { return (char *)Data(); }

    inline uint8 *DataU8() { return (uint8 *)Data(); }

    inline T *Data() { return (T *)block.Begin(); }

    inline const T *DataConst() const { return (const T *)(block.BeginConst()); }

    inline T *Last() { return Data() + Size(); }

    inline T *Pointer(int index) { return Data() + index; }

    static float Sum(T *data, uint number);

    BufferHeap<T> &operator=(const BufferHeap<T> &);

    T &operator[](uint i);
    T &operator[](int i);

private:

    MemoryBlock block;

    void Malloc(int size)
    {
        block.SetSize(size * (int)sizeof(T));
    }
};
