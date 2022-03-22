#include "defines.h"
#include "Utils/PasswordResolver.h"
#include "Menu/Menu.h"
#include "Menu/Pages/Definition.h"


void PasswordResolver::ProcessEvent(KeyboardEvent &event)
{
    static Key::E keys[10] =
    {
        Key::F1, Key::F1,
        Key::F2, Key::F2,
        Key::F3, Key::F3,
        Key::F4, Key::F4,
        Key::F5, Key::F5
    };

    static int pointer = 0;

    if (!Menu::IsShown())
    {
        if (event.IsUp())
        {
            if (keys[pointer] == event.key)
            {
                pointer++;
            }
            else
            {
                pointer = 0;
            }

            if (pointer == 10)
            {
                PageMain::EnablePageDebug();
            }
        }
    }
}
