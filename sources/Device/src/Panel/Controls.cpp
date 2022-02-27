// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Controls.h"


pchar NameButton(Key::E button)
{
    static const pchar namesButtons[] =
    {
        "Key::Empty",
        "Key::ChannelA",
        "Key::Service",
        "Key::ChannelB",
        "Key::Display",
        "Key::Time",
        "Key::Memory",
        "B_Sinchro",
        "Key::Start",
        "Key::Cursors",
        "Key::Measures",
        "Key::Power",
        "B_Info",
        "Key::Menu",
        "Key::F1",
        "Key::F2",
        "Key::F3",
        "Key::F4",
        "Key::F5"
    };

    return namesButtons[button];
}


Key::E& operator++(Key::E &button)
{
    button = (Key::E)((int)button + 1);
    return button;
}
