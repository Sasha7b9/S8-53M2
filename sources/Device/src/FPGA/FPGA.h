// 2022/02/11 17:48:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"
#include "FPGA/SettingsFPGA.h"
#include "FPGA/TypesFPGA.h"
#include "FPGA/StructuresFPGA.h"
#include "Settings/Settings.h"


namespace FPGA
{
    static const int MAX_POINTS = 1024;

    extern bool AUTO_FIND_IN_PROGRESS;

    extern bool runningBeforeSmallButtons;      // ����� ����������� ���������� � ���, �������� �� ���� ����� ��������� � ����� ������ � �������

    extern StateFPGA state;

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

    namespace Compactor
    {
        // ����  true - ����� ��������� ������ ����������� ��������� ���������� �� 2 �������� �
        // ������������ �����
        inline bool Enabled() { return SET_FPGA_COMPACT; };

        // ����������� ���������� - ������ ���� 1, 4, ��� 5.
        // ������������ ��� ������� SET_TBASE
        int Koeff();

        // ���������� TBase, ������� ����� ������������� ��� ���������� ���������� ��� ������� SET_TBASE
        TBase::E CompactTBase();
    }

    namespace Calibrator
    {
        // ������ ������� ����������.
        void RunCalibrate();
    }

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

    // ������� ������. ����� ��� ������ �������������, ��� ���������� ������� ����� �� �������� ���������������.
    void ClearData();

    // ���������� ���������� ���������, �� ������� ����� �������������� ������ � ������ �������������.
    void SetNumberMeasuresForGates(int number);

    // ������������� ��������� �������������.
    void SwitchingTrig();

    // ���������� ��������� ����� ����� ��������� ����� - ����� ��������� ������ ������������� �� ��������� �����
    void TemporaryPause();

    // ����� � ���������� ������� ������������� �� ���������� ���������� �������
    void FindAndSetTrigLevel();
}


namespace BUS_FPGA
{
    // ������ � ������� ���� ������ ��������. restart - true ��������, ��� ����� ������ ����� �������� ����� ���������,
    // ���� �� ����� ������ �� ��������� � ������ �����.
    void Write(uint16 *address, uint16 value, bool restart);

    void WriteAnalog(TypeWriteAnalog::E type, uint data);

    void WriteDAC(TypeWriteDAC::E type, uint16 data);
}
