// 2022/2/11 19:47:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


enum TypePageHelp
{
    TypePage_Content,       // �� ���� �������� ����������� ������ �� ������ ��������
    TypePage_Description    // �� ���� �������� ��� ������, ������ ��������
};


#define MAX_PAGES 3

/** @todo ���������� �� ��������� funcNotUsed */

struct PageHelp
{
    TypePageHelp    type;
    void*           parent;             // ����� ������������ ��������
    pFuncBV         funcNotUsed;        // ��������� ��� ������������� � ����� Page - ����� ��� ���������.
    char*           titleHint[4];       // �������� �������� �� ������� � ���������� ������, � ����� ���������� �� ������� � ���������� (��� ������ TypePage_Description)
    void*           pages[MAX_PAGES];   // ������ �������� ������ ��������� ������� � ������ TypePage_Content
};

extern const PageHelp helpMenu;

static const PageHelp helpMenuCommon =
{
    TypePage_Description, (void*)&helpMenu, 0,
    {
        "����� �������� ��������� ����", "General description of the principles of the menu"
        ,
        "������ �� ������ ���������� ����� ��� ���� ������� - ��������, ������������� ����� 0.5 ��� � �������, ������������� ����� 0.5 ���. "
        "���� ������������ ����� ����������� ��������� ���������. "
        "������� �������� ���� ����������� �������� ���� ������� �������� ������ ����. "
        "�� ���� ��������� ������� �������� ������� ���������������� ������ � ������� ������� ����� �� ������ �� ������ ����������:\n"
        "\"�������\" - �������\n"
        "\"����� 1\" - ����� 1\n"
        "\"����� 2\" - ����� 2\n"
        "\"�����.\" - �����\n"
        "\"����E����\" - ����\n"
        "\"�������\" - �������\n"
        "\"������\" - ������\n"
        "\"���������\" - �����\n"
        "\"������\" - ������\n"
        "\"������\" - ������\n"
        "� ������ �������� ������� �������� ������� ��������� ��������������� ���������"
        ,
        ""
    }
};

static const PageHelp helpMenuControls =
{
    TypePage_Description, (void*)&helpMenu, 0,
    {
        "�������� ������� �����������", "Description of the controls"
        ,
        "",
        ""
    }
};

extern const PageHelp helpMain;

static const PageHelp helpSCPI =
{
    TypePage_Description, (void*)&helpMain, 0,
    {
        "������ � SCPI", "Working with SCPI",
        "",
        ""
    }
};

static const PageHelp helpMenu =
{
    TypePage_Content, (void*)&helpMain, 0,
    {
        "������ � ����", "Working with menus",
        "",
        ""
    },
    {
        (void*)&helpMenuCommon,
        (void*)&helpMenuControls
    }
};

static const PageHelp helpMain =
{
    TypePage_Content,  0, 0,
    {
        "������", "HELP",
        "",
        ""
    },
    {
        (void*)&helpMenu,
        (void*)&helpSCPI
    }
};
