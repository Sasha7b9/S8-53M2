// 2022/03/24 17:44:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Блок памяти, распределяемой из кучи, в который можно записывать произвольное количество байт.
// Он выделяется/увеличивается всега на величину, кратную размеру, задаваемому в конструкторе
class MemoryBlock
{
public:
    MemoryBlock(int size_cell) : size_block(size_cell), capacity(0), buffer(nullptr), num_elems(0) { }

    T *Buffer() { return buffer; }

    int Size() const { return num_elems; }

    void ReallocFromBuffer(const T *buffer, int size);

private:
    const int size_block;       // Кратность выделения памяти
    int capacity;               // Столько байт может быть записано в блоке
    T *buffer;                  // Указатель на начало выделенного блока памяти
    int num_elems;              // Столько байт в блоке
};