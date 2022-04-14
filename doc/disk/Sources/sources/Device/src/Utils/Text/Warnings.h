// 2022/03/23 12:17:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Warning
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
        SignalNotFound,                 // ������ �� ������
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
        SettingsSaved,
        SettingsNotSaved,
        InstrumentNotCalibrated,
        Count
    };

    // �������� ��������������.
    void DrawWarnings();

    void ShowBad(E warning);

    void ShowGood(E warning);
};
