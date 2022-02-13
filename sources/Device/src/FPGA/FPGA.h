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

    // ����� � ���������� ������� ������������� �� ���������� ���������� �������
    void FindAndSetTrigLevel();

    // ���������� ����� ������ �� �����.
    void SetModeCouple(Chan::E, ModeCouple::E modeCoupe);

    // ��������/��������� ������ �� ����� ������.
    void EnableChannelFiltr(Chan::E, bool enable);

    // ��������/��������� ����� �������� ���������.
    void SetPeackDetMode(PeackDetMode peackDetMode);

    // ��������/��������� ����������.
    void SetCalibratorMode(CalibratorMode calibratorMode);

    // ��������� � ������� ������������ ���������� �������.
    void LoadKoeffCalibration(Chan::E);

    // ������ ������� ����������.
    void ProcedureCalibration();
};
