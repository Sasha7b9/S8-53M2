// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "FPGA/FPGA.h"
#include "Hardware/LAN/LAN.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Containers/Values.h"
#include "SCPI/SCPI.h"


static Chan ch(ChA);


bool SCPI::ProcessCHANNEL(pchar buffer)
{
    static const StructCommand commands[] =
    {
        {":INPUT",      SCPI::CHANNEL::INPUT},
        {":COUPLING",   SCPI::CHANNEL::COUPLE},
        {":COUPL",      SCPI::CHANNEL::COUPLE},
        {":FILTR",      SCPI::CHANNEL::FILTR_},
        {":INVERSE",    SCPI::CHANNEL::INVERSE},
        {":INV",        SCPI::CHANNEL::INVERSE},
        {":RANGE",      SCPI::CHANNEL::RANGE_},
        {":OFFSET",     SCPI::CHANNEL::OFFSET},
        {":FACTOR",     SCPI::CHANNEL::FACTOR},
        {":FACT",       SCPI::CHANNEL::FACTOR},
        {0}
    };

    ch = (char)(*(buffer - 2)) == '1' ? ChA : ChB;

    return SCPI::ProcessingCommand(commands, (pchar)(buffer));
}


bool SCPI::CHANNEL::INPUT(pchar buffer)
{
    if      FIRST_SYMBOLS(" ON")  { SET_ENABLED(ch) = true;  return true; }
    else if FIRST_SYMBOLS(" OFF") { SET_ENABLED(ch) = false; return true; }

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:INPUT %s", ch.ToNumber(), ch.Enabled() ? "ON" : "OFF"));

    return false;
}


bool SCPI::CHANNEL::COUPLE(pchar buffer)
{
    if      FIRST_SYMBOLS(" DC")  { ModeCouple::Set(ch, ModeCouple::DC);  return true; }
    else if FIRST_SYMBOLS(" AC")  { ModeCouple::Set(ch, ModeCouple::AC);  return true; }
    else if FIRST_SYMBOLS(" GND") { ModeCouple::Set(ch, ModeCouple::GND); return true; }

    if FIRST_SYMBOLS("?")
    {
        static pchar modes[3] = { " DC", " AC", " GND" };
        SCPI::SendFormat(":CHANNEL%d:COUPLING %s", ch.ToNumber(), modes[SET_COUPLE(ch)]);
        return true;
    }

    return false;
}


bool SCPI::CHANNEL::FILTR_(pchar buffer)
{
    static const pFuncVB func[2] = {PageChannelA::OnChanged_Filtr, PageChannelB::OnChanged_Filtr};

    if      FIRST_SYMBOLS(" ON")  { SET_FILTR(ch) = true; func[ch](true);  return true; }
    else if FIRST_SYMBOLS(" OFF") { SET_FILTR(ch) = false; func[ch](true); return true; }

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:FILTR %s", ch.ToNumber(), SET_FILTR(ch) ? "ON" : "OFF"));

    return false;
}


bool SCPI::CHANNEL::INVERSE(pchar buffer)
{
    if      FIRST_SYMBOLS(" ON")  { SET_INVERSE(ch) = true;  return true; }
    else if FIRST_SYMBOLS(" OFF") { SET_INVERSE(ch) = false; return true; }

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:SET_INVERSE %s", ch.ToNumber(), SET_INVERSE(ch) ? "ON" : "OFF"));

    return false;
}


bool SCPI::CHANNEL::RANGE_(pchar buffer)
{
    static const MapElement map[] = 
    {
        {" 2MV",   (uint8)Range::_2mV},
        {" 5MV",   (uint8)Range::_5mV},
        {" 10MV",  (uint8)Range::_10mV},
        {" 20MV",  (uint8)Range::_20mV},
        {" 50MV",  (uint8)Range::_50mV},
        {" 100MV", (uint8)Range::_100mV},
        {" 200MV", (uint8)Range::_200mV},
        {" 500MV", (uint8)Range::_500mV},
        {" 1V",    (uint8)Range::_1V},
        {" 2V",    (uint8)Range::_2V},
        {" 5V",    (uint8)Range::_5V},
        {" 10V",   (uint8)Range::_10V},
        {" 20V",   (uint8)Range::_20V},
        {0}
    };

    SCPI_CYCLE(Range::Set(ch, (Range::E)it->value));

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:SET_RANGE %s", ch.ToNumber(), map[SET_RANGE(ch)].key));

    return false;
}


bool SCPI::CHANNEL::OFFSET(pchar buffer)
{
    if FIRST_SYMBOLS("?")
    {
        int offset = (int)(0.5F * (SET_RSHIFT(ch) - RShift::ZERO));
        SCPI::SendFormat(":CHANNNEL%d:OFFSET %d", ch.ToNumber(), offset);
        return true;
    }
    else
    {
        Int value(buffer);

        if (value.IsValid() && value >= -240 && value <= 240)
        {
            int rShift = RShift::ZERO + 2 * value;
            RShift::Set(ch, (int16)rShift);
            return true;
        }
    }

    return false;
}


bool SCPI::CHANNEL::FACTOR(pchar buffer)
{
    if      FIRST_SYMBOLS(" X1")  { SET_DIVIDER(ch) = Divider::_1;  return true; }
    else if FIRST_SYMBOLS(" X10") { SET_DIVIDER(ch) = Divider::_10; return true; }

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:PROBE %s", ch.ToNumber(), SET_DIVIDER(ch) == Divider::_1 ? "X1" : "X10"));

    return false;
}
