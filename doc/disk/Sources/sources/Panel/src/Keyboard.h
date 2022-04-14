// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "common/Panel/Controls.h"


namespace Keyboard
{
    void Init();

    void Update();

    namespace Buffer
    {
        // ���������� true, ���� ����� ����
        bool Empty();

        int NumEvents();

        // ���������� ��������� ����� ����������, ���� ������� �������
        KeyboardEvent GetNextEvent();
    };
};
