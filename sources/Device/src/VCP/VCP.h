#pragma once


namespace VCP
{
    void Init();

    void SendSynch(const uint8 *data, int size);
    void SendStringSynch(char *data);                   // ��������� ������ ��� ������������ ����.
    void SendStringSynch(char *format, ...);      // ��� ������ ��������� � ������������ ��������� \r\n.

    void SendStringAsynch(char *format, ...);     // ��� ������ ��������� � ������������ ��������� \r\n.

    void Flush();

    extern void *handlePCD;
    extern void *handleUSBD;

    extern bool cableIsConnected;
    extern bool clientIsConnected;
};
