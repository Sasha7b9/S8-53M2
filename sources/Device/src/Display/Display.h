// 2022/02/11 17:43:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"
#include "Tables.h"


namespace Display
{
    extern uint8 back[240][320];
    extern uint8 *display_back_buffer;
    extern uint8 *display_back_buffer_end;

    const int WIDTH = 320;
    const int HEIGHT = 240;

    void Init();

    void Update(bool endScene = true);

    void RotateRShift(Chan::E ch);

    void RotateTrigLev();

    void Redraw();

    void EnableTrigLabel(bool enable);
    // ������� ������ ���������� ��� ������� ���������� ������. ���������� ��� ��������� �������� �� ����������.
    void ResetP2Ppoints(bool empty);
    // ��������� ����� � ������ ����������� ������ � ��������� data00, data01 - ��� ������� ������; data10, data11 - ��� ������� ������
    void AddPoints(uint8 data00, uint8 data01, uint8 data10, uint8 data11);

    void ShowWarningBad(Warning warning);

    void ShowWarningGood(Warning warning);

    void ClearFromWarnings();

    void SetDrawMode(DrawMode mode, pFuncVV func);

    void SetAddDrawFunction(pFuncVV func);

    void RemoveAddDrawFunction();

    void Clear();

    void ShiftScreen(int delta);

    void ChangedRShiftMarkers();

    void AddStringToIndicating(const char *string);

    void OneStringUp();

    void OneStringDown();

    void SetPauseForConsole(bool pause);
    // ����� ��������� ���������� ������ ��� ������� ����� ������� ���� ���.
    void RunAfterDraw(pFuncVV func);
    // ��������� ��������������� ����� ������� �������� �� ���������� ������� ������.
    void DisableShowLevelRShiftA();
    // ��������� ��������������� ����� ������� �������� �� ���������� ������� ������.
    void DisableShowLevelRShiftB();
    
    void DisableShowLevelTrigLev();

    void OnRShiftMarkersAutoHide();
};

