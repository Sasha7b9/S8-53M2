// 2022/2/11 19:47:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsTypes.h"


void  Cursors_Update();    // �������� ��� ������� ��� ������� ���������, ����� �������� ������� ��������, ���� ��� ������ ����������� �������������.


class PageCursors
{
public:
    
    class PageSet
    {
    public:
        static void *pointer;
    };

    static void *GetPointer();
};
