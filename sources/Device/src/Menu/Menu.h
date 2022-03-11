// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Menu/MenuItems.h"
#include "common/Panel/Controls.h"


namespace Menu
{
    extern bool showDebugMenu;
    extern bool needClosePageSB;    // ���� 1, ����� ��������� �������� ����� ������

    namespace Handlers
    {
        // ������� ��������� �������� ����� ��������� ������.
        void RotateRegSetRight();

        // ������� ��������� �������� ����� ��������� �����.
        void RotateRegSetLeft();
    }

    void Draw();

    // ������� ������ ���������� � ������� �����.
    void UpdateInput();

    // ����������/������ ����.
    void Show(bool);

    // ���������� ����� ��������������� �������� ���� � ������������ � �����������.
    void SetAutoHide(bool active);

    // ���������� ���� � �������� ������ ���� � ��������� ����, ������� � ������ �� �����.
    char *StringNavigation(char buffer[100]);

    void ChangeStateFlashDrive();

    // ���������� �� ���� �� ������.
    bool IsShown();

    // ���� true - ���� ��������� � �������������� ������.
    bool IsMinimize();

    // ���������� ������� ��������� �������� ������� �� ������� ���� item.
    void ExecuteFuncForLongPressureOnItem(Item *);

    // ���������� ������� ��������� ��������� ������� �� ������� ���� item.
    void ExecuteFuncForShortPressOnItem(Item *);

    void ProcessButtonForHint(Key::E);
};
