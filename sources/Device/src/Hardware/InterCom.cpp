// 2022/2/11 22:43:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/InterCom.h"


CommandBuffer::CommandBuffer(int size, uint8 type)
{
    data = new uint8[(uint)size];
    PushByte(type);
    pointer = 0;
}


CommandBuffer::~CommandBuffer()
{
    if (data)
    {
        delete []data;
        size = 0;
    }
}


void CommandBuffer::PushHalfWord(uint16 half_word)
{
    PushByte((uint8)half_word);
    PushByte((uint8)(half_word >> 8));
}


void CommandBuffer::PushWord(int word)
{
    PushHalfWord((uint16)word);
    PushHalfWord((uint16)(word >> 16));
}
