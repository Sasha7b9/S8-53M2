// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/VCP/USBD.h"
#include "Utils/Math.h"
#include "Utils/Containers/Buffer.h"
#ifdef GUI
    #include "GUI/ConsoleSCPI.h"
#endif
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <usbd_cdc.h>


namespace VCP
{
    bool cableIsConnected = false;
    bool connectToHost = false;

    TimeMeterMS _meter;
    uint _sended_bytes = 0;

    namespace Buffer
    {
        ::Buffer<uint8, 64> buffer;
    }
}


void VCP::Init()
{
    USBD::Init();
    Buffer::buffer.Realloc(0);
}


void VCP::SendString(pchar string)
{
    SendBuffer(string, (int)std::strlen(string) + 1);
}


void VCP::SendBuffer(const void *_buffer, int size)
{
    _sended_bytes += size;

    const uint8 *buffer = (const uint8 *)_buffer;

#ifdef GUI

    (void)size;

    ConsoleSCPI::Self()->AddText("<<< ");
    ConsoleSCPI::Self()->AddText((char *)buffer);

#else

    if (!VCP::connectToHost)
    {
        return;
    }

    _meter.Continue();

    const int SIZE_BUFFER = 64;
    static uint8 tr_buf[SIZE_BUFFER];

    while (size)
    {
        int portion = SIZE_BUFFER;

        if (portion > size)
        {
            portion = size;
        }

        while (!USBD::PrevSendingComplete())
        {
            if (!VCP::connectToHost)
            {
                return;
            }
        }

        std::memcpy(tr_buf, buffer, (uint)portion);

        USBD::Transmit(tr_buf, portion);

        buffer += portion;
        size -= portion;
    }

    _meter.Pause();

#endif
}


void VCP::Buffer::Send(const void *buf, int size)
{
    uint8 *pointer = (uint8 *)buf;

    while (size > 0)
    {
        if (buffer.Size() == buffer.Capacity())
        {
            Flush();
        }

        int portion = size;

        int rest_buffer = buffer.Capacity() - buffer.Size();

        if (portion > rest_buffer)
        {
            portion = rest_buffer;
        }

        buffer.Append(pointer, portion);

        pointer += portion;
        size -= portion;
    }
}


void VCP::Buffer::Flush()
{
    if (buffer.Size())
    {
        SendBuffer(buffer.Data(), buffer.Size());
    }

    buffer.Realloc(0);
}


void VCP::SendFormat(pchar format, ...)
{
    static const int SIZE_BUFFER = 1024;
    static char buffer[SIZE_BUFFER];

    std::va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    std::strcat(buffer, "\n");

    SendBuffer(buffer, (int)std::strlen(buffer) + 1);
}


void VCP::SendFormatTrace(pchar module, pchar func, int line, char *format, ...)
{
    const int SIZE_BUFFER = 1024;

    char buffer[SIZE_BUFFER];

    std::va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    char message[SIZE_BUFFER];

    std::snprintf(message, SIZE_BUFFER, "%s                             %s:%s:%d", buffer, module, func, line);

    SendBuffer(message, (int)std::strlen(message) + 1);
}


void VCP::DebugPoint(pchar module, pchar function, int line)
{
    static const int SIZE_BUFFER = 256;
    char message[SIZE_BUFFER];

    std::sprintf(message, "%s:%s:%d", module, function, line);

    SendBuffer(message, (int)std::strlen(message) + 1);
}
