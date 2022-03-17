#pragma once


namespace VCP
{
    void Init();

    void SendSynch(const uint8 *data, int size);
    void SendStringSynch(char *data);                   // Передаётся строка без завершающего нуля.
    void SendStringSynch(char *format, ...);      // Эта строка передаётся с завершающими символами \r\n.

    void SendStringAsynch(char *format, ...);     // Эта строка передаётся с завершающими символами \r\n.

    void Flush();

    extern void *handlePCD;
    extern void *handleUSBD;

    extern bool cableIsConnected;
    extern bool clientIsConnected;
};
