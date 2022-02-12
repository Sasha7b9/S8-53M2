// 2022/2/11 22:43:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/InterCom.h"


CommandBuffer::CommandBuffer(int _size, uint8 type) : size(_size), pointer(0)
{
    data = new uint8[(uint)size];
    PushByte(type);
}


CommandBuffer::~CommandBuffer()
{
    delete []data;
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
