// 2022/03/24 17:44:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstdlib>
#include <cstring>


// ���� ������, �������������� �� ����, � ������� ����� ���������� ������������ ���������� ����.
// �� ����������/������������� ����� �� ��������, ������� �������, ����������� � ������������
class MemoryBlock
{
public:
    MemoryBlock(int size_cell) : size_block(size_cell), capacity(0), buffer(nullptr), num_elems(0) { }

    uint8 *Begin() { return buffer; }
    const uint8 *BeginConst() const { return buffer; }
    uint8 *End()  { return buffer + Size(); }

    int Size() const { return num_elems; }

    // ���������� ������ ����� ������ size. ���� capacity ������������, ���� ��������������. ���� capacity ����������,
    // ������ ��������������� ����� num_elems
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

    // ���������� ������ �� data. "����� �������". ��� ������� ��������
    void CopyData(const void *data, int size)
    {
        std::memcpy(buffer, data, (uint)size);
    }

private:
    const int size_block;   // ��������� ��������� ������
    int       capacity;     // ������� ���� ����� ���� �������� � �����
    uint8    *buffer;       // ��������� �� ������ ����������� ����� ������
    int       num_elems;    // ������� ���� � �����

    // ���������� ���������� ������, ����������� ��� ���������� need ����. � ������ ������������� size_block
    int CalculateNeedSize(int need)
    {
        if ((need % size_block) == 0)
        {
            return need;
        }

        return need + (size_block - (need % size_block));
    }
};
