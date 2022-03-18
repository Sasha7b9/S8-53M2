// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/VCP/USBD.h"
#include "Utils/Math.h"
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
}


void VCP::Init()
{
    USBD::Init();
}


void VCP::Send(const uint8 *buffer, int size)
{
#ifdef GUI

    (void)size;

    ConsoleSCPI::Self()->AddText("<<< ");
    ConsoleSCPI::Self()->AddText((char *)buffer);

#else

    if (!VCP::connectToHost)
    {
        return;
    }

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

        USBD::Transmit(tr_buf, size);

        buffer += portion;
        size -= portion;
    }

#endif
}


void VCP::SendFormat(char *format, ...)
{
    static const int SIZE_BUFFER = 200;
    static char buffer[SIZE_BUFFER];

    std::va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    std::strcat(buffer, "\n");

    Send((uint8 *)buffer, (int)std::strlen(buffer));
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

    std::sprintf(message, "%s   %s:%s:%d", buffer, module, func, line);

    Send((uint8 *)message, (int)std::strlen(message) + 1);
}


void VCP::DebugPoint(pchar module, pchar function, int line)
{
    static const int SIZE_BUFFER = 256;
    char message[SIZE_BUFFER];

    std::sprintf(message, "%s:%s:%d", module, function, line);

    Send((const uint8 *)message, (int)std::strlen(message) + 1);
}
