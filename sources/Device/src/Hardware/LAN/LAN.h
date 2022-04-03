// 2022/03/29 16:52:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace LAN
{
    extern bool cableIsConnected;
    extern bool clientIsConnected;

    void Init();

    void Update();

    void SendBuffer(const void *buffer, int length);
    void SendFormat(pchar format, ...);
    void SendString(pchar);
}
