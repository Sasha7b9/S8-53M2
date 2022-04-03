// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP_.h"
#include "FPGA/FPGA.h"
#include "Menu/Pages/Definition.h"
#include "SCPI/SCPI.h"


pchar SCPI::ProcessTBASE(pchar buffer)
{
    static const StructCommand commands[] =
    {
        {":RANGE",       SCPI::TBASE_::RANGE_},
        {":OFFSET",      SCPI::TBASE_::OFFSET},
        {":SAMPLING",    SCPI::TBASE_::SAMPLING},
        {":SAMPL",       SCPI::TBASE_::SAMPLING},
        {":PEACKDET",    SCPI::TBASE_::PEACKDET},
        {":PEACK",       SCPI::TBASE_::PEACKDET},
        {":TPOS",        SCPI::TBASE_::TPOS_},
        {":SELFRECORDER",SCPI::TBASE_::SELFRECORDER},
        {":FUNCTIMEDIV", SCPI::TBASE_::FUNCTIMEDIV},
        {0}
    };

    return SCPI::ProcessingCommand(commands, buffer);
}


pchar SCPI::TBASE_::RANGE_(pchar buffer)
{
    static const MapElement map[] = 
    {
        {" 2NS",     (uint8)TBase::_2ns},
        {" 5NS",     (uint8)TBase::_5ns},
        {" 10NS",    (uint8)TBase::_10ns},
        {" 20NS",    (uint8)TBase::_20ns},
        {" 50NS",    (uint8)TBase::_50ns},
        {" 100NS",   (uint8)TBase::_100ns},
        {" 200NS",   (uint8)TBase::_200ns},
        {" 500NS",   (uint8)TBase::_500ns},
        {" 1US",     (uint8)TBase::_1us},
        {" 2US",     (uint8)TBase::_2us},
        {" 5US",     (uint8)TBase::_5us},
        {" 10US",    (uint8)TBase::_10us},
        {" 20US",    (uint8)TBase::_20us},
        {" 50US",    (uint8)TBase::_50us},
        {" 100US",   (uint8)TBase::_100us},
        {" 200US",   (uint8)TBase::_200us},
        {" 500US",   (uint8)TBase::_500us},
        {" 1MS",     (uint8)TBase::_1ms},
        {" 2MS",     (uint8)TBase::_2ms},
        {" 5MS",     (uint8)TBase::_5ms},
        {" 10MS",    (uint8)TBase::_10ms},
        {" 20MS",    (uint8)TBase::_20ms},
        {" 50MS",    (uint8)TBase::_50ms},
        {" 100MS",   (uint8)TBase::_100ms},
        {" 200MS",   (uint8)TBase::_200ms},
        {" 500MS",   (uint8)TBase::_500ms},
        {" 1S",      (uint8)TBase::_1s},
        {" 2S",      (uint8)TBase::_2s},
        {" 5S",      (uint8)TBase::_5s},
        {" 10S",     (uint8)TBase::_10s},
        {0}
    };

    SCPI_CYCLE(TBase::Set((TBase::E)it->value));

    IF_REQUEST(SCPI::SendFormat(":TBASE:RANGE%s", map[TBASE].value));

    return buffer;
}


pchar SCPI::TBASE_::OFFSET(pchar buffer)
{
    if FIRST_SYMBOLS("?")
    {
        int shift = TSHIFT - 1024;
        SCPI::SendFormat(":TBASE:OFFSET %d", shift);
    }
    else
    {
        Int value(buffer);

        if (value.IsValid() && value >= -1024 && value <= 15000)
        {
            int shift = value + 1024;
            TShift::Set(shift);
        }
    }

    return buffer;
}


pchar SCPI::TBASE_::SAMPLING(pchar buffer)
{
    static const MapElement map[] =
    {
        {" EQUAL", 0},
        {" REAL",  1},
        {0}
    };

    SCPI_CYCLE(SAMPLE_TYPE = (SampleType::E)it->value);

    IF_REQUEST(SCPI::SendFormat(":TBASE:SAMPLING%s", map[SAMPLE_TYPE].key));

    return buffer;
}


pchar SCPI::TBASE_::PEACKDET(pchar buffer)
{
    static const MapElement map[] =
    {
        {" OFF",  0},
        {" ON", 1},
        {0}
    };

    SCPI_CYCLE(MODE_PEAK_DET = (PeakDetMode::E)it->value);

    IF_REQUEST(SCPI::SendFormat(":TBASE:PEACKDET%s", map[MODE_PEAK_DET].key));

    return buffer;
}


pchar SCPI::TBASE_::TPOS_(pchar buffer)
{
    static const MapElement map[] =
    {
        {" LEFT",   0},
        {" CENTER", 1},
        {" RIGHT",  2},
        {0}
    };

    SCPI_CYCLE(TPOS = (TPos::E)it->value; PageTime::OnChanged_TPos(true));

    IF_REQUEST(SCPI::SendFormat(":TBASE:TPOS%s", map[TPOS].key));

    return buffer;
}


pchar SCPI::TBASE_::SELFRECORDER(pchar buffer)
{
    if      FIRST_SYMBOLS(" ON")  { SELF_RECORDER = true; }
    else if FIRST_SYMBOLS(" OFF") { SELF_RECORDER = false; }

    IF_REQUEST(SCPI::SendFormat(":TBASE:SELFRECORDER%s", PageTime::InSelfRecoredMode() ? "ON" : "OFF"));

    return buffer;
}


pchar SCPI::TBASE_::FUNCTIMEDIV(pchar buffer)
{
    static const MapElement map[] =
    {
        {" TIME",   0},
        {" MEMORY", 1},
        {0}
    };

    SCPI_CYCLE(FUNCTION_REG_TIME = (FunctionTime::E)it->value);

    IF_REQUEST(SCPI::SendFormat(":TBASE:FUNCTIMEDIV%s", map[FUNCTION_REG_TIME].key));

    return buffer;
}
