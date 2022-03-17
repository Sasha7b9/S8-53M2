#pragma once


namespace VCP
{
    void Init();

    void SendSynch(const uint8 *data, int size);
    void SendSynch(char *data);                   // Передаётся строка без завершающего нуля.
    void SendSynch(char *format, ...);      // Эта строка передаётся с завершающими символами \r\n.

    void SendAsynch(char *format, ...);     // Эта строка передаётся с завершающими символами \r\n.

    void Flush();

    extern void *handlePCD;
    extern void *handleUSBD;

    extern bool cableIsConnected;
    extern bool clientIsConnected;
};
