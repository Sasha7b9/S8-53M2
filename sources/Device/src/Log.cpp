// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Log.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "VCP/VCP.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


namespace Log
{
    bool loggerUSB = false;

    const int SIZE_BUFFER_LOG = 200;
}


void Log::Write(char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    Display::AddStringToIndicating(buffer);
    if(loggerUSB)
    {
        //VCP::SendFormatStringAsynch(buffer);
    }
}


void Log::ErrorTrace(pchar module, pchar func, int numLine, char *format, ...)
{
    char buffer[SIZE_BUFFER_LOG];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    char numBuffer[20];
    sprintf(numBuffer, ":%d", numLine);
    char message[SIZE_BUFFER_LOG];
    message[0] = 0;
    strcat(message, "!!!ERROR!!! ");
    strcat(message, module);
    strcat(message, " ");
    strcat(message, func);
    strcat(message, numBuffer);
    Display::AddStringToIndicating(message);
    Display::AddStringToIndicating(buffer);
    if(loggerUSB)
    {
        //VCP::SendFormatStringAsynch(message);
        //VCP::SendFormatStringAsynch(buffer);
    }
}


void Log::DisconnectLoggerUSB()
{
    //static uint8 data = 20;
    //Log_Write("посылаю %d", data);
    //VCP_SendData(&data, 1);
}


void Log::EnableLoggerUSB(bool enable)
{
    loggerUSB = enable;
}
