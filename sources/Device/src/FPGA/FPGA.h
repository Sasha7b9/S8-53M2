// 2022/02/11 17:48:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"
#include "FPGA/SettingsFPGA.h"
#include "FPGA/TypesFPGA.h"


namespace FPGA
{
    static const int MAX_POINTS = 1024;

    static const int MAX_POINTS_FOR_CALCULATE = 900;

    void Init();

    // ���������� ���������� ����������� �������� � �������.
    void SetNumSignalsInSec(int numSigInSec);

    void Update();

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

    // ������� ������. ����� ��� ������ �������������, ��� ���������� ������� ����� �� �������� ���������������.
    void ClearData();

    // ���������� true,���� ��� ����� �������� � ������ �������������.
    bool AllPointsRandomizer();

    // ���������� ���������� ���������, �� ������� ����� �������������� ������ � ������ �������������.
    void SetNumberMeasuresForGates(int number);

    // ������������� ��������� �������������.
    void SwitchingTrig();

    // ���������� ��������� ����� ����� ��������� ����� - ����� ��������� ������ ������������� �� ��������� �����
    void TemporaryPause();

    // ����� � ���������� ������� ������������� �� ���������� ���������� �������
    void FindAndSetTrigLevel();

    // ��������� � ������� ������������ ���������� �������.
    void LoadKoeffCalibration(Chan::E);

    // ������ ������� ����������.
    void ProcedureCalibration();

    namespace FreqMeter
    {
        // �������� �������� �������.
        float GetFreq();
    }

    namespace AutoFinder
    {
        // ��������� ������� ������ �������.
        void StartAutoFind();
    }
}


namespace BUS_FPGA
{
    // ������ � ������� ���� ������ ��������. restart - true ��������, ��� ����� ������ ����� �������� ����� ���������, ���� �� ����� ������ �� ��������� � ������ �����.
    void WriteToHardware(uint8 *address, uint8 value, bool restart);
    void Write(uint16 *address, uint16 value, bool restart);

    void WriteToAnalog(TypeWriteAnalog::E type, uint data);

    void WriteToDAC(TypeWriteDAC::E type, uint16 data);
}
