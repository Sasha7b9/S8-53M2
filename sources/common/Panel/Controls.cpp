// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "common/Panel/Controls.h"


bool KeyboardEvent::IsFunctional() const
{
    return (key >= Key::F1) && (key <= Key::F4);
}


bool KeyboardEvent::IsRelease() const
{
    return (action == Action::Up) || (action == Action::Long);
}


pstring Key::Name() const
{
    static pstring names[Key::Count] =
    {
        "None",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "ÊÀÍÀË 1",
        "ÊÀÍÀË 2",
        "ÐÀÇÂ",
        "ÑÈÍÕÐ",
        "ÊÓÐÑÎÐÛ",
        "ÈÇÌÅÐ",
        "ÄÈÑÏËÅÉ",
        "ÏÎÌÎÙÜ",
        "ÏÓÑÊ/ÑÒÎÏ",
        "ÏÀÌßÒÜ",
        "ÑÅÐÂÈÑ",
        "ÌÅÍÞ",
        "ÏÈÒÀÍÈÅ",

        "ÂÎËÜÒ/ÄÅË 1",
        "ÂÎËÜÒ/ÄÅË 2",
        "RSHIFT 1",
        "RSHIFT 2",
        "ÂÐÅÌß/ÄÅË",
        "TSHIFT",
        "ÓÐÎÂÍÜ",
        "ÓÑÒÀÍÎÂÊÀ"
    };

    return names[value];
}


Key::E &operator++(Key::E &right)
{
    right = (Key::E)((int)right + 1);
    return right;
}


Key::E Key::FromCode(uint16 code)
{
    return (Key::E)(code & 0x1F);
}


uint16 Key::ToCode(Key::E key)
{
    return (uint16)(key);
}


bool Key::IsGovernor(Key::E key)
{
    return (key >= Key::RangeA) && (key <= Key::Setting);
}


bool Key::Is(Key::E key)
{
    return (key < Key::RangeA) && (key != Key::None);
}


Action::E Action::FromCode(uint16 code)
{
    return (Action::E)((code >> 5) & 0x7);
}


uint16 Action::ToCode(Action::E action)
{
    return (uint16)(action << 5);
}


bool Key::IsFunctional(Key::E key)
{
    return (key >= Key::F1) && (key <= Key::F5);
}

pchar Action::Name() const
{
    static const pchar names[Count] =
    {
        "Down",
        "Up",
        "Long",
        "Rotate_Left",
        "Rotate_Right"
    };

    return names[value];
}


KeyboardEvent::KeyboardEvent(uint8 *buffer)
{
    if (*buffer == 0xFF)
    {
        key = (Key::E) *(buffer + 1);
        action = (Action::E) *(buffer + 2);
    }
    else
    {
        key = Key::None;
        action = Action::Count;
    }
}


void KeyboardEvent::Log() const
{
    if (key != Key::None)
    {

    }
}
