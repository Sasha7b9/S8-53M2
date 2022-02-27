// 2022/2/11 19:33:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsCursors.h"


struct PageChannelA
{
    static void OnChanged_Input(bool active);
};


struct PageChannelB
{
    static void OnChanged_Input(bool active);
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

    static void *GetPointer();
};


struct PageDebug
{
    static void LoadStretchADC(Chan::E);

    struct SerialNumber
    {
        static void *GetPointer();
    };
};


struct PageHelp
{
    static void *GetPointer();
};


struct PageMeasures
{
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
        static void *GetPointer();
    };

    struct Internal
    {
        static void *GetPointer();
    };

    struct SetMask
    {
        static void *GetPointer();
    };

    struct SetName
    {
        static void *GetPointer();
    };

    struct FileManager
    {
        static void *GetPointer();
    };

    static void *GetPointer();
};


struct PageService
{
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
        static void *GetPointer();
    };
};


#define COMMON_BEGIN_SB_EXIT  0, "�����", "Exit", "������ ��� ������ � ���������� ����", "Button for return to the previous menu"


const void* PageForButton(Key::E);          // ���������� �������� ����, ������� ������ ����������� �� ������� ������ button.
bool IsMainPage(void *item);                            // ���������� true, ���� item - ����� ������� �������� ����.


void CalculateConditions(int16 pos0, int16 pos1, CursCntrl, bool *cond0, bool *cond1);    // ������������ ������� ��������� ��� ����� ������ ���������� ������� ���������.
void DrawMenuCursTime(int x, int y, bool left, bool right);
void DrawSB_Exit(int x, int y);
