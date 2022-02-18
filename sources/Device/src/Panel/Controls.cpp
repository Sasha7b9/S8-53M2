// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Controls.h"


pchar  NameButton(PanelButton button) 
{
    static const pchar namesButtons[] =
    {
        "B_Empty",
        "B_ChannelA",
        "B_Service",
        "B_ChannelB",
        "B_Display",
        "B_Time",
        "B_Memory",
        "B_Sinchro",
        "B_Start",
        "B_Cursors",
        "B_Measures",
        "B_Power",
        "B_Info",
        "B_Menu",
        "B_F1",
        "B_F2",
        "B_F3",
        "B_F4",
        "B_F5"
    };

    return namesButtons[button];
}


PanelButton& operator++(PanelButton &button)
{
    button = (PanelButton)((int)button + 1);
    return button;
}
