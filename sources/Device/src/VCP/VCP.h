#pragma once


namespace VCP
{
    void Init();
    void SendDataSynch(const uint8 *data, int size);
    void SendStringSynch(char *data);                   // Передаётся строка без завершающего нуля.
    void SendFormatStringAsynch(char *format, ...);     // Эта строка передаётся с завершающими символами \r\n.
    void SendFormatStringSynch(char *format, ...);      // Эта строка передаётся с завершающими символами \r\n.
    void Flush();
    void Update();

    extern void *handlePCD;
    extern void *handleUSBD;
};
