// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Panel/Controls.h"
#include "Menu/MenuItems.h"


namespace Menu
{
    void Draw();

    // ������� ������ ���������� � ������� �����.
    void UpdateInput();

    // ����������/������ ����.
    void Show(bool);

    namespace Handlers
    {
        // ������� ��������� ��������� ������� ������ (����� 0.5 ���.).
        void ShortPressureButton(Key::E);

        // ������� ��������� �������� ������� ������ (����� 0.5 ���.).
        void LongPressureButton(Key::E);

        // ������� ����������, ����� ������ ��������� �� �������� � ������� ���������.
        void PressButton(Key::E);

        // ������� ����������, ����� ������ ��������� �� �������� � ������� ���������.
        void ReleaseButton(Key::E);

        // ������� ��������� �������� ����� ��������� ������.
        void RotateRegSetRight();

        // ������� ��������� �������� ����� ��������� �����.
        void RotateRegSetLeft();
    }

    // ���������� ����� ��������������� �������� ���� � ������������ � �����������.
    void SetAutoHide(bool active);
    // ���������� ���� � �������� ������ ���� � ��������� ����, ������� � ������ �� �����.
    char *StringNavigation(char buffer[100]);

    // ���������� ��� ��������� �������� ����.
    TypeItem::E TypeOpenedItem();

    NamePage::E GetNameOpenedPage();

    // ���������� true, ���� button - �������������� ������� [1..5].
    bool IsFunctionalButton(Key::E);

    // ������� �������� ������� ����.
    void CloseOpenedItem();

    void ShortPressOnPageItem(Page *page, int numItem);     // �������� ������� ��������� ������� ������ ��� ������ numItem �������� page;

    void ChangeStateFlashDrive();

    // ���������� �� ���� �� ������.
    bool IsShown();
};
