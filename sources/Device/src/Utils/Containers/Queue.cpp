// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Containers/Queue.h"
#include "common/Panel/Controls.h"


template Queue<uint16>::Queue();
template Queue<uint8>::Queue();
template Queue<float>::Queue();
template Queue<KeyboardEvent>::Queue();
template Queue<uint16>::~Queue();
template Queue<uint8>::~Queue();
template Queue<float>::~Queue();
template Queue<KeyboardEvent>::~Queue();
template uint16 Queue<uint16>::Front();
template uint8 Queue<uint8>::Front();
template KeyboardEvent Queue<KeyboardEvent>::Front();
template bool Queue<uint16>::IsEmpty() const;
template bool Queue<KeyboardEvent>::IsEmpty() const;
template void Queue<uint16>::Clear();
template float &Queue<float>::operator[](int);
template void Queue<KeyboardEvent>::Clear();


template<typename T>
Queue<T>::Queue() : pointer(nullptr), iFront(0), iBack(0)
{

}


template<typename T>
Queue<T>::~Queue()
{
    Destroy();
}


template<typename T>
void Queue<T>::Destroy()
{
    if (pointer != nullptr)
    {
        delete[] pointer;
        pointer = nullptr;
        iFront = 0;
        iBack = 0;
    }
}


template<typename T>
T Queue<T>::Front()
{
    if (pointer != nullptr)
    {
        T result = pointer[iFront];
        iFront++;
        if (iFront == iBack)
        {
            Destroy();
        }

        return result;
    }

    return T(0);
}


template<typename T>
T Queue<T>::Back()
{
    T result(0);

    if (pointer != nullptr)
    {
        result = pointer[iBack - 1];
        iBack--;
        if (iFront == iBack)
        {
            Destroy();
        }
    }

    return result;
}


template<typename T>
bool Queue<T>::IsEmpty() const
{
    return (Size() == 0);
}


template<typename T>
T &Queue<T>::operator[](int n)
{
    if (pointer != nullptr)
    {
        int index = iFront + n;
        if (index >= iFront && index < iBack)
        {
            return pointer[index];
        }
    }

	static T result(0);

    return result;
}


template<typename T>
T *Queue<T>::Data()
{
    return pointer;
}


template<typename T>
void Queue<T>::Clear()
{
    while (!IsEmpty())
    {
        Front();
    }
}
