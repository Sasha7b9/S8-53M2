// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Controls_Old.h"


pchar Key::Name(Key::E button)
{
    static const pchar names[] =
    {
        "Key::Empty",
        "Key::ChannelA",
        "Key::Service",
        "Key::ChannelB",
        "Key::Display",
        "Key::Time",
        "Key::Memory",
        "Key::Trig",
        "Key::Start",
        "Key::Cursors",
        "Key::Measures",
        "Key::Power",
        "Key::Help",
        "Key::Menu",
        "Key::F1",
        "Key::F2",
        "Key::F3",
        "Key::F4",
        "Key::F5"
    };

    return names[button];
}


Key::E& operator++(Key::E &button)
{
    button = (Key::E)((int)button + 1);
    return button;
}
