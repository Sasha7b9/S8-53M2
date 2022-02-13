// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsDisplay.h"


enum Warning
{
    LimitChan1_Volts,
    LimitChan2_Volts,
    LimitSweep_Time,
    EnabledPeakDet,
    LimitChan1_RShift,
    LimitChan2_RShift,
    LimitSweep_Level,
    LimitSweep_TShift,
    TooSmallSweepForPeakDet,
    TooFastScanForSelfRecorder,
    FileIsSaved,
    SignalIsSaved,
    SignalIsDeleted,
    MenuDebugEnabled,
    MovingData,
    TimeNotSet,
    SignalNotFound,
    SetTPosToLeft1,
    SetTPosToLeft2,
    SetTPosToLeft3,
    VerySmallMeasures,
    NeedRebootDevice1,
    NeedRebootDevice2,
    ImpossibleEnableMathFunction,
    ImpossibleEnableFFT,
    FirmwareSaved,
    FullyCompletedOTP,
    Warning_NumWarnings
};

enum WarningWithNumber
{
    ExcessValues        // Превышение значения количества сигналов в "НАКОПЛЕНИЕ", "УСРЕДНЕНИЕ", "Мин Макс"
};


ENumSignalsInSec Tables_NumSignalsInSecToENUM(int enumSignalsInSec);

int Tables_ENUMtoNumSignalsInSec(ENumSignalsInSec enumSignalsInSec);

const char* Tables_GetWarning(Warning warning);

const char* Tables_GetTBaseString(TBase::E);

const char* Tables_GetTBaseStringEN(TBase::E);

const char* Tables_RangeNameFromValue(Range range);
// Возвращает номер канала от 1 до 2
int Tables_GetNumChannel(Chan::E);

extern const char* symbolsAlphaBet[0x48];
// Выводит строку из таблицы symbolsAlphaBet
void DrawStr(int index, int x, int y);
