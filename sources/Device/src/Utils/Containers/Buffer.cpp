// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Utils/Containers/Buffer.h"
#include <cstdlib>
#include <cstring>


template<class T>
Buffer<T>::Buffer(int s) : data(nullptr)
{
    Malloc(s);
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
            std::memset(data, value, (uint)size);
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
            LOG_WRITE("��� ������");
        }
    }
    else
    {
        data = nullptr;
        size = 0U;
    }
}


template<class T>
T &Buffer<T>::operator[](uint i) const
{
    if ((int)i < Size())
    {
        return data[i];
    }

    static T empty(0);

    return empty;
}


template<class T>
T &Buffer<T>::operator[](int i) const
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
Buffer<T> &Buffer<T>::operator=(const Buffer<T> &rhs)
{
    Realloc(rhs.Size());

    std::memcpy(data, rhs.data, (uint)Size());

    return *this;
}