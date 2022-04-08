// 2022/03/29 16:52:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace LAN
{
    extern bool cableIsConnected;

    void Init();

    void Update();

    bool ClientIsConnected();

    void Init(void (*funcReciever)(const void *buffer, int length));

    void SendBuffer(const void *buffer, int length);

    void SendString(char *format, ...);
}
