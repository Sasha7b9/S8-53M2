// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define NUM_STRINGS             (set.debug.numStrings)
#define SIZE_FONT_CONSOLE       (set.debug.sizeFont)
#define NUM_MEAS_FOR_GATES      (set.debug.numMeasuresForGates)
#define SHOW_STATS              (set.debug.showStats)

#define SET_BALANCE_ADC(ch)     (set.debug.balanceADC[ch])
#define SET_BALANCE_ADC_A       (SET_BALANCE_ADC(ChA))
#define SET_BALANCE_ADC_B       (SET_BALANCE_ADC(ChB))

#define SET_FPGA_COMPACT        set.debug.fpga_compact
#define FPGA_GATES_MIN          set.debug.fpga_gates_min
#define FPGA_GATES_MAX          set.debug.fpga_gates_max

#define RSHIFT_HAND(ch, range, couple) set.debug.hand_rshift[ch][range][couple]

#define CAL_RSHIFT(ch)          (set.debug.cal_rshift[ch][SET_RANGE(ch)][SET_COUPLE(ch)])
#define CAL_RSHIFT_A            (CAL_RSHIFT(ChA))
#define CAL_RSHIFT_B            (CAL_RSHIFT(ChB))


// Несбрасываемые настройки настройки.
struct SettingsNRST
{
    int16   numStrings;             // Число строк в консоли.
    int8    sizeFont;               // Размер шрифта консоли - 0 - 5, 1 - 8,
    int16   numMeasuresForGates;    // Число измерений для ворот.
    bool    showStats;              // Показывать статистику на экране (fps, например).

    int16   numAveForRand;          // По скольким измерениям усреднять сигнал в режиме рандомизатора.
    bool    fpga_compact;           // Если true - уплотнять сигнал (FPGA::Compactor)
    int16   fpga_gates_min;         // \ Минимальные и максимальные значения, за пределами которых нужно отбрасывать
    int16   fpga_gates_max;         // /

    int16   first_byte;

    int16   balanceADC[Chan::Count];        // Баланс АЦП (смещение первой точки относительно второй) для обоих каналов
    int16   hand_rshift[Chan::Count][Range::Count][ModeCouple::Count];  // Вручную устанавливаемое дополнительное смещение
    int8    cal_rshift[Chan::Count][Range::Count][ModeCouple::Count];   // Добавочное смещение, вычисляемое при калибровке
};
