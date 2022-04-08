// 2022/02/11 17:43:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"


namespace Display
{
    const int WIDTH = 320;
    const int HEIGHT = 240;

    static const int DELTA = 5;

    extern uint8 *const back_buffer;

    extern bool framesElapsed;

    extern int topMeasures;         // ���� ������� ������ ���������. ��� �������� ����� ��� ����������� ������ ��������� �� ����� - ����� 
                                    // ��� ������ �� �����������

    void Init();

    void Update();

    void Update1(bool endScene = true);

    void RotateRShift(Chan);

    void RotateTrigLev();

    void SwitchTrigLabel(bool enable);

    void SetDrawMode(DrawMode::E, pFuncVV = nullptr);

    void Clear();

    void ShiftScreen(int delta);

    // ����� ��������� ���������� ������ ��� ������� ����� ������� ���� ���.
    void RunAfterDraw(pFuncVV func);

    namespace Sender
    {
        extern bool needSendPalette;
        extern bool needSendFrame;
    }
};
