#include "defines.h"
#include "Hardware/InterCom.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/LAN/LAN.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>


void InterCom::Send(const void* pointer, int size)
{
    VCP::Buffer::Send(pointer, size);
    LAN::SendBuffer(pointer, size);
}


void InterCom::SendFormat0D(pchar format, ...)
{
    char buffer[128];
    std::va_list args;
    va_start(args, format);
    std::vsprintf(buffer, format, args);
    va_end(args);
    std::strcat(buffer, "\n");

    VCP::Buffer::Send(buffer, (int)std::strlen(buffer));
    LAN::SendBuffer(buffer, (int)std::strlen(buffer));
}


void InterCom::Flush()
{
    VCP::Buffer::Flush();
}
