// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Controls.h"


namespace Keyboard
{
    void Init();

    void Update();

    namespace Buffer
    {
        // ���������� true, ���� ����� ����
        bool IsEmpty();

        int NumEvents();

        // ���������� ��������� ����� ����������, ���� ������� �������
        KeyboardEvent GetNextEvent();
    };
};
    