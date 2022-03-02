// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsTypes.h"
#include "FPGA/SettingsFPGA.h"


#define SET_BALANCE_ADC(ch)     (set.chan[ch].balanceADC)
#define SET_BALANCE_ADC_A       (SET_BALANCE_ADC(ChA))
#define SET_BALANCE_ADC_B       (SET_BALANCE_ADC(ChB))

#define SET_STRETCH_ADC(ch)     (set.chan[ch].stretchADC)
#define SET_STRETCH_ADC_A       (SET_STRETCH_ADC(ChA))
#define SET_STRETCH_ADC_B       (SET_STRETCH_ADC(ChB))

#define SET_SHIFT_ADC(ch)       (set.chan[ch].shiftADC)
#define SET_SHIFT_ADC_A         (SET_SHIFT_ADC(ChA))
#define SET_SHIFT_ADC_B         (SET_SHIFT_ADC(ChB))


#define SET_INVERSE(ch)         (set.chan[ch].inverse)          // SettingsChannel.inverse
#define SET_INVERSE_A           (SET_INVERSE(Chan::A))          // set.chan[Chan::A].inverse
#define SET_INVERSE_B           (SET_INVERSE(Chan::B))          // set.chan[Chan::B].inverse
#define SET_RSHIFT_MATH         (set.math.rShift)               // SettingsMath.rShift
#define SET_RANGE_MATH          (set.math.range)                // SettingsMath.range

#define SET_RANGE(ch)           (set.chan[ch].range)            // SettingsChannel.range
#define SET_RANGE_A             (SET_RANGE(Chan::A))
#define SET_RANGE_B             (SET_RANGE(Chan::B))

#define SET_RSHIFT(ch)          (set.chan[ch].rShiftRel)        // SettingsChannel.rShiftRel
#define SET_RSHIFT_A            (SET_RSHIFT(Chan::A))
#define SET_RSHIFT_B            (SET_RSHIFT(Chan::B))

#define SET_COUPLE(ch)          (set.chan[ch].modeCouple)       // SettingsChannel.modeCouple
#define SET_COUPLE_A            (SET_COUPLE(Chan::A))
#define SET_COUPLE_B            (SET_COUPLE(Chan::B))

#define SET_DIVIDER(ch)         (set.chan[ch].divider)          // SettingsChannel.divider
#define SET_DIVIDER_A           SET_DIVIDER(Chan::A)
#define SET_DIVIDER_B           SET_DIVIDER(Chan::B)
#define SET_DIVIDER_ABS(ch)     (Divider::ToAbs(SET_DIVIDER(ch)))

#define SET_ENABLED(ch)         (set.chan[ch].enable)           // SettingsChannel.enable
#define SET_ENABLED_A           SET_ENABLED(Chan::A)            // set.chan[Chan::A].enable
#define SET_ENABLED_B           SET_ENABLED(Chan::B)            // set.chan[Chan::B].enable

#define SET_FILTR(ch)           (set.chan[ch].filtr)            // SettingsChannel.filtr
#define SET_FILTR_A             (SET_FILTR(Chan::A))            // set.chan[Chan::A].filtr
#define SET_FILTR_B             (SET_FILTR(Chan::B))            // set.chan[Chan::B].filtr
