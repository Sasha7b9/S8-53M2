// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Controls.h"


namespace Keyboard
{
    void Init();

    void Update();

    namespace Buffer
    {
        // Возвращает true, если буфер пуст
        bool IsEmpty();

        int NumEvents();

        // Возвращает следующий орган управления, если таковой имеется
        KeyboardEvent GetNextEvent();
    };
};
    