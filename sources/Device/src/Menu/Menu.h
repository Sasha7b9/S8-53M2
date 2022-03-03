// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Panel/Controls.h"
#include "Menu/MenuItems.h"


namespace Menu
{
    // ���������� ����� �������� ����, ���������������� ������ button.
    void* ItemUnderButton(Key::E);

    // ���������� true, ���� ������� ���� item ������ (��������� �� �� ����� ������� ����. ��� �������, ��� ��������, ��� ������� �������������� ������� ���� ����� Choice ��� Governor.
    bool IsShade(void* item);

    bool IsPressed(void* item);

    void Draw();

    // ������� ������ ���������� � ������� �����.
    void UpdateInput();

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
    // ���������� ����� ������ ����, ������������ ��� ������� � ������ ������ �������.
    void *ItemUnderKey();
    // ���������� ���� � �������� ������ ���� � ��������� ����, ������� � ������ �� �����.
    char *StringNavigation(char buffer[100]);

    void OpenItemTime();
    // ���������� true, ���� ������� ���� item �������� �������, �.�. ����� ���� �����.
    bool ItemIsActive(void *item);
    // ���������� ���������� ��������� � �������� �� ������ page.
    int NumItemsInPage(const Page * const page);
    // ���������� ����� ������� ���������� �������� �� ������ page.
    int NumCurrentSubPage(Page *page);
    // D��������� ����� ���������� � �������� �� ������ page.
    int NumSubPages(const Page *page);
    // ���������� ��� �������� ���� �� ������ address.
    TypeItem TypeMenuItem(void *address);
    // ���������� ��� ��������� �������� ����.
    TypeItem TypeOpenedItem();
    // ���������� ����� ��������� �������� ����.
    void *OpenedItem();
    // ���������� ����� �������� �������� ���� (�������, ��� �������, �������� �������, ������ �������� ���� ������ ���������.
    void *CurrentItem();
    // ���������� ����� �������� ���� �������� ��������.
    void *Item(const Page *page, int numElement);
    // ���������� true, ���� ������� ������� �������� � ������ namePage ������.
    bool CurrentItemIsOpened(NamePage namePage);
    // �������/��������� ������� ����� ��������.
    void SetCurrentItem(const void *item, bool active);
    // ���������� ����� ��������, �������� ����������� ������� �� ������ item.
    Page* Keeper(const void *item);
    // ���������� ��� �������� page.
    NamePage GetNamePage(const Page *page);

    NamePage GetNameOpenedPage();

    void OpenPageAndSetItCurrent(void *page);

    bool ChangeOpenedItem(void *item, int delta);
    // ��������� ��� ����������� �������� Governor, GovernorColor ��� Choice �� ������ item � ����������� �� ����� delta
    void ChangeItem(void *item, int delta);
    // ���������� ������ � �������� ��������� �������� Choice ��� NamePage �� ������ item.
    int HeightOpenedItem(void *item);
    // ���������� �������� �������� �� ������ item, ��� ��� �������� �� ������� �������.
    pchar  TitleItem(void *item);
    // ���������� ������� ������� �������� �������� �� ������ page �� ������. ���� ������� ����������� 0, ��� ����� 0, ���� ������� ����������� 1, ��� ����� 5 � �.�.
    int PosItemOnTop(Page *page);
    // ���������� true, ���� button - �������������� ������� [1..5].
    bool IsFunctionalButton(Key::E);
    // ������� �������� ������� ����.
    void CloseOpenedItem();

    void OpenItem(const void *item, bool open);          // �������/������� ������� ���� �� ������ item.

    bool ItemIsOpened(void *item);                       // ���������� true, ���� ������� ���� �� ������ item ������.

    void ChangeSubPage(const Page *page, int delta);

    void ShortPressOnPageItem(Page *page, int numItem);   // �������� ������� ��������� ������� ������ ��� ������ numItem �������� page;

    Page *PagePointerFromName(NamePage namePage);

    bool PageIsSB(const Page *page);

    SmallButton *SmallButonFromPage(Page *page, int numButton);

    void ChangeStateFlashDrive();

    int8 PosActItem(NamePage namePage);                         // ���������� ������� ��������� ������ �� �������� namePage.
};
