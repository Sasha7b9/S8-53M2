#pragma once


template<class T, int size_buffer>
class Buffer
{
public:

    Buffer() : size(size_buffer), MAX_SIZE(size_buffer) {};

    void Fill(T value);

    T *Data() { return buffer; }

    const T *DataConst() const { return buffer; }

    void Realloc(int size);

    void ReallocAndFill(int size, T value);

    // Перевыделить память и заполнить её из buffer
    void ReallocFromBuffer(const T *buffer, int size);

    T &operator[](uint i);
    T &operator[](int i);

protected:

    int size;

    const int MAX_SIZE;

    T buffer[size_buffer];
};



template<class T>
class Buffer2048 : public Buffer<T, 2048>
{
public:
    Buffer2048();
    Buffer2048(int size, T value);
private:
};
