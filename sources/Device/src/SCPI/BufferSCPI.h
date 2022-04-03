// 2022/01/21 16:38:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "common/Utils/Containers/String_.h"


class BufferSCPI
{
public:

    BufferSCPI();

    void Append(const uint8 *data, int length);

    // true, если присутствует разделитель - должна быть команда, которую можно обработать
    bool ExistDivider() const;

    String ExtractCommand();

    int Size() const { return size_data; }

    // Удалить разделители из начала буфера
    void RemoveFirstDividers();

    void Free();

    String ToString();

private:

    uint8 *buffer;

    int size_buffer;    // Размер выделенной памяти
    int size_data;      // Размер буфера, занятый данными

    // Удлаить первые num_bytes из начала буфера
    void RemoveFirstBytes(int num_bytes);

    // Сместить num_bytes байт на shfit байт влево
    void ShiftToLeft(uint8 *data, int num_bytes, int shift);
};

