// 2022/02/11 17:48:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace LAN
{
    void Init();

    // Функция будет обрабатывать входной/выходной буфер timeMS миллисекунд. Это надо для передчи больших массивов информации. WARN говнокод. Надо исправить.
    void Update(uint timeMS);

    extern bool clientIsConnected;
    extern bool cableIsConnected;
};
