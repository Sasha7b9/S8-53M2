// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsTypes.h"


#define NUM_STRINGS             (nrst.numStrings)
#define SIZE_FONT_CONSOLE       (nrst.sizeFont)
#define NUM_MEAS_FOR_GATES      (nrst.numMeasuresForGates)
#define SHOW_STATS              (nrst.showStats)

#define SET_BALANCE_ADC(ch)     (nrst.balanceADC[ch])
#define SET_BALANCE_ADC_A       (SET_BALANCE_ADC(ChA))
#define SET_BALANCE_ADC_B       (SET_BALANCE_ADC(ChB))

#define SET_FPGA_COMPACT        nrst.fpga_compact
#define FPGA_GATES_MIN          nrst.fpga_gates_min
#define FPGA_GATES_MAX          nrst.fpga_gates_max

#define RSHIFT_HAND(ch, range)  nrst.hand_rshift[ch][range]


// �������������� ��������� ���������.
struct SettingsNRST
{
    static const int SIZE_FIELD_RECORD = 64;        // ������ ���� ��� ������ ��������. ���� ������ ������ ���� ������ ������� ���������.
                                                    // ����� ���������� ������ �������� �� ����� ��������

    uint    crc32;                  // ����������� �����. �������������� �� ������� �����, ���������� �� ��������� ������ crc32
    int16   numStrings;             // ����� ����� � �������.
    int8    sizeFont;               // ������ ������ ������� - 0 - 5, 1 - 8,
    int16   numMeasuresForGates;    // ����� ��������� ��� �����.
    bool    showStats;              // ���������� ���������� �� ������ (fps, ��������).

    int16   numAveForRand;          // �� �������� ���������� ��������� ������ � ������ �������������.
    bool    fpga_compact;           // ���� true - ��������� ������ (FPGA::Compactor)
    int16   fpga_gates_min;         // \ ����������� � ������������ ��������, �� ��������� ������� ����� �����������
    int16   fpga_gates_max;         // /

    int16   first_byte;

    int16   balanceADC[Chan::Count];        // ������ ��� (�������� ������ ����� ������������ ������) ��� ����� �������
    int16   hand_rshift[Chan::Count][3];    // ������� ��������������� �������������� ��������. ��� ��������� ����� � 2��, 5��, 10��
    uint    not_used;                       // ��� ���� ������ ����� ����. ��� ����������� �������� ����������� �����

    static void Load();
    static void Save();

    uint CalculateCRC32();
};


extern SettingsNRST nrst;
