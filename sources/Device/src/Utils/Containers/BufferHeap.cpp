// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Utils/Containers/BufferHeap.h"
#include "Utils/Strings.h"
#include <cstdlib>
#include <cstring>


template                BufferHeap<char>::BufferHeap(int);
template                BufferHeap<float>::BufferHeap(int);
template                BufferHeap<uint>::BufferHeap(int);
template                BufferHeap<uint8>::BufferHeap(int, uint8);
template                BufferHeap<uint>::BufferHeap(int, uint);
template                BufferHeap<uint8>::~BufferHeap();
template                BufferHeap<char>::~BufferHeap();
template                BufferHeap<float>::~BufferHeap();
template                BufferHeap<uint>::~BufferHeap();
template void           BufferHeap<uint8>::Fill(uint8);
template void           BufferHeap<uint8>::ReallocFromBuffer(const uint8 *, int);
template void           BufferHeap<char>::ReallocFromBuffer(const char *, int);
template void           BufferHeap<uint>::Fill(uint);
template void           BufferHeap<uint8>::FillFromBuffer(const uint8 *, int);
template void           BufferHeap<float>::Free();
template void           BufferHeap<uint8>::Realloc(int);
template void           BufferHeap<float>::Realloc(int);
template void           BufferHeap<float>::ReallocAndFill(int, float);
template void           BufferHeap<uint8>::ReallocAndFill(int, uint8);
template uint8         &BufferHeap<uint8>::operator[](int);
template uint8         &BufferHeap<uint8>::operator[](uint);
template float         &BufferHeap<float>::operator[](int);
template uint          &BufferHeap<uint>::operator[](int);
template BufferHeap<uint8> &BufferHeap<uint8>::operator=(const BufferHeap<uint8> &);
template float          BufferHeap<uint8>::Sum(uint8 *, uint);


template<class T>
BufferHeap<T>::BufferHeap(int _size, T value) : block(32)
{
    Malloc(_size);

    Fill(value);
}


template<class T>
BufferHeap<T>::BufferHeap(const BufferHeap<T> &rhs) : block(32)
{
    *this = rhs;
}


template<class T>
BufferHeap<T>::~BufferHeap()
{
    Free();
}


template<class T>
void BufferHeap<T>::Realloc(int _size)
{
    if (_size != Size())
    {
        Free();
        Malloc(_size);
    }
}


template<class T>
void BufferHeap<T>::ReallocAndFill(int _size, T value)
{
    if (_size != Size())
    {
        Realloc(_size);
    }

    Fill(value);
}


template<class T>
void BufferHeap<T>::Fill(T value)
{
    for (int i = 0; i < Size(); i++)
    {
        (*this)[i] = value;
    }
}


template<class T>
void BufferHeap<T>::ReallocFromBuffer(const T *buffer, int size)
{
    if (size != Size())
    {
        block.SetSize(size * (int)sizeof(T));
    }

    block.CopyData(buffer, (int)sizeof(T) * size);
}


template<class T>
void BufferHeap<T>::FillFromBuffer(const T *buffer, int size)
{
    if (Size() < size)
    {
        size = Size();
    }

    block.CopyData(buffer, (int)sizeof(T) * size);
}


template<class T>
void BufferHeap<T>::Free()
{
    block.Free();
}


template<class T>
T &BufferHeap<T>::operator[](uint i)
{
    if ((int)i < Size())
    {
        return Data()[i];
    }

    static T empty(0);

    return empty;
}


template<class T>
T &BufferHeap<T>::operator[](int i)
{
    if (i >= 0 && i < (int)Size())
    {
        return Data()[i];
    }

    static T empty(0);

    return empty;
}


template<class T>
float BufferHeap<T>::Sum(T *data, uint number)
{
    float result = 0.0F;

    for (uint i = 0; i < number; i++)
    {
        result += *data;
        data++;
    }

    return result;
}


template<class T>
BufferHeap<T> &BufferHeap<T>::operator=(const BufferHeap<T> &rhs)
{
    Realloc(rhs.Size());

    block.CopyData(rhs.DataConst(), Size() * (int)sizeof(T));

    return *this;
}
