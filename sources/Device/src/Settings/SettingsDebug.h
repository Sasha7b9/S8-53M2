// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define NUM_STRINGS                         (set.debug.numStrings)
#define SIZE_FONT_CONSOLE                   (set.debug.sizeFont)
#define NUM_MEAS_FOR_GATES                  (set.debug.numMeasuresForGates)
#define SHOW_STATS                          (set.debug.showStats)
#define NUM_AVE_FOR_RAND                    (set.debug.numAveForRand)


// ���������� ���������.
struct SettingsDebug
{
    int16           numStrings;                     // ����� ����� � �������.
    int8            sizeFont;                       // ������ ������ ������� - 0 - 5, 1 - 8,
    int16           numMeasuresForGates;            // ����� ��������� ��� �����.
    bool            showStats;                      // ���������� ���������� �� ������ (fps, ��������).
    int16           numAveForRand;                  // �� �������� ���������� ��������� ������ � ������ �������������.
    bool            viewAlteraWrittingData;         // ���������� �� ������, ������ � �������.
};
