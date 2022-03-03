// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define IS_SHOW_REGISTERS_ALL   (set.debug.showRegisters.all)
#define IS_SHOW_REG_RSHIFT_A    (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.rShiftA)
#define IS_SHOW_REG_RSHIFT_B    (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.rShiftB)
#define IS_SHOW_REG_TRIGLEV     (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.trigLev)
#define IS_SHOW_REG_RANGE_A     (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.range[Chan::A])
#define IS_SHOW_REG_RANGE_B     (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.range[Chan::B])
#define IS_SHOW_REG_TRIGPARAM   (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.trigParam)
#define IS_SHOW_REG_PARAM_A     (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.chanParam[Chan::A])
#define IS_SHOW_REG_PARAM_B     (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.chanParam[Chan::B])
#define IS_SHOW_REG_TSHIFT      (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.tShift)
#define IS_SHOW_REG_TBASE       (IS_SHOW_REGISTERS_ALL || set.debug.showRegisters.tBase)


#define NUM_STRINGS                         (set.debug.numStrings)

#define SIZE_FONT_CONSOLE                   (set.debug.sizeFont)

#define CONSOLE_IN_PAUSE                    (set.debug.consoleInPause)

#define NUM_MEAS_FOR_GATES                  (set.debug.numMeasuresForGates)

#define ADD_SHIFT_T0                        (set.debug.shiftT0)

#define SHOW_STATS                          (set.debug.showStats)

#define NUM_AVE_FOR_RAND                    (set.debug.numAveForRand)


struct OutputRegisters
{
    bool    all;            // Показывать значения всех регистров.
    bool    flag;           // Выводить ли флаг готовности.
    bool    rShiftA;
    bool    rShiftB;
    bool    trigLev;
    bool    range[2];
    bool    chanParam[2];
    bool    trigParam;
    bool    tShift;
    bool    tBase;
};


// Отладочные настройки.
struct SettingsDebug
{
    int16           numStrings;                     // Число строк в консоли.
    int8            sizeFont;                       // Размер шрифта консоли - 0 - 5, 1 - 8,
    bool            consoleInPause;                 // Признак того, что консоль находится в режиме паузы. Режим паузы означает, что новые 
                                                    // сообщения она не записывает и не сохраняет.
    int16           numMeasuresForGates;            // Число измерений для ворот.
    int             shiftT0;                        // Дополнительное смещение по времени для данной развёртки режима рандомизатора.
    bool            showStats;                      // Показывать статистику на экране (fps, например).
    int16           numAveForRand;                  // По скольким измерениям усреднять сигнал в режиме рандомизатора.
    bool            viewAlteraWrittingData;         // Показывать ли данные, идущие в альтеру.
    bool            viewAllAlteraWrittingData;      // Показывать ли все данные, идущие в альтеру (если false, то постоянно идущие команды вроде 
                                                    // START, STOP не показываются).
    OutputRegisters showRegisters;
};
