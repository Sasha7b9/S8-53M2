// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "HAL.h"
#include "Interface.h"
#include "Keyboard.h"
#include "PowerSupply.h"


int main()
{
    HAL::Init();

    Keyboard::Init();

    while (1)
    {
        Keyboard::Update();

        KeyboardEvent event = Keyboard::Buffer::GetNextEvent();

        if (!PowerSupply::AttemptToTurnOn(event))
        {
            Interface::Update(event);
        }
    }
}
