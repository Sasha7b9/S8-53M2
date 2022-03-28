// 2022/02/11 17:48:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"
#include "FPGA/SettingsFPGA.h"
#include "FPGA/TypesFPGA.h"
#include "FPGA/StructuresFPGA.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"


namespace FPGA
{
    static const int MAX_POINTS = 1024;
    static const int MAX_BYTES = MAX_POINTS * 2;

    extern StateFPGA state;

    namespace FreqMeter
    {
        // �������� �������� �������.
        float GetFreq();

        void Update(uint16);
    }

    namespace AutoFinder
    {
        // ���� ������. ���� ������ ������, �� �� ���������� ������ ����� �� ������
        void FindSignal();
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

    struct Flag
    {
        uint16 value;

        uint16 Read()
        {
            value = HAL_FMC::Read(RD_FL);
            FreqMeter::Update(value);
            return value;
        }

        bool FirstByte() { return _GET_BIT(value, FL_LAST_RECOR) == 1; }
    };

    extern Flag flag;

    void Init();

    // ���������� ���������� ����������� �������� � �������.
    void SetNumSignalsInSec(int numSigInSec);

    void Update();

    // ��������� ���� ����� ����������.
    void OnPressStartStop();

    // ������ �������� ����� ����������.
    void Start();

    // ��������� ������� ����� ����������.
    void Stop();

    // ���������� true, ���� ������ ��������� �� � �������� ����� ����������.
    bool IsRunning();

    // ���������� ���������� ���������, �� ������� ����� �������������� ������ � ������ �������������.
    void SetNumberMeasuresForGates(int number);

    // ������������� ��������� �������������.
    void SwitchingTrig();

    // ���������� ��������� ����� ����� ��������� ����� - ����� ��������� ������ ������������� �� ��������� �����
    void TemporaryPause();

    // ����� � ���������� ������� ������������� �� ���������� ���������� �������
    void FindAndSetTrigLevel();

    // ��������� ������� WR_UPR (������� �������� � ���������� � ���������).
    void LoadRegUPR();
}


namespace BUS_FPGA
{
    // ������ � ������� ���� ������ ��������. restart - true ��������, ��� ����� ������ ����� �������� ����� ���������,
    // ���� �� ����� ������ �� ��������� � ������ �����.
    void Write(uint16 *, uint16, bool restart);

    void WriteAnalog(TypeWriteAnalog::E, uint);

    void WriteDAC(TypeWriteDAC::E, uint16);
}
