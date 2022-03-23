// 2022/03/23 12:17:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


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

    // Написать предупреждения.
    static void DrawWarnings();

    static void ShowBad(E warning);

    static void ShowGood(E warning);

    static void Clear();

private:

    static void ShowWarn(pchar);
};
