// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsTypes.h"
#include "FPGA/SettingsFPGA.h"


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
#define SET_DIVIDER_1(ch)       (SET_DIVIDER(ch) == Divider_1)
#define SET_DIVIDER_10(ch)      (SET_DIVIDER(ch) == Divider_10)
#define VALUE_MULTIPLIER(ch)    (Divider::ToAbs(SET_DIVIDER(ch)))

#define SET_ENABLED(ch)         (set.chan[ch].enable)           // SettingsChannel.enable
#define SET_ENABLED_A           SET_ENABLED(Chan::A)            // set.chan[Chan::A].enable
#define SET_ENABLED_B           SET_ENABLED(Chan::B)            // set.chan[Chan::B].enable

#define SET_BALANCE_ADC(ch)     (set.chan[ch].balanceShiftADC)  // SettingsChannel.balanceShiftADC
#define SET_BALANCE_ADC_A       (SET_BALANCE_ADC(Chan::A))      // set.chan[Chan::A].balanceShiftADC
#define SET_BALANCE_ADC_B       (SET_BALANCE_ADC(Chan::B))      // set.chan[Chan::B].balanceShiftADC

#define STRETCH_ADC(ch)         (set.chan[ch].stretchADC)       // SettingsChannel.stretchADC
#define STRETCH_ADC_A           (STRETCH_ADC(Chan::A))          // set.chan[Chan::A].stretchADC
#define STRETCH_ADC_B           (STRETCH_ADC(Chan::B))          // set.chan[Chan::B].stretchADC

#define SET_FILTR(ch)           (set.chan[ch].filtr)            // SettingsChannel.filtr
#define SET_FILTR_A             (SET_FILTR(Chan::A))            // set.chan[Chan::A].filtr
#define SET_FILTR_B             (SET_FILTR(Chan::B))            // set.chan[Chan::B].filtr



pchar sChannel_Range2String(Range::E, Divider::E);

pchar sChannel_RShift2String(int16 rShiftRel, Range::E, Divider::E, char buffer[20]);
