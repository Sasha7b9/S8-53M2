// 2022/2/11 19:33:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsCursors.h"


class Page;


struct PageChannelA
{
    static void OnChanged_Input(bool active);

    static Page *self;
};


struct PageChannelB
{
    static void OnChanged_Input(bool active);

    static Page *self;
};


struct PageCursors
{
    // �������� ������� ������� ����������.
    static float GetCursPosU(Chan::E, int numCur);

    // ���������� true,���� ����� �������� �������.
    static bool NecessaryDrawCursors();

    // �������� ������ ������� ����������.
    static pchar GetCursVoltage(Chan::E source, int numCur, char buffer[20]);

    // �������� ������ ������� �������.
    static pchar GetCursorTime(Chan::E source, int numCur, char buffer[20]);

    // �������� ������ ��������� �������� ����������.
    static pchar GetCursorPercentsU(Chan::E source, char buffer[20]);

    // �������� ������ ��������� �������� �������.
    static pchar GetCursorPercentsT(Chan::E source, char buffer[20]);

    static void DrawMenuCursVoltage(int x, int y, bool top, bool bottom);

    static void Cursors_Update();    // �������� ��� ������� ��� ������� ���������, ����� �������� ������� ��������, ���� ��� ������ ����������� �������������.

    struct PageSet
    {
        static void *pointer;
    };

    static Page *self;
};


struct PageDebug
{
    struct SerialNumber
    {
        static Page *self;
    };
};


struct PageDisplay
{
    static Page *self;
};


struct PageHelp
{
    static Page *self;
};


struct PageMeasures
{
    static Page *self;

    struct Tune
    {
        static void *pointer;
    };

    // ���� true, �� ������� ����� ���� ��������� ��� ������ �� �������� ���������-���������
    static bool choiceMeasuresIsActive;
};


struct PageMemory
{
    static void OnChanged_NumPoints(bool active);
    static void OnPressExtFileManager();
    static void SaveSignalToFlashDrive();
    static const void *pMspFileManager;

    struct Latest
    {
        static int16 current;               // ������� ����� ���������� ������� � ������ ������ - ���������

        static const Page *GetPointer();
    };

    struct Internal
    {
        static int8 currentSignal;          // ������� ����� �������, ����������� � ����
        static bool showAlways;             // ���� true, �� ���������� ������ ��������� � ������ "�����. ��" ������
        static bool exitToLast;             // ���� true, �� �������� �� �������� ���������� ������ ����� �� ����������, � � ���� ���������

        static Page *self;
    };

    struct SetMask
    {
        static const Page *GetPointer();
    };

    struct SetName
    {
#define RETURN_TO_MAIN_MENU             0U
#define RETURN_TO_LAST_MEM              1U
#define RETURN_TO_INT_MEM               2U
#define RETURN_TO_DISABLE_MENU          3U

#define EXIT_FROM_SET_NAME_TO_MAIN_MENU (PageMemory::SetName::exitTo == RETURN_TO_MAIN_MENU)
#define EXIT_FROM_SET_NAME_TO_LAST      (PageMemory::SetName::exitTo == RETURN_TO_LAST_MEM)
#define EXIT_FROM_SET_NAME_TO_INT       (PageMemory::SetName::exitTo == RETURN_TO_INT_MEM)
#define EXIT_FROM_SET_NAME_TO_DIS_MENU  (PageMemory::SetName::exitTo == RETURN_TO_DISABLE_MENU)
        static uint8 exitTo;    // ���� ������������ �� ���� ��������� ����� ��� ���������� : 0 - � �������� ����, 1 - � ���� 
                                // ���������, 2 - � ���� ����� ��, 3 - � ������� ���� � ����������� ����
        static Page *self;
    };

    struct FileManager
    {
        static const Page *GetPointer();
    };

    static void *GetPointer();
};


struct PageService
{
    static Page *self;

    struct Math
    {
        static bool Enabled();

        struct Function
        {
            static void *GetPointer();
        };

        struct FFT
        {
            struct Cursors
            {
                static void *GetPointer();
            };
        };
    };

    struct Information
    {
        static const Page *GetPointer();
    };
};


#define COMMON_BEGIN_SB_EXIT  0, "�����", "Exit", "������ ��� ������ � ���������� ����", "Button for return to the previous menu"


bool IsMainPage(void *item);            // ���������� true, ���� item - ����� ������� �������� ����.


void CalculateConditions(int16 pos0, int16 pos1, CursCntrl, bool *cond0, bool *cond1);    // ������������ ������� ��������� ��� ����� ������ ���������� ������� ���������.
void DrawMenuCursTime(int x, int y, bool left, bool right);
void DrawSB_Exit(int x, int y);
