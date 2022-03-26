// 2022/2/11 19:33:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsCursors.h"
#include "Display/Text.h"


class Page;


struct PageMain
{
    static const Page *self;

    static void EnablePageDebug();
};


struct PageChannelA
{
    static void OnChanged_Input(bool active);

    static const Page *self;
};


struct PageChannelB
{
    static void OnChanged_Input(bool active);

    static const Page *self;
};


struct PageCursors
{
    // �������� ������� ������� ����������.
    static float GetCursPosU(Chan, int numCur);

    // ���������� true,���� ����� �������� �������.
    static bool NecessaryDrawCursors();

    // �������� ������ ������� ����������.
    static String GetCursVoltage(Chan, int numCur);

    // �������� ������ ������� �������.
    static String GetCursorTime(Chan, int numCur);

    // �������� ������ ��������� �������� ����������.
    static String GetCursorPercentsU(Chan);

    // �������� ������ ��������� �������� �������.
    static String GetCursorPercentsT(Chan);

    static void DrawMenuCursVoltage(int x, int y, bool top, bool bottom);

    static void Cursors_Update();    // �������� ��� ������� ��� ������� ���������, ����� �������� ������� ��������, ���� ��� ������ ����������� �������������.

    struct PageSet
    {
        static void *pointer;
    };

    static const Page *self;
};


struct PageDebug
{
    static const Page *self;

    struct SerialNumber
    {
        static const Page *self;
    };

    struct PageADC
    {
        // �������� ������������� ������������ �������� ������
        static void ResetCalRShift(Chan);

        static void ResetCalStretch(Chan);
    };
};


struct PageDisplay
{
    static const Page *self;
};


struct PageHelp
{
    static const Page *self;
};


struct PageMeasures
{
    static const Page *self;

    struct Tune
    {
        static void *pointer;
    };

    // ���� true, �� ������� ����� ���� ��������� ��� ������ �� �������� ���������-���������
    static bool choiceMeasuresIsActive;
};


struct PageMemory
{
    static void OnChanged_NumPoints(bool active = true);
    static void OnPressExtFileManager();
    static void SaveSignalToFlashDrive();

    struct Latest
    {
        static int16 current;               // ������� ����� ���������� ������� � ������ ������ - ���������

        static const Page *self;
    };

    struct Internal
    {
        static int8 currentSignal;          // ������� ����� �������, ����������� � ����
        static bool showAlways;             // ���� true, �� ���������� ������ ��������� � ������ "�����. ��" ������
        static bool exitToLast;             // ���� true, �� �������� �� �������� ���������� ������ ����� �� ����������, � � ���� ���������

        static const Page *self;
    };

    struct External
    {
        static const Page *self;
    };

    struct SetMask
    {
        static const Page *self;
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
        static const Page *self;
    };

    struct FileManager
    {
        static const Page *self;
    };

    static const Page *self;
};


struct PageService
{
    static const Page *self;

    struct Math
    {
        static bool Enabled();

        struct Function
        {
            static const Page *self;
        };

        struct FFT
        {
            struct Cursors
            {
                static const Page *self;
            };

            static const Page *self;
        };
    };

    struct Information
    {
        static const Page *self;
    };

    struct Calibrator
    {
        static const Page *self;
    };
};


struct PageTime
{
    static const Page *self;
};


struct PageTrig
{
    static const Page *self;
};


#define COMMON_BEGIN_SB_EXIT  0, "�����", "Exit", "������ ��� ������ � ���������� ����", "Button for return to the previous menu"


bool IsMainPage(void *item);            // ���������� true, ���� item - ����� ������� �������� ����.


// ������������ ������� ��������� ��� ����� ������ ���������� ������� ���������.
void CalculateConditions(int16 pos0, int16 pos1, CursCntrl::E, bool *cond0, bool *cond1);
void DrawMenuCursTime(int x, int y, bool left, bool right);
void DrawSB_Exit(int x, int y);
