// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsDisplay.h"


struct Warning
{
    enum E
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
        SignalNotFound,                 // Сигнал не найден
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
        Count
    };
};

enum WarningWithNumber
{
    ExcessValues        // Превышение значения количества сигналов в "НАКОПЛЕНИЕ", "УСРЕДНЕНИЕ", "Мин Макс"
};


namespace Tables
{
    pchar GetWarning(Warning::E warning);

    // Выводит строку из таблицы symbolsAlphaBet
    void DrawStr(int index, int x, int y);

    extern pchar symbolsAlphaBet[0x48];
}

