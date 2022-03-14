// 2022/02/11 17:43:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"
#include "Tables.h"


namespace Display
{
    const int WIDTH = 320;
    const int HEIGHT = 240;

    extern uint8 *const back_buffer;

    extern bool framesElapsed;
    extern int  numDrawingSignals;  // „исло нарисованных сигналов дл€ режима накоплени€

    extern int topMeasures;         // ¬ерх таблицы вывода измерений. Ёто значение нужно дл€ нормального вывода сообщений на экран - чтобы 
                                    // они ничего не перекрывали

    void Init();

    void Update(bool endScene = true);

    void RotateRShift(Chan::E);

    void RotateTrigLev();

    void EnableTrigLabel(bool enable);

    void ShowWarningBad(Warning::E warning);

    void ShowWarningGood(Warning::E warning);

    void ClearFromWarnings();

    void SetDrawMode(DrawMode::E, pFuncVV = nullptr);

    void SetAddDrawFunction(pFuncVV func);

    void RemoveAddDrawFunction();

    void Clear();

    void ShiftScreen(int delta);

    void ChangedRShiftMarkers();

    void AddStringToIndicating(pchar string);

    // ѕосле отрисовки очередного экрана эта функци€ будет вызвана один раз.
    void RunAfterDraw(pFuncVV func);

    // ќтключить вспомогательную линию маркера смещени€ по напр€жению первого канала.
    void DisableShowLevelRShiftA();

    // ќтключить вспомогательную линию маркера смещени€ по напр€жению второго канала.
    void DisableShowLevelRShiftB();
    
    void DisableShowLevelTrigLev();

    void OnRShiftMarkersAutoHide();

    // Ќаписать сообщени€ отладочной консоли.
    void DrawConsole();
};
