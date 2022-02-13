// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "SCPI.h"
#include "Settings/Settings.h"
#include "Utils/Map.h"
#include "VCP/VCP.h"
#include "FPGA/FPGA.h"



ENTER_PARSE_FUNC(SCPI::ProcessTBASE)
    {"SET_RANGE",   SCPI::TBASE::RANGE},
    {"OFFSET",      SCPI::TBASE::OFFSET},
    {"SAMPLING",    SCPI::TBASE::SAMPLING},
    {"SAMPL",       SCPI::TBASE::SAMPLING},
    {"PEACKDET",    SCPI::TBASE::PEACKDET},
    {"PEACK",       SCPI::TBASE::PEACKDET},
    {"TPOS",        SCPI::TBASE::TPOS},
    {"SELFRECORDER",SCPI::TBASE::SELFRECORDER},
    {"FUNCTIMEDIV", SCPI::TBASE::FUNCTIMEDIV},
LEAVE_PARSE_FUNC



void SCPI::TBASE::RANGE(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"2ns",     (uint8)TBase::_2ns},
        {"5ns",     (uint8)TBase::_5ns},
        {"10ns",    (uint8)TBase::_10ns},
        {"20ns",    (uint8)TBase::_20ns},
        {"50ns",    (uint8)TBase::_50ns},
        {"100ns",   (uint8)TBase::_100ns},
        {"200ns",   (uint8)TBase::_200ns},
        {"500ns",   (uint8)TBase::_500ns},
        {"1us",     (uint8)TBase::_1us},
        {"2us",     (uint8)TBase::_2us},
        {"5us",     (uint8)TBase::_5us},
        {"10us",    (uint8)TBase::_10us},
        {"20us",    (uint8)TBase::_20us},
        {"50us",    (uint8)TBase::_50us},
        {"100us",   (uint8)TBase::_100us},
        {"200us",   (uint8)TBase::_200us},
        {"500us",   (uint8)TBase::_500us},
        {"1ms",     (uint8)TBase::_1ms},
        {"2ms",     (uint8)TBase::_2ms},
        {"5ms",     (uint8)TBase::_5ms},
        {"10ms",    (uint8)TBase::_10ms},
        {"20ms",    (uint8)TBase::_20ms},
        {"50ms",    (uint8)TBase::_50ms},
        {"100ms",   (uint8)TBase::_100ms},
        {"200ms",   (uint8)TBase::_200ms},
        {"500ms",   (uint8)TBase::_500ms},
        {"1s",      (uint8)TBase::_1s},
        {"2s",      (uint8)TBase::_2s},
        {"5s",      (uint8)TBase::_5s},
        {"10s",     (uint8)TBase::_10s},
        {"?",       255},
        {0}
    };
    ENTER_ANALYSIS
        if (TBase::Count > value) { TBase::Set((TBase::E)value); }
        else if (255 == value)
        {
            SCPI_SEND(":TBASE:SET_RANGE %s", Tables_GetTBaseStringEN(SET_TBASE));
        }
    LEAVE_ANALYSIS
}



void SCPI::TBASE::OFFSET(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"?", 0},
        {0}
    };

    int intVal = 0;
    if (SCPI::FirstIsInt(buffer, &intVal, -1024, 15000))
    {
        int tShift = intVal + 1024;
        TShift::Set(tShift);
        return;
    }

    ENTER_ANALYSIS
        if (0 == value)
        {
            int retValue = TSHIFT - 1024;
            SCPI_SEND(":TBASE:OFFSET %d", retValue);
        }
    LEAVE_ANALYSIS
}



void SCPI::TBASE::SAMPLING(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"EQUAL", 0},
        {"REAL",  1},
        {"?",     2},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 2) { SAMPLE_TYPE = (SampleType)value; }
        else if (2 == value)
        {
            SCPI_SEND(":TBASE:SAMPLING %s", map[SAMPLE_TYPE].key);
        }
    LEAVE_ANALYSIS
}



void SCPI::TBASE::PEACKDET(uint8 *buffer)
{
    extern void OnChanged_PeakDet(bool active);  // \todo ¬ообще-то это нехорошо, как нехорошо и дублировать. Ќадо бы подумать.

    static const MapElement map[] =
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 2) { PEAKDET = (value == 0) ? PeackDetMode::Disable : PeackDetMode::Enable; OnChanged_PeakDet(true); } // WARN SCPI дл€ пикового детектора переделать
        else if (2 == value)
        {
            SCPI_SEND(":TBASE:PEACKDET %s", PEAKDET ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}



void SCPI::TBASE::TPOS(uint8 *buffer)
{
    extern void OnChanged_TPos(bool active);

    static const MapElement map[] =
    {
        {"LEFT",   0},
        {"CENTER", 1},
        {"RIGHT",  2},
        {"?",      3},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 3)      { SET_TPOS = (TPos)value; OnChanged_TPos(true); }
        else if (4 == value)
        {
            SCPI_SEND(":TBASE:TPOS %s", map[SET_TPOS].key);
        }
    LEAVE_ANALYSIS
}



void SCPI::TBASE::SELFRECORDER(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON", 0},
        {"OFF", 1},
        {"?", 2},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 2) { SET_SELFRECORDER = (value == 0); }
        else if (2 == value)
        {
            SCPI_SEND(":TBASE:SELFRECORDER %s", SET_SELFRECORDER ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}



void SCPI::TBASE::FUNCTIMEDIV(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"TIME",   0},
        {"MEMORY", 1},
        {"?",      2},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 2) { TIME_DIV_XPOS = (FunctionTime)value; }
        else if (2 == value)
        {
            SCPI_SEND(":TBASE:FUNCTIMEDIV %s", map[TIME_DIV_XPOS].key);
        }
    LEAVE_ANALYSIS
}
