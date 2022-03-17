// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define VCP_DEBUG_POINT()       VCP::DebugPoint(__MODULE__, __FUNCTION__, __LINE__)



namespace VCP
{
    void Init();

    void Send(const uint8 *buffer, int size);

    void SendAsynch(char *format, ...);

    // ��������� ��������� ��� ������������ ����
    void SendMessage(pchar message);

    void DebugPoint(pchar module, pchar function, int line);

    void Flush();

    extern bool cableIsConnected;       // true, ���� ���������� ������
    extern bool connectToHost;          // true, ���� ���� �����������
};
