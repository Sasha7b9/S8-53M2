#pragma once


namespace VCP
{
    void Init();

    void SendSynch(const uint8 *data, int size);
    void SendSynch(char *data);                   // ��������� ������ ��� ������������ ����.
    void SendSynch(char *format, ...);      // ��� ������ ��������� � ������������ ��������� \r\n.

    void SendAsynch(char *format, ...);     // ��� ������ ��������� � ������������ ��������� \r\n.

    void Flush();

    extern void *handlePCD;
    extern void *handleUSBD;

    extern bool cableIsConnected;
    extern bool clientIsConnected;
};
