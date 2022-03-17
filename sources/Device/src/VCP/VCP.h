#pragma once


namespace VCP
{
    void Init();
    void SendDataSynch(const uint8 *data, int size);
    void SendStringSynch(char *data);                   // ��������� ������ ��� ������������ ����.
    void SendStringAsynch(char *format, ...);     // ��� ������ ��������� � ������������ ��������� \r\n.
    void SendFormatStringSynch(char *format, ...);      // ��� ������ ��������� � ������������ ��������� \r\n.
    void Flush();

    extern void *handlePCD;
    extern void *handleUSBD;

    extern bool cableIsConnected;
    extern bool clientIsConnected;
};
