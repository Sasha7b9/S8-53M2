// 2022/02/11 17:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


struct tcp_pcb;

static const int    DEFAULT_PORT = 7,
                    POLICY_PORT = 843;

namespace SocketTCP
{
    bool Init(void (*funcConnect)(), void (*funcReciever)(pchar buffer, uint length));

    bool Send(pchar buffer, uint length);

    void SendFormatString(char *format, ...);
}
