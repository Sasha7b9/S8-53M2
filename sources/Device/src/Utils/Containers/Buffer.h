// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Containers/MemoryBlock.h"
#include <cstring>


/*
    ����� ����������� ������������ ��� ��������������� ���������/������������ ������ �� ����
*/

template<class T>
class Buffer
{
public:

    Buffer(int size = 0U);

    Buffer(int size, T value);

    Buffer(const Buffer<T> &);

    ~Buffer();

    void Realloc(int size);
    void ReallocAndFill(int size, T value);
    // ������������ ������ � ��������� � �� buffer
    void ReallocFromBuffer(const T *buffer, int size);

    void Free();

    void Fill(T value);
    // �������� � ����� size ���� �� buffer. ���� ������ �������� ������, ��������� ������ ���������� ������
    void FillFromBuffer(const T *buffer, int size);

    // ���������� ���������� ��������� � ������
    inline int Size() const { return block.Size(); }

    inline char *DataChar() { return (char *)data; }

    inline uint8 *DataU8() { return (uint8 *)data; }

    inline T *Data() { return block.Buffer(); }

    inline T *Last() { return Data() + Size(); }

    inline T *Pointer(int index) { return Data() + index; }

    static float Sum(T *data, uint number);

    Buffer<T> &operator=(const Buffer<T> &);

    T &operator[](uint i);
    T &operator[](int i);

    void Log() const;

private:

    MemoryBlock<T> block;

    void Malloc(int size);
};
