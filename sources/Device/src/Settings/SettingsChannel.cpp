// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Settings.h"
#include "SettingsChannel.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"


// Структура для описания диапазона масштаба по напряжению.
struct RangeStruct
{
    pchar name[2][2];     // Название диапазона в текстовом виде, пригодном для вывода на экран.
};


// Массив структур описаний масштабов по напряжению.
static const RangeStruct ranges[Range::Count] =
{
    {{{"2\x10мВ",     "20\x10мВ"}, {"2\x10mV",  "20\x10mV"}}},
    {{{"5\x10мВ",     "50\x10мВ"}, {"5\x10mV",  "50\x10mV"}}},
    {{{"10\x10мВ",    "0.1\x10В"}, {"10\x10mV", "0.1\x10V"}}},
    {{{"20\x10мВ",    "0.2\x10В"}, {"20\x10mV", "0.2\x10V"}}},
    {{{"50\x10мВ",    "0.5\x10В"}, {"50\x10mV", "0.5\x10V"}}},
    {{{"0.1\x10В",    "1\x10В"},   {"0.1\x10V", "1\x10V"}}},
    {{{"0.2\x10В",    "2\x10В"},   {"0.2\x10V", "2\x10V"}}},
    {{{"0.5\x10В",    "5\x10В"},   {"0.5\x10V", "5\x10V"}}},
    {{{"1\x10В",      "10\x10В"},  {"1\x10V",   "10\x10V"}}},
    {{{"2\x10В",      "20\x10В"},  {"2\x10V",   "20\x10V"}}},
    {{{"5\x10В",      "50\x10В"},  {"5\x10V",   "50\x10V"}}},
    {{{"10\x10В",     "100\x10В"}, {"10\x10V",  "100\x10V"}}},
    {{{"20\x10В",     "200\x10В"}, {"20\x10V",  "200\x10V"}}}
};



int sChannel_MultiplierRel2Abs(Divider::E multiplier)
{
    switch (multiplier)
    {
    case Divider::_10:
        return 10;

    case Divider::_1:
        return 1;
    }

    return 1;
}


bool sChannel_Enabled(Chan::E ch)
{
    if (ch == Chan::Math && !DISABLED_DRAW_MATH)
    {
        return true;
    }
    return SET_ENABLED(ch);
}


pchar  sChannel_Range2String(Range::E range, Divider::E multiplier)
{
    return ranges[range].name[set.common.lang][multiplier];
}


pchar  sChannel_RShift2String(int16 rShiftRel, Range::E range, Divider::E multiplier, char buffer[20])
{
    float rShiftVal = RSHIFT_2_ABS(rShiftRel, range) * sChannel_MultiplierRel2Abs(multiplier);
    return Voltage2String(rShiftVal, true, buffer);
};
