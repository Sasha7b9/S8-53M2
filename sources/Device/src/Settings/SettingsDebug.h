// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define NUM_STRINGS             (set.debug.numStrings)
#define SIZE_FONT_CONSOLE       (set.debug.sizeFont)
#define NUM_MEAS_FOR_GATES      (set.debug.numMeasuresForGates)
#define SHOW_STATS              (set.debug.showStats)
#define NUM_AVE_FOR_RAND        (set.debug.numAveForRand)

#define SET_BALANCE_ADC(ch)     (set.debug.balanceADC[ch])
#define SET_BALANCE_ADC_A       (SET_BALANCE_ADC(ChA))
#define SET_BALANCE_ADC_B       (SET_BALANCE_ADC(ChB))


// Несбрасываемые настройки настройки.
struct SettingsNRST
{
    int16   numStrings;             // Число строк в консоли.
    int8    sizeFont;               // Размер шрифта консоли - 0 - 5, 1 - 8,
    int16   numMeasuresForGates;    // Число измерений для ворот.
    bool    showStats;              // Показывать статистику на экране (fps, например).
    int16   numAveForRand;          // По скольким измерениям усреднять сигнал в режиме рандомизатора.
    int8    balanceADC[2];          // Баланс АЦП (смещение первой точки относительно второй) для обоих каналов
};
