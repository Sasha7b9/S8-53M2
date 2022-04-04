// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Panel/Panel.h"
#include "SCPI/SCPI.h"


bool SCPI::CONTROL::KEY(pchar buffer)
{
    static const MapElement keys[] =
    {
        {" ",          0},
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
        {" ", },
        {" ", },
        {" ", },


        {" MENU",     17},
        {" CURS",     -},
        {" MEAS",     -},
        {" DISPL",    -},
        {" MEM",      -},
        {" SERVICE",  -},
        {" SERV",     -},
        {" CHAN1",    -},
        {" CHAN2",    -},
        {" TRIG",     -},
        {0}
    };

    Word command;
    Word parameter;

    if (command.GetWord(buffer, 0) && parameter.GetWord(buffer, 1))
    {
        int numKey = 0;
        char *name = keys[numKey].key;
        while (name != 0)
        {
            if (command.WordEqualZeroString(name))
            {
                uint16 code = keys[numKey].value;
                if (parameter.WordEqualZeroString("DOWN"))
                {
                    code += 128;
                }

//              Panel::ProcessingCommandFromPIC(code);

                return true;
            }
            numKey++;
            name = keys[numKey].key;
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
