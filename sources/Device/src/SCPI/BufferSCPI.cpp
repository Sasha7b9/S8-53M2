// 2022/01/21 16:39:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "SCPI/BufferSCPI.h"
#include <cstring>
#include <cstdlib>


BufferSCPI::BufferSCPI() : buffer(nullptr), size_buffer(0), size_data(0)
{
}


void BufferSCPI::Append(const uint8 *data, int length)
{
    if (size_data + length > size_buffer)
    {
        uint8 *new_buffer = (uint8 *)std::malloc((uint)(size_data + length));

        if (new_buffer == nullptr)
        {
            Free();

            TLOG_WRITE("Нет памяти");
        }
        else
        {
            if (size_data)
            {
                std::memcpy(new_buffer, buffer, (uint)size_data);
            }

            std::free(buffer);

            std::memcpy(new_buffer + size_data, data, (uint)length);

            size_data = size_data + length;

            size_buffer = size_data;

            buffer = new_buffer;
        }
    }
}


bool BufferSCPI::ExistDivider() const
{
    for (int i = 0; i < Size(); i++)
    {
        if (buffer[i] == 0x0d || buffer[i] == 0x0a)
        {
            return true;
        }
    }

    return false;
}


void BufferSCPI::RemoveFirstDividers()
{
    while (Size() && (buffer[0] == 0x0d || buffer[0] == 0x0a))
    {
        RemoveFirstBytes(1);
    }
}


String BufferSCPI::ToString()
{
    String result;

    result.Append((char *)buffer, Size());

    return result;
}


String BufferSCPI::ExtractCommand()
{
    int pos_start = 0;

    for (int i = 0; i < Size(); i++)
    {
        if (buffer[i] == 0x0d || buffer[i] == 0x0a)
        {
            pos_start++;
        }
        else
        {
            break;
        }
    }

    int pos_end = -1;

    for (int i = pos_start; i < Size(); i++)
    {
        if (buffer[i] == 0x0d || buffer[i] == 0x0a)
        {
            pos_end = i;
            break;
        }
    }

    if (pos_end == -1)
    {
        return String("");
    }

    String result;

    result.Append((char *)&buffer[pos_start], pos_end - pos_start);

    RemoveFirstBytes(pos_end);

    RemoveFirstDividers();

    return result;
}


void BufferSCPI::RemoveFirstBytes(int num_bytes)
{
    if (num_bytes > Size())
    {
        num_bytes = Size();
    }

    if (num_bytes == Size())
    {
        Free();
    }
    else
    {
        ShiftToLeft(buffer + num_bytes, size_data - num_bytes, num_bytes);

        size_data -= num_bytes;
    }
}


void BufferSCPI::ShiftToLeft(uint8 *data, int num_bytes, int shift)
{
    uint8 *to = data - shift;

    while (num_bytes--)
    {
        *to++ = *data++;
    }
}


void BufferSCPI::Free()
{
    std::free(buffer);

    buffer = nullptr;

    size_buffer = 0;
    size_data = 0;
}
