// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Panel/Controls_Old.h"
#include "Menu/MenuItems.h"


namespace Menu
{
    extern bool showDebugMenu;
    extern bool needClosePageSB;    // ���� 1, ����� ��������� �������� ����� ������

    namespace Handlers
    {
        // ������� ��������� ��������� ������� ������ (����� 0.5 ���.).
        void ShortPressureButton(KeyOld::E);

        // ������� ��������� �������� ������� ������ (����� 0.5 ���.).
        void LongPressureButton(KeyOld::E);

        // ������� ����������, ����� ������ ��������� �� �������� � ������� ���������.
        void PressButton(KeyOld::E);

        // ������� ����������, ����� ������ ��������� �� �������� � ������� ���������.
        void ReleaseButton(KeyOld::E);

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
};
