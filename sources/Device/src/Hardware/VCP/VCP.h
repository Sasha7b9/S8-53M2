// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define VCP_DEBUG_POINT()       VCP::DebugPoint(__MODULE__, __FUNCTION__, __LINE__)
#define VCP_FORMAT_TRACE(...)   VCP::SendFormatTrace(__MODULE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define VCP_FORMAT(...)         VCP::SendFormat(__VA_ARGS__)

namespace VCP
{
    void Init();

    void SendBuffer(const void *buffer, int size);
    void SendFormat(pchar format, ...);
    void SendString(pchar);

    void SendFormatTrace(pchar module, pchar func, int line, char *format, ...);

    void DebugPoint(pchar module, pchar function, int line);

    extern bool cableIsConnected;       // true, ���� ����������� ������
    extern bool connectToHost;          // true, ���� ���� �����������
};
