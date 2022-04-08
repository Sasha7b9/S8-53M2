#pragma once
#include "defines.h"


namespace TCP
{
    void Init(void (*funcReciever)(const void *buffer, int length));
    
    void SendBuffer(const void *buffer, int length);
    
    void SendString(char *format, ...);

    bool IsConnected();
}
