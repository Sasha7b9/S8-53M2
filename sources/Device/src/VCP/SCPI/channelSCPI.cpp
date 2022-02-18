// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "SCPI.h"
#include "Settings/Settings.h"
#include "Utils/Map.h"
#include "Utils/GlobalFunctions.h"
#include "VCP/VCP.h"
#include "FPGA/FPGA.h"



static Chan::E ch = Chan::A;



ENTER_PARSE_FUNC(SCPI::ProcessCHANNEL)
        {"INPUT",       SCPI::CHANNEL::INPUT},
        {"COUPLING",    SCPI::CHANNEL::COUPLE},
        {"COUPL",       SCPI::CHANNEL::COUPLE},
        {"FILTR",       SCPI::CHANNEL::FILTR},
        {"SET_INVERSE", SCPI::CHANNEL::INVERSE},
        {"INV",         SCPI::CHANNEL::INVERSE},
        {"SET_RANGE",   SCPI::CHANNEL::RANGE},
        {"OFFSET",      SCPI::CHANNEL::OFFSET},
        {"FACTOR",      SCPI::CHANNEL::FACTOR},
        {"FACT",        SCPI::CHANNEL::FACTOR},
        {0}
    };

    ch = (char)(*(buffer - 2)) == '1' ? Chan::A : Chan::B;

    SCPI::ProcessingCommand(commands, buffer);
}



void SCPI::CHANNEL::INPUT(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_ENABLED(ch) = true; }
        else if (1 == value)    { SET_ENABLED(ch) = false; }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:INPUT %s", Chan::GetNumber(ch), Chan::Enabled(ch) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}



extern void OnChanged_CoupleA(bool);
extern void OnChanged_CoupleB(bool);



void SCPI::CHANNEL::COUPLE(uint8 *buffer)
{
    static const pFuncVB func[2] = {OnChanged_CoupleA, OnChanged_CoupleB};

    static const MapElement map[] = 
    {
        {"DC",  0},
        {"AC",  1},
        {"GND", 2},
        {"?",   3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_COUPLE(ch) = ModeCouple::DC; func[ch](true); }
        else if (1 == value)    { SET_COUPLE(ch) = ModeCouple::AC; func[ch](true); }
        else if (2 == value)    { SET_COUPLE(ch) = ModeCouple::GND; func[ch](true); }
        else if (3 == value)
        {
            SCPI_SEND(":CHANNEL%d:COUPLING %s", Chan::GetNumber(ch), map[SET_COUPLE(ch)].key);
        }
    LEAVE_ANALYSIS
}



extern void OnChanged_FiltrA(bool activate);
extern void OnChanged_FiltrB(bool activate);



void SCPI::CHANNEL::FILTR(uint8 *buffer)
{
    static const pFuncVB func[2] = {OnChanged_FiltrA, OnChanged_FiltrB};

    static const MapElement map[] =
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_FILTR(ch) = true; func[ch](true); }
        else if (1 == value)    { SET_FILTR(ch) = false; func[ch](true); }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:FILTR %s", Chan::GetNumber(ch), SET_FILTR(ch) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}



void SCPI::CHANNEL::INVERSE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_INVERSE(ch) = true; }
        else if (1 == value)    { SET_INVERSE(ch) = false; }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:SET_INVERSE %s", Chan::GetNumber(ch), SET_INVERSE(ch) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}



void SCPI::CHANNEL::RANGE(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"2MV",   (uint8)Range::_2mV},
        {"5MV",   (uint8)Range::_5mV},
        {"10MV",  (uint8)Range::_10mV},
        {"20MV",  (uint8)Range::_20mV},
        {"50MV",  (uint8)Range::_50mV},
        {"100MV", (uint8)Range::_100mV},
        {"200MV", (uint8)Range::_200mV},
        {"500MV", (uint8)Range::_500mV},
        {"1V",    (uint8)Range::_1V},
        {"2V",    (uint8)Range::_2V},
        {"5V",    (uint8)Range::_5V},
        {"10V",   (uint8)Range::_10V},
        {"20V",   (uint8)Range::_20V},
        {"?",     (uint8)Range::Count},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= (uint8)Range::_20V)      { Range::Set(ch, (Range::E)value); }
        else if (value == (uint8)Range::Count)
        {
            SCPI_SEND(":CHANNEL%d:SET_RANGE %s", Chan::GetNumber(ch), map[SET_RANGE(ch)].key);
        }
    LEAVE_ANALYSIS
}



void SCPI::CHANNEL::OFFSET(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"?", 0},
        {0}
    };
    int intVal = 0;
    if (SCPI::FirstIsInt(buffer, &intVal, -240, 240))
    {
        int rShift = RShift::ZERO + 2 * intVal;
        RShift::Set(ch, (int16)rShift);
        return;
    }
    ENTER_ANALYSIS
        if (value == 0)
        {
            int retValue = 0.5f * (SET_RSHIFT(ch) - RShift::ZERO);
            SCPI_SEND(":CHANNNEL%d:OFFSET %d", Chan::GetNumber(ch), retValue);
        }
    LEAVE_ANALYSIS
}



void SCPI::CHANNEL::FACTOR(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"X1",  0},
        {"X10", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (value == 0)         { SET_DIVIDER(ch) = Divider::_1; }
        else if (value == 1)    { SET_DIVIDER(ch) = Divider::_10; }
        else if (value == 2)
        {
            SCPI_SEND(":CHANNEL%d:PROBE %s", Chan::GetNumber(ch), map[SET_DIVIDER(ch)].key);
        }
    LEAVE_ANALYSIS
}
