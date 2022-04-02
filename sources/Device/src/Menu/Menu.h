// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Menu/MenuItems.h"
#include "common/Panel/Controls.h"


// ����� �������� �����, �������������� ��������������� �������
struct ItemsUnderKey
{
    static void Set(Key::E, Item *);
    static Item *Get(Key::E);
    static void Reset();
private:
    static Item *items[Key::Count];
};


namespace Menu
{
    extern bool showDebugMenu;
    extern bool needClosePageSB;    // ���� 1, ����� ��������� �������� ����� ������

    void Draw();

    // ������� ������ ���������� � ������� �����.
    void UpdateInput();

    // ����������/������ ����.
    void Show(bool);

    // ���������� ����� ��������������� �������� ���� � ������������ � �����������.
    void SetAutoHide(bool active);

    // ���������� ���� � �������� ������ ���� � ��������� ����, ������� � ������ �� �����.
    String<> StringNavigation();

    void ChangeStateFlashDrive();

    // ���������� �� ���� �� ������.
    bool IsShown();

    // ���� true - ���� ��������� � �������������� ������.
    bool IsMinimize();

    // ���������� ������� ��������� �������� ������� �� ������� ���� item.
    void ExecuteFuncForLongPressureOnItem(Item *);

    // ���������� ������� ��������� ��������� ������� �� ������� ���� item.
    void ExecuteFuncForShortPressOnItem(Item *);
};
