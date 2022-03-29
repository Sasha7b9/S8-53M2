// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Utils/Containers/Buffer.h"
#include "Utils/Strings.h"
#include <cstdlib>
#include <cstring>


template                Buffer<char>::Buffer(int);
template                Buffer<float>::Buffer(int);
template                Buffer<uint>::Buffer(int);
template                Buffer<uint8>::Buffer(int, uint8);
template                Buffer<uint>::Buffer(int, uint);
template                Buffer<uint8>::~Buffer();
template                Buffer<char>::~Buffer();
template                Buffer<float>::~Buffer();
template                Buffer<uint>::~Buffer();
template void           Buffer<uint8>::Fill(uint8);
template void           Buffer<uint8>::ReallocFromBuffer(const uint8 *, int);
template void           Buffer<char>::ReallocFromBuffer(const char *, int);
template void           Buffer<uint>::Fill(uint);
template void           Buffer<uint8>::FillFromBuffer(const uint8 *, int);
template void           Buffer<float>::Free();
template void           Buffer<uint8>::Realloc(int);
template void           Buffer<float>::Realloc(int);
template void           Buffer<float>::ReallocAndFill(int, float);
template void           Buffer<uint8>::ReallocAndFill(int, uint8);
template uint8         &Buffer<uint8>::operator[](int);
template uint8         &Buffer<uint8>::operator[](uint);
template float         &Buffer<float>::operator[](int);
template uint          &Buffer<uint>::operator[](int);
template Buffer<uint8> &Buffer<uint8>::operator=(const Buffer<uint8> &);
template float          Buffer<uint8>::Sum(uint8 *, uint);


template<class T>
Buffer<T>::Buffer(int _size, T value) : block(32)
{
    Malloc(_size);

    Fill(value);
}


template<class T>
Buffer<T>::Buffer(const Buffer<T> &rhs) : block(32)
{
    *this = rhs;
}


template<class T>
Buffer<T>::~Buffer()
{
    Free();
}


template<class T>
void Buffer<T>::Realloc(int _size)
{
    if (_size != Size())
    {
        Free();
        Malloc(_size);
    }
}


template<class T>
void Buffer<T>::ReallocAndFill(int _size, T value)
{
    if (_size != Size())
    {
        Realloc(_size);
    }

    Fill(value);
}


template<class T>
void Buffer<T>::Fill(T value)
{
    for (int i = 0; i < Size(); i++)
    {
        (*this)[i] = value;
    }
}


template<class T>
void Buffer<T>::ReallocFromBuffer(const T *buffer, int size)
{
    if (size != Size())
    {
        block.SetSize(size * (int)sizeof(T));
    }

    block.CopyData(buffer, (int)sizeof(T) * size);
}


template<class T>
void Buffer<T>::FillFromBuffer(const T *buffer, int size)
{
    if (Size() < size)
    {
        size = Size();
    }

    block.CopyData(buffer, (int)sizeof(T) * size);
}


template<class T>
void Buffer<T>::Free()
{
    block.Free();
}


template<class T>
T &Buffer<T>::operator[](uint i)
{
    if ((int)i < Size())
    {
        return Data()[i];
    }

    static T empty(0);

    return empty;
}


template<class T>
T &Buffer<T>::operator[](int i)
{
    if (i >= 0 && i < (int)Size())
    {
        return Data()[i];
    }

    static T empty(0);

    return empty;
}


template<class T>
float Buffer<T>::Sum(T *data, uint number)
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
Buffer<T> &Buffer<T>::operator=(const Buffer<T> &rhs)
{
    Realloc(rhs.Size());

    block.CopyData(rhs.DataConst(), Size() * (int)sizeof(T));

    return *this;
}
