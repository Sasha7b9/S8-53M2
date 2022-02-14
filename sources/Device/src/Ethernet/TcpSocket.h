// 2022/02/11 17:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


struct tcp_pcb;

static const int    DEFAULT_PORT = 7,
                    POLICY_PORT = 843;

bool TCPSocket_Init(void (*funcConnect)(), void (*funcReciever)(pchar buffer, uint length));

bool TCPSocket_Send(pchar buffer, uint length);

void TCPSocket_SendFormatString(char *format, ...);
