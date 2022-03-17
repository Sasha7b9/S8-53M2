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


namespace VCP
{
    bool cableIsConnected = false;
    bool connectToHost = false;
}


void VCP::Init()
{
    USBD::Init();
}


void VCP::SendBuffer(const uint8 *buffer, int size)
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
    static uint8 trBuf[SIZE_BUFFER];

    size = Math::MinFrom2(size, SIZE_BUFFER);
    while (!USBD::PrevSendingComplete())
    {
        if(!VCP::connectToHost)
        {
            return;
        }
    };
    std::memcpy(trBuf, buffer, (uint)(size));

    USBD::Transmit(trBuf, size);

#endif
}


void VCP::SendMessage(pchar message)
{
    SendBuffer((const uint8 *)message, (int)std::strlen(message));
}


void VCP::SendSynch(const uint8 *buffer, int size)
{

}


void VCP::SendAsynch(char *format, ...)
{

}


void VCP::Flush()
{

}
