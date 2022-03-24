// 2022/03/24 17:44:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstdlib>
#include <cstring>


// Блок памяти, распределяемой из кучи, в который можно записывать произвольное количество байт.
// Он выделяется/увеличивается всега на величину, кратную размеру, задаваемому в конструкторе
class MemoryBlock
{
public:
    MemoryBlock(int size_cell) : size_block(size_cell), capacity(0), buffer(nullptr), num_elems(0) { }

    uint8 *Begin() { return buffer; }
    const uint8 *BeginConst() const { return buffer; }
    uint8 *End()  { return buffer + Size(); }

    int Size() const { return num_elems; }

    // Установить размер блока равным size. Если capacity недостаточно, блок перевыделяется. Если capacity достаточно,
    // Просто устанавливается новый num_elems
    void SetSize(int size)
    {
        if (Size())
        {
            Free();
        }

        capacity = CalculateNeedSize(size);

        buffer = (uint8 *)std::malloc((uint)capacity);

        num_elems = size;
    }

    void Free()
    {
        if (buffer)
        {
            std::free(buffer);
            buffer = nullptr;
            capacity = 0;
            num_elems = 0;
        }
    }

    // Копировать данные из data. "Голая функций". Нет никаких проверок
    void CopyData(const void *data, int size)
    {
        std::memcpy(buffer, data, (uint)size);
    }

private:
    const int size_block;   // Кратность выделения памяти
    int       capacity;     // Столько байт может быть записано в блоке
    uint8    *buffer;       // Указатель на начало выделенного блока памяти
    int       num_elems;    // Столько байт в блоке

    // Рассчитать количество памяти, необходимой для размещения need байт. С учётом гранулярности size_block
    int CalculateNeedSize(int need)
    {
        if ((need % size_block) == 0)
        {
            return need;
        }

        return need + (size_block - (need % size_block));
    }
};
