// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Utils/Containers/Buffer.h"
#include "Utils/GlobalFunctions.h"
#include <cstdlib>
#include <cstring>


template                Buffer<uint8>::Buffer(int);
template                Buffer<char>::Buffer(int);
template                Buffer<float>::Buffer(int);
template                Buffer<uint>::Buffer(int);
template                Buffer<uint8>::~Buffer();
template                Buffer<char>::~Buffer();
template                Buffer<float>::~Buffer();
template                Buffer<uint>::~Buffer();
template void           Buffer<uint8>::Fill(uint8);
template void           Buffer<uint8>::ReallocFromBuffer(const uint8 *, int);
template void           Buffer<uint>::Fill(uint);
template void           Buffer<float>::Free();
template void           Buffer<uint8>::Realloc(int);
template void           Buffer<float>::Realloc(int);
template void           Buffer<float>::Realloc(int, float);
template void           Buffer<uint8>::Realloc(int, uint8);
template uint8         &Buffer<uint8>::operator[](int);
template uint8         &Buffer<uint8>::operator[](uint);
template float         &Buffer<float>::operator[](int);
template uint          &Buffer<uint>::operator[](int);
template Buffer<uint8> &Buffer<uint8>::operator=(const Buffer<uint8> &);
template void           Buffer<uint8>::Log() const;
template float          Buffer<uint8>::Sum(uint8 *, uint);


template<class T>
Buffer<T>::Buffer(int _size) : data(nullptr)
{
    Malloc(_size);
}


template<class T>
Buffer<T>::Buffer(const Buffer<T> &rhs) : data(nullptr)
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
    Free();
    Malloc(_size);
}


template<class T>
void Buffer<T>::Realloc(int _size, T value)
{
    Realloc(_size);
    Fill(value);
}


template<class T>
void Buffer<T>::Fill(T value)
{
    if (size)
    {
#ifdef WIN32
#pragma warning(push, 0)
#endif
        if (sizeof(data[0]) == 1)
#ifdef WIN32
#pragma warning(pop)
#endif
        {
            std::memset(data, (int)value, (uint)size);
        }
        else
        {
            for (int i = 0; i < size; i++)
            {
                data[i] = value;
            }
        }
    }
}


template<class T>
void Buffer<T>::ReallocFromBuffer(const T *buffer, int _size)
{
    Realloc(_size);

    std::memcpy(data, buffer, (uint)_size);
}


template<class T>
void Buffer<T>::Free()
{
    std::free(data);
    data = nullptr;
    size = 0U;
}


template<class T>
void Buffer<T>::Malloc(int s)
{
    if (s > 0)
    {
        data = (T *)(std::malloc((uint)(s) * sizeof(T)));
        size = (data) ? s : 0;

        if(!data)
        {
            LOG_WRITE("Нет памяти");
        }
    }
    else
    {
        data = nullptr;
        size = 0U;
    }
}


template<class T>
T &Buffer<T>::operator[](uint i)
{
    if ((int)i < Size())
    {
        return data[i];
    }

    static T empty(0);

    return empty;
}


template<class T>
T &Buffer<T>::operator[](int i)
{
    if (i >= 0 && i < (int)Size())
    {
        return data[i];
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
void Buffer<T>::Log() const
{
    char buffer[1024];
    char buffer_number[20];

    buffer[0] = '\0';

    for (int i = 0; i < Size(); i++)
    {
        std::strcat(buffer, Int2String(data[i], false, 1, buffer_number));
        std::strcat(buffer, " ");
    }

    LOG_WRITE(buffer);
}


template<class T>
Buffer<T> &Buffer<T>::operator=(const Buffer<T> &rhs)
{
    Realloc(rhs.Size());

    std::memcpy(data, rhs.data, (uint)Size());

    return *this;
}
