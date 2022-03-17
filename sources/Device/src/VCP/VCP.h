#pragma once


#define VCP_DEBUG_POINT()       VCP::SendDebugPoint(__MODULE__, __FUNC__, __LINE__)


namespace VCP
{
    void Init();

    void SendDebugPoint(pchar module, pchar func, int line);

    void SendSynch(const uint8 *data, int size);
    void SendStringSynch(char *data);                     // ��������� ������ ��� ������������ ����.
    void SendStringSynchZ(char *);                        // �������� ������ � ����������� ����
    void SendSynch(char *format, ...);              // ��� ������ ��������� � ������������ ��������� \r\n.

    void SendAsynch(char *format, ...);             // ��� ������ ��������� � ������������ ��������� \r\n.

    void Flush();

    extern void *handlePCD;
    extern void *handleUSBD;

    extern bool cableIsConnected;
    extern bool clientIsConnected;
};
