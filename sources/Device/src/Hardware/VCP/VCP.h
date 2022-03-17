// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


//#define VCP_DEBUG_POINT()       VCP::DebugPoint(__MODULE__, __FUNCTION__, __LINE__)



namespace VCP
{
    void Init();

    void Send(const uint8 *buffer, int size);

    void SendFormat(char *format, ...);

    void DebugPoint(pchar module, pchar function, int line);

    extern bool cableIsConnected;       // true, если подсоединён кабель
    extern bool connectToHost;          // true, если есть подключение
};
