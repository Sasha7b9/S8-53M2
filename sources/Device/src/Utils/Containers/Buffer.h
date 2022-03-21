// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstring>


/*
    Класс единственно предназначен для автоматического выделения/освобождения памяти из кучи
*/

template<class T>
class Buffer
{
public:

    Buffer(int size = 0U);

    Buffer(const Buffer<T> &);

    ~Buffer();

    void Realloc(int size);
    void ReallocAndFill(int size, T value);
    // Перевыделить память и заполнить её из buffer
    void ReallocFromBuffer(const T *buffer, int size);

    void Free();

    void Fill(T value);
    // Записать в буфер size байт из buffer. Если памяти выделено меньше, заполнить только выделенную память
    void FillFromBuffer(const T *buffer, int size);

    inline int Size() const { return size; }

    inline char *DataChar() { return (char *)data; }

    inline uint8 *DataU8() { return (uint8 *)data; }

    inline T *Data() { return data; }

    inline T *Last() { return data + Size(); }

    inline T *Pointer(int index) { return data + index; }

    static float Sum(T *data, uint number);

    Buffer<T> &operator=(const Buffer<T> &);

    T &operator[](uint i);
    T &operator[](int i);

    void Log() const;

protected:

    T *data;

private:

    int size;

    void Malloc(int size);
};
