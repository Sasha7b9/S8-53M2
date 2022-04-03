// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP_.h"
#include "FPGA/FPGA.h"
#include "Hardware/LAN/LAN.h"
#include "Menu/Pages/Definition.h"
#include "SCPI/SCPI.h"


static Chan &ch = ChA;


pchar SCPI::ProcessCHANNEL(pchar buffer)
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


pchar SCPI::CHANNEL::INPUT(pchar buffer)
{
    if      FIRST_SYMBOLS(" ON")  { ENABLED(ch) = true; }
    else if FIRST_SYMBOLS(" OFF") { ENABLED(ch) = false; }

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:INPUT %s", ch.ToNumber(), ch.IsEnabled() ? "ON" : "OFF"));

    return buffer;
}


pchar SCPI::CHANNEL::COUPLE(pchar buffer)
{
    if      FIRST_SYMBOLS(" DC")  { ModeCouple::Set(ch, ModeCouple::DC); }
    else if FIRST_SYMBOLS(" AC")  { ModeCouple::Set(ch, ModeCouple::AC); }
    else if FIRST_SYMBOLS(" GND") { ModeCouple::Set(ch, ModeCouple::GND); }

    if FIRST_SYMBOLS("?")
    {
        static pchar modes[3] = { " DC", " AC", " GND" };
        SCPI::SendFormat(":CHANNEL%d:COUPLING %s", ch.ToNumber(), modes[MODE_COUPLE(ch)]);
    }

    return buffer;
}


pchar SCPI::CHANNEL::FILTR_(pchar buffer)
{
    static const pFuncVB func[2] = {PageChannel::A::OnChanged_Filtr, PageChannel::B::OnChanged_Filtr};

    if      FIRST_SYMBOLS(" ON")  { FILTR(ch) = true; func[ch](true); }
    else if FIRST_SYMBOLS(" OFF") { FILTR(ch) = false; func[ch](true); }

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:FILTR %s", ch.ToNumber(), FILTR(ch) ? "ON" : "OFF"));

    return buffer;
}


pchar SCPI::CHANNEL::INVERSE(pchar buffer)
{
    if      FIRST_SYMBOLS(" ON")  { INVERSED(ch) = true; }
    else if FIRST_SYMBOLS(" OFF") { INVERSED(ch) = false; }

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:SET_INVERSE %s", ch.ToNumber(), ch.IsInversed() ? "ON" : "OFF"));

    return buffer;
}


pchar SCPI::CHANNEL::RANGE_(pchar buffer)
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

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:SET_RANGE %s", ch.ToNumber(), map[RANGE(ch)].key));

    return buffer;
}


pchar SCPI::CHANNEL::OFFSET(pchar buffer)
{
    if FIRST_SYMBOLS("?")
    {
        int offset = (int)(0.5F * (RSHIFT(ch) - RShift::ZERO));
        SCPI::SendFormat(":CHANNNEL%d:OFFSET %d", ch.ToNumber(), offset);
    }
    else
    {
        Int value(buffer);

        if (value.IsValid() && value >= -240 && value <= 240)
        {
            int rShift = RShift::ZERO + 2 * value;
            RShift::Set(ch, (int16)rShift);
        }
    }

    return buffer;
}


pchar SCPI::CHANNEL::FACTOR(pchar buffer)
{
    if      FIRST_SYMBOLS(" X1")  { DIVIDER(ch) = Divider::_1; }
    else if FIRST_SYMBOLS(" X10") { DIVIDER(ch) = Divider::_10; }

    IF_REQUEST(SCPI::SendFormat(":CHANNEL%d:PROBE %s", ch.ToNumber(), DIVIDER(ch) == Divider::_1 ? "X1" : "X10"));

    return buffer;
}
