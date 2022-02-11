// 2022/2/11 22:43:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/InterCom.h"


void CommandBuffer::Transmit(int num_bytes)
{
    InterCom::Send(Data(), num_bytes);
}