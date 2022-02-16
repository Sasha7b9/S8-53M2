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


#define COMMON_BEGIN_SB_EXIT  0, "�����", "Exit", "������ ��� ������ � ���������� ����", "Button for return to the previous menu"


const void* PageForButton(PanelButton);          // ���������� �������� ����, ������� ������ ����������� �� ������� ������ button.
bool IsMainPage(void *item);                            // ���������� true, ���� item - ����� ������� �������� ����.


void DrawMenuCursVoltage(int x, int y, bool top, bool bottom);
void CalculateConditions(int16 pos0, int16 pos1, CursCntrl, bool *cond0, bool *cond1);    // ������������ ������� ��������� ��� ����� ������ ���������� ������� ���������.
void DrawMenuCursTime(int x, int y, bool left, bool right);
void DrawSB_Exit(int x, int y);
