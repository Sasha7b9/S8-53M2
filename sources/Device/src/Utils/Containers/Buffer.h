// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
     ласс единственно предназначен дл€ автоматического выделени€/освобождени€ пам€ти из кучи
*/

template<class T>
class Buffer
{
public:

    Buffer(int size = 0U);

    ~Buffer();

    void Realloc(int size);

    void Free();

    void Fill(T value);

    inline int Size() const { return size; }

    inline char *DataChar() { return (char *)data; }

    inline uint8 *DataU8() { return (uint8 *)data; }

    inline T *Data() { return data; }

    inline T *Last() { return data + Size(); }

    inline T *Pointer(int index) { return data + index; }

    static float Sum(T *data, uint number);

    Buffer<T> &operator=(const Buffer<T> &);

    T &operator[](uint i) const;
    T &operator[](int i) const;

private:

    int size;

    T *data;

    void Malloc(int size);
};
