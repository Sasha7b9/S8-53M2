// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/Timer.h"


//#define VCP_DEBUG_POINT()       VCP::DebugPoint(__MODULE__, __FUNCTION__, __LINE__)
//#define VCP_FORMAT_TRACE(...)   VCP::SendFormatTrace(__MODULE__, __FUNCTION__, __LINE__, __VA_ARGS__)
//#define VCP_FORMAT(...)         VCP::SendFormat(__VA_ARGS__)

namespace VCP
{
    void Init();

    // ������� ��������������� ������ � ����������� �������� '\n'
    void SendFormat0D(pchar format, ...);

    void SendFormatTrace(pchar module, pchar func, int line, char *format, ...);

    void DebugPoint(pchar module, pchar function, int line);

    extern bool cableIsConnected;       // true, ���� ���������� ������
    extern bool connectToHost;          // true, ���� ���� �����������

    extern TimeMeterMS _meter;
    extern uint _sended_bytes;

    namespace Buffer
    {
        void Send(const void *buffer, int size);
        void Flush();
    }
};
