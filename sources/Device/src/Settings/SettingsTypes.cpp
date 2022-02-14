// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "SettingsTypes.h"
#include "FPGA/SettingsFPGA.h"


pchar RangeName(Range::E range)
{
    static pchar names[Range::Count + 1] =
    {
        "2mV",
        "5mV",
        "10mV",
        "20mV",
        "50mV",
        "100mV",
        "200mV",
        "500mV",
        "1V",
        "2V",
        "5V",
        "10V",
        "20V",
        "Range::Count"
    };
    return names[range];
}


pchar TBaseName(TBase::E tBase)
{
    static pchar names[TBase::Count + 1] =
    {
        "2ns",
        "5ns",
        "10ns",
        "20ns",
        "50ns",
        "100ns",
        "200ns",
        "500ns",
        "1us",
        "2us",
        "5us",
        "10us",
        "20us",
        "50us",
        "100us",
        "200us",
        "500us",
        "1ms",
        "2ms",
        "5ms",
        "10ms",
        "20ms",
        "50ms",
        "100ms",
        "200ms",
        "500ms",
        "1s",
        "2s",
        "5s",
        "10s",
        "TBase::Count"
    };

    return names[tBase];
}
