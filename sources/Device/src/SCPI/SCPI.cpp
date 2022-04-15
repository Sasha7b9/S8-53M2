// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/VCP/VCP.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/String.h"
#include "Hardware/LAN/LAN.h"
#include "SCPI/SCPI.h"
#include "SCPI/BufferSCPI.h"
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#include <cstdio>


namespace SCPI
{
    static pchar ParseNewCommand(pchar buffer);

    static BufferSCPI input;
}


void SCPI::AppendNewData(const void *data, int length)
{
    input.Append((pchar)data, length);
}


void SCPI::Update()
{
    input.RemoveFirstDividers();

    while (input.ExistDivider())
    {
        String<> command = input.ExtractCommand();

        pchar result = SCPI::ParseNewCommand(command.c_str());

        if(result != nullptr)
        {
            SCPI::SendFormat0D("Error !!! Invalid sequency \"%s\"", result);
        }
    }
}


pchar SCPI::ParseNewCommand(pchar buffer)
{
    static const StructCommand commands[] =
    {
        {"*IDN?",       SCPI::COMMON::IDN},
        {":RUN",        SCPI::COMMON::RUN},
        {":STOP",       SCPI::COMMON::STOP},
        {"*RST",        SCPI::COMMON::RESET},
        {":AUTOSCALE",  SCPI::COMMON::AUTOSCALE},
        {":REQUEST ?",  SCPI::COMMON::REQUEST},

        {":DISPLAY",    ProcessDISPLAY},       // Вначале всегда идёт полное слово, потом сокращение.
        {":DISP",       ProcessDISPLAY},       // Это нужно для правильного парсинга.

        {":CHANNEL1",   ProcessCHANNEL},
        {":CHAN1",      ProcessCHANNEL},

        {":CHANNEL2",   ProcessCHANNEL},
        {":CHAN2",      ProcessCHANNEL},

        {":TRIGGER",    ProcessTRIG},
        {":TRIG",       ProcessTRIG},

        {":TBASE",      ProcessTBASE},
        {":TBAS" ,      ProcessTBASE},

        {":SPEEDTEST",  ProcessSpeedTest},

        {":KEY",        SCPI::CONTROL::KEY},
        {":GOVERNOR",   SCPI::CONTROL::GOVERNOR},
        {0}
    };

    return SCPI::ProcessingCommand(commands, buffer);
}



pchar SCPI::ProcessSpeedTest(pchar)
{
    TimeMeterMS meter;

    uint num_bytes = 0;

    while (meter.ElapsedTime() < 1000)
    {
        const int SIZE_BUFFER = 1028;
        static char buffer[SIZE_BUFFER];

        VCP::SendBuffer(buffer, SIZE_BUFFER);

        num_bytes += SIZE_BUFFER;
    }

    LOG_WRITE("Передано %d байт, скорость %f кб/сек", num_bytes, num_bytes / (float)meter.ElapsedTime());

    return nullptr;
}



pchar SCPI::ProcessingCommand(const StructCommand *commands, pchar buffer)
{
    for (const StructCommand *command = commands; command->name != 0; command++)
    {
        int size_name = (int)std::strlen(command->name);

        if (SU::EqualsStrings(buffer, command->name, size_name))
        {
            return command->func(buffer + size_name);
        }
    }

    return buffer;
}


bool SCPI::FirstSymbols(pchar *data, pchar word)
{
    pchar buffer = *data;

    while (*word && *word == *buffer)
    {
        word++;
        buffer++;
    }

    if (*word)
    {
        return false;
    }

    *data = buffer;

    return true;
}


void SCPI::SendBuffer(const uint8 *buffer, int size)
{
    VCP::SendBuffer(buffer, size);
    LAN::SendBuffer(buffer, size);
}


void SCPI::SendFormat0D(pchar format, ...)
{
    char buffer[128];
    std::va_list args;
    va_start(args, format);
    std::vsprintf(buffer, format, args);
    va_end(args);
    std::strcat(buffer, "\n");

    VCP::SendBuffer(buffer, (int)std::strlen(buffer));
    LAN::SendBuffer(buffer, (int)std::strlen(buffer));
}


uint8 SCPI::MapElement::GetValue(Word *eKey) const
{
    const MapElement *map = this;

    int numKey = 0;
    char *_key = map[numKey].key;

    while (_key != 0)
    {
        if (eKey->WordEqualZeroString(_key))
        {
            return map[numKey].value;
        }

        numKey++;
        _key = map[numKey].key;
    }
    return 255;
}
