// 2022/01/21 16:38:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Text/String.h"
#include "Utils/Containers/Buffer.h"


class BufferSCPI : public Buffer<char, 1024>
{
public:

    BufferSCPI() : Buffer<char, 1024>() { Realloc(0); }

    void Append(pchar data, int length);

    // true, ���� ������������ ����������� - ������ ���� �������, ������� ����� ����������
    bool ExistDivider() const;

    String<> ExtractCommand();

    // ������� ����������� �� ������ ������
    void RemoveFirstDividers();

    String<> ToString();

private:

    // ������� ������ num_bytes �� ������ ������
    void RemoveFirstBytes(int num_bytes);
};

