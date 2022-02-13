// 2022/02/11 17:48:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA_Types.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"
#include "FPGA/SettingsFPGA.h"


namespace FPGA
{
    static const int MAX_POINTS = 1024;

    void Init();

    // ���������� ���������� ����������� �������� � �������.
    void SetNumSignalsInSec(int numSigInSec);

    void Update();

    // ������ � ������� ���� ������ ��������. restart - true ��������, ��� ����� ������ ����� �������� ����� ���������, ���� �� ����� ������ �� ��������� � ������ �����.
    void WriteToHardware(uint8 *address, uint8 value, bool restart);

    // ��������� ���� ����� ����������.
    void OnPressStartStop();

    // ������ �������� ����� ����������.
    void Start();

    // ��������� ������� ����� ����������.
    void Stop(bool pause);

    // ���������� true, ���� ������ ��������� �� � �������� ����� ����������.
    bool IsRunning();

    // ��������� ������� ���������. ����� �� ����� ������������ �������� FPGA_RestoreState().
    void SaveState();

    // ��������������� ���������, ����� ���������� �������� FPGA_SaveState().
    void RestoreState();

    // �������� �������� �������.
    float GetFreq();

    // ������� ������. ����� ��� ������ �������������, ��� ���������� ������� ����� �� �������� ���������������.
    void ClearData();

    // ���������� �������������� ��������. ����� ��� ���������� ����������� ����� � ������ �������������.
    void SetAdditionShift(int shift);

    // ���������� true,���� ��� ����� �������� � ������ �������������.
    bool AllPointsRandomizer();

    // ���������� ���������� ���������, �� ������� ����� �������������� ������ � ������ �������������.
    void SetNumberMeasuresForGates(int number);

    // ������������� ��������� �������������.
    void SwitchingTrig();

    // ��������� ������� ������ �������.
    void StartAutoFind();

    // ���������� ��������� ����� ����� ��������� ����� - ����� ��������� ������ ������������� �� ��������� �����
    void TemporaryPause();

    void FillDataPointer(DataSettings *dp);

    // ����� � ���������� ������� ������������� �� ���������� ���������� �������
    void FindAndSetTrigLevel();

    // ���������� ����� ������ �� �����.
    void SetModeCouple(Chan::E ch, ModeCouple modeCoupe);

    // ��������/��������� ������ �� ����� ������.
    void EnableChannelFiltr(Chan::E ch, bool enable);

    // ���������� ������� �� ����������.
    void SetRange(Chan::E ch, Range range);

    // ��������� ������� �� ����������.
    bool RangeIncrease(Chan::E ch);

    // ��������� ������� �� ����������.
    bool RangeDecrease(Chan::E ch);

    // ���������� ������� �� �������.
    void SetTBase(TBase tBase);

    // ��������� ������� �� �������.
    void TBaseDecrease();

    // ��������� ������� �� �������.
    void TBaseIncrease();

    // ���������� ������������� �������� �� ����������.
    void SetRShift(Chan::E ch, int16 rShift);

    // ���������� ������������� �������� �� �������.
    void SetTShift(int tShift);

    // ���������� ���������� �������� �� ������� ��� ������ �������������. � ������ �������� ��� �������� ������ ���� ������.
    void SetDeltaTShift(int16 shift);

    // ��������/��������� ����� �������� ���������.
    void SetPeackDetMode(PeackDetMode peackDetMode);

    // ��������/��������� ����������.
    void SetCalibratorMode(CalibratorMode calibratorMode);

    // ��������� � ������� ������������ ���������� �������.
    void LoadKoeffCalibration(Chan::E ch);

    // ���������� ������������� ������� �������������.
    void SetTrigLev(TrigSource chan, int16 trigLev);

    // ���������� �������� �������������.
    void SetTrigSource(TrigSource trigSource);

    // ���������� ���������� �������������.
    void SetTrigPolarity(TrigPolarity polarity);

    // ���������� ����� ����� �������������.
    void SetTrigInput(TrigInput trigInput);

    // ���������� ������������� �������� �� ������� � ��������� ����, ��������� ��� ������ �� �����.
    const char* GetTShiftString(int16 tShiftRel, char buffer[20]);

    // ������ ������� ����������.
    void ProcedureCalibration();

    StateWorkFPGA CurrentStateWork();
};
