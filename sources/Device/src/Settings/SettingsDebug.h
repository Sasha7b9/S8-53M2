// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define NUM_STRINGS                         (set.debug.numStrings)
#define SIZE_FONT_CONSOLE                   (set.debug.sizeFont)
#define NUM_MEAS_FOR_GATES                  (set.debug.numMeasuresForGates)
#define ADD_SHIFT_T0                        (set.debug.shiftT0)
#define SHOW_STATS                          (set.debug.showStats)
#define NUM_AVE_FOR_RAND                    (set.debug.numAveForRand)


// Отладочные настройки.
struct SettingsDebug
{
    int16           numStrings;                     // Число строк в консоли.
    int8            sizeFont;                       // Размер шрифта консоли - 0 - 5, 1 - 8,
    int16           numMeasuresForGates;            // Число измерений для ворот.
    int             shiftT0;                        // Дополнительное смещение по времени для данной развёртки режима рандомизатора.
    bool            showStats;                      // Показывать статистику на экране (fps, например).
    int16           numAveForRand;                  // По скольким измерениям усреднять сигнал в режиме рандомизатора.
    bool            viewAlteraWrittingData;         // Показывать ли данные, идущие в альтеру.
    bool            viewAllAlteraWrittingData;      // Показывать ли все данные, идущие в альтеру (если false, то постоянно идущие команды вроде 
                                                    // START, STOP не показываются).
};
