// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Controls_Old.h"


pchar KeyOld::Name(KeyOld::E button)
{
    static const pchar names[] =
    {
        "KeyOld::Empty",
        "KeyOld::ChannelA",
        "KeyOld::Service",
        "KeyOld::ChannelB",
        "KeyOld::Display",
        "KeyOld::Time",
        "KeyOld::Memory",
        "KeyOld::Trig",
        "KeyOld::Start",
        "KeyOld::Cursors",
        "KeyOld::Measures",
        "KeyOld::Power",
        "KeyOld::Help",
        "KeyOld::Menu",
        "KeyOld::F1",
        "KeyOld::F2",
        "KeyOld::F3",
        "KeyOld::F4",
        "KeyOld::F5"
    };

    return names[button];
}


KeyOld::E& operator++(KeyOld::E &button)
{
    button = (KeyOld::E)((int)button + 1);
    return button;
}
