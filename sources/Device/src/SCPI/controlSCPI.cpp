// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Panel.h"
#include "SCPI/SCPI.h"


bool SCPI::CONTROL::KEY(pchar buffer)
{
    static const MapElement keys[] =
    {
        {" 1",         1},
        {" 2",         2},
        {" 3",         3},
        {" 4",         4},
        {" 5",         5},
        {" CHANNEL1",  6},
        {" CHANNEL2",  7},
        {" TIME",      8},
        {" TRIG",      9},
        {" CURSORS",  10},
        {" MEASURES", 11},
        {" DISPLAY",  12},
        {" HELP",     13},
        {" START",    14},
        {" MEMORY",   15},
        {" SERVICE",  16},
        {" MENU",     17},
        {" POWER",    18},
        {0}
    };

    Word button;        // Здесь хранится принятое название кнопки
    Word action;        // Здесь хранится принятое действие кнопки

    if (button.GetWord(buffer, 0) && action.GetWord(buffer, 1))
    {
        int numKey = 0;
        while (keys[numKey].key != 0)
        {
            if (button.WordEqualZeroString(keys[numKey].key))
            {
                uint8 data[3] = { 255, keys[numKey].value, 0 };

                if (action.WordEqualZeroString("DOWN"))
                {
                    data[2] = Action::Down;
                }
                else if (action.WordEqualZeroString("UP"))
                {
                    data[2] = Action::Up;
                }
                else if (action.WordEqualZeroString("LONG"))
                {
                    data[2] = Action::Long;
                }

                if (data[2])
                {
                    Panel::Callback::OnReceiveSPI5(data, 3);
                    return true;
                }
            }

            numKey++;
        }
    }

    return false;
}


bool SCPI::CONTROL::GOVERNOR(pchar buffer)
{
    static const MapElement governors[] =
    {
        {" RSHIFT1", 21},
        {" RSHIFT2", 23},
        {" RANGE1",  20},
        {" RANGE2",  22},
        {" SET",     27},
        {" TSHIFT",  25},
        {" TBASE",   24},
        {" TRIGLEV", 26},
        {0}
    };

    Word command;
    Word parameter;

    if (command.GetWord(buffer, 0) && parameter.GetWord(buffer, 1))
    {
        int numGov = 0;
        char *name = governors[numGov].key;
        while (name != 0) {
            if (command.WordEqualZeroString(name))
            {
                uint16 code = governors[numGov].value;
                if (parameter.WordEqualZeroString("RIGHT"))
                {
                    code += 128;
                }

//                Panel::ProcessingCommandFromPIC(code);
                return true;
            }
            numGov++;
            name = governors[numGov].key;
        }
    }

    return false;
}
