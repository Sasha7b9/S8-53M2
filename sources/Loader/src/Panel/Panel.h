// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "common/Panel/Controls_.h"


namespace Panel
{
    uint16 NextData();
    Key::E PressedButton();

    void CallbackOnReceiveSPI5(uint8 *data, uint size);
};
