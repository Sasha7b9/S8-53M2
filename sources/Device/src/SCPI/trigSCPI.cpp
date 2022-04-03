// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Menu/Pages/Definition.h"
#include "SCPI/SCPI.h"
#include "Utils/Containers/Values.h"


/*
    TRIG:
        MODE {AUTO | WAIT | SINGLE | ?}
        SOURCE {CHAN1 | CHAN2 | EXT | ?}
        POLARITY {FRONT | BACK | ?}
        INPUT {FULL | AC | LPF | HPF | ?}
        OFFSET {-200...200 | ?}
*/


bool SCPI::ProcessTRIG(pchar buffer)
{
    static const StructCommand commands[] =
    {
        {":MODE",        SCPI::TRIGGER::MODE},
        {":SOURCE",      SCPI::TRIGGER::SOURCE},
        {":POLARITY",    SCPI::TRIGGER::POLARITY},
        {":POLAR",       SCPI::TRIGGER::POLARITY},
        {":INPUT",       SCPI::TRIGGER::INPUT},
        {":FIND",        SCPI::TRIGGER::FIND},
        {":OFFSET",      SCPI::TRIGGER::OFFSET},
        {0}
    };

    return SCPI::ProcessingCommand(commands, buffer);
}


bool SCPI::TRIGGER::MODE(pchar buffer)
{
    static const MapElement map[] =
    {
        {" AUTO",   0},
        {" WAIT",   1},
        {" SINGLE", 2},
        {0}
    };

    SCPI_CYCLE(START_MODE = (StartMode::E)it->value; PageTrig::OnPress_Mode(true));

    IF_REQUEST(SCPI::SendFormat(":TRIGGER:MODE%s", map[START_MODE].key));

    return false;
}


bool SCPI::TRIGGER::SOURCE(pchar buffer)
{
    static const MapElement map[] =
    {
        {" CHAN1", 0},
        {" CHAN2", 1},
        {" EXT",   2},
        {0}
    };

    SCPI_CYCLE(TrigSource::Set((TrigSource::E)it->value));

    IF_REQUEST(SCPI::SendFormat(":TRIGGER:SOUCRE%s", map[TRIG_SOURCE].key));

    return false;
}


bool SCPI::TRIGGER::POLARITY(pchar buffer)
{
    if      FIRST_SYMBOLS(" FRONT") { TrigPolarity::Set(TrigPolarity::Front); return true; }
    else if FIRST_SYMBOLS(" BACK")  { TrigPolarity::Set(TrigPolarity::Back);  return true; }

    IF_REQUEST(SCPI::SendFormat(":TRIGGER:POLARITY %s", TRIG_POLARITY_IS_FRONT ? "FRONT" : "BACK"));

    return false;
}


bool SCPI::TRIGGER::INPUT(pchar buffer)
{
    static const MapElement map[] =
    {
        {" FULL", 0},
        {" AC",   1},
        {" LPF",  2},
        {" HPF",  3},
        {0}
    };

    SCPI_CYCLE(TrigInput::Set((TrigInput::E)it->value));

    IF_REQUEST(SCPI::SendFormat(":TRIGGER:INPUT%s", map[TRIG_INPUT].key));

    return false;
}


bool SCPI::TRIGGER::FIND(pchar buffer)
{
    if      FIRST_SYMBOLS(" HAND") { TRIG_MODE_FIND = TrigModeFind::Hand; return true; }
    else if FIRST_SYMBOLS(" AUTO") { TRIG_MODE_FIND = TrigModeFind::Auto; return true; }
    else if FIRST_SYMBOLS(" FIND") { TrigLev::FindAndSet();               return true; }

    IF_REQUEST(SCPI::SendFormat(":TRIGGER:FIND %s", TRIG_MODE_FIND_IS_HAND ? "HAND" : "AUTO"));

    return false;
}


bool SCPI::TRIGGER::OFFSET(pchar buffer)
{
    if FIRST_SYMBOLS("?")
    {
        int trig_lev = (int)(0.5F * (TRIG_LEVEL_SOURCE - RShift::ZERO));
        SCPI::SendFormat(":TRIGGER:OFFSET %d", trig_lev);
        return true;
    }
    else
    {
        Int value(buffer);

        if (value.IsValid() && value >= -240 && value <= 240)
        {
            int trig_lev = RShift::ZERO + 2 * value;
            TrigLev::Set(TRIG_SOURCE, (int16)trig_lev);
            return true;
        }
    }

    return false;
}
