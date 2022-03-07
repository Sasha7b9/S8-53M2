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

    extern bool needFinishDraw;     // Если true, то дисплей нуждается в перерисовке
    extern bool framesElapsed;
    extern int  numDrawingSignals;  // Число нарисованных сигналов для режима накопления

    extern int topMeasures;         // Верх таблицы вывода измерений. Это значение нужно для нормального вывода сообщений на экран - чтобы 
                                    // они ничего не перекрывали

    void Init();

    void Update(bool endScene = true);

    void RotateRShift(Chan::E);

    void RotateTrigLev();

    void Redraw();

    void EnableTrigLabel(bool enable);

    void ShowWarningBad(Warning::E warning);

    void ShowWarningGood(Warning::E warning);

    void ClearFromWarnings();

    void SetDrawMode(DrawMode mode, pFuncVV func);

    void SetAddDrawFunction(pFuncVV func);

    void RemoveAddDrawFunction();

    void Clear();

    void ShiftScreen(int delta);

    void ChangedRShiftMarkers();

    void AddStringToIndicating(pchar string);

    // После отрисовки очередного экрана эта функция будет вызвана один раз.
    void RunAfterDraw(pFuncVV func);

    // Отключить вспомогательную линию маркера смещения по напряжению первого канала.
    void DisableShowLevelRShiftA();

    // Отключить вспомогательную линию маркера смещения по напряжению второго канала.
    void DisableShowLevelRShiftB();
    
    void DisableShowLevelTrigLev();

    void OnRShiftMarkersAutoHide();
};

