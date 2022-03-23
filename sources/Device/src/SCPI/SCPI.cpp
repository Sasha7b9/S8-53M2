// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "SCPI.h"
#include "Utils/Strings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Containers/Buffer.h"
#include <cctype>
#include <cstring>
#include <cstdlib>


enum
{
    WAIT,
    SAVE_SYMBOLS
} StateProcessing;

static int FindNumSymbolsInCommand(uint8 *buffer);

static const int SIZE_BUFFER = 100;
// Сюда добавляем новые данные
static uint8 bufData[SIZE_BUFFER];
// Указатель на первый свободный байт буфера
static int pointer = 0;



void SCPI::AddNewData(uint8 *data, uint length)
{
    std::memcpy(&bufData[pointer], data, length);
    pointer += length;

label_another:

    for (int i = 0; i < pointer; i++)
    {
        bufData[i] = (uint8)std::toupper((int8)bufData[i]);

        if (bufData[i] == 0x0d || bufData[i] == 0x0a)
        {
            uint8 *pBuffer = bufData;
            while (*pBuffer == ':' || *pBuffer == 0x0d || *pBuffer == 0x0a)
            {
                ++pBuffer;
            }

            SCPI::ParseNewCommand(pBuffer);
            if (i == pointer - 1)
            {
                pointer = 0;                // Если буфер пуст - выходим
                return;
            }
            else                            // Если в буфере есть есть данные
            {
                uint8 *pBuf = bufData;
                for (++i; i < pointer; i++)
                {
                    *pBuf = bufData[i];   // копируем их в начало
                    ++pBuf;
                    pointer = pBuf - bufData;
                }
                goto label_another;         // и проверяем буфер ещё раз
            }
        }
    }
}



void SCPI::ParseNewCommand(uint8 *buf)
{
    static const StructCommand commands[] =
    {
    {"*IDN ?",      SCPI::COMMON::IDN},
    {"*IDN?",       SCPI::COMMON::IDN},
    {"RUN",         SCPI::COMMON::RUN},
    {"STOP",        SCPI::COMMON::STOP},
    {"*RST",        SCPI::COMMON::RESET},
    {"AUTOSCALE",   SCPI::COMMON::AUTOSCALE}, 
    {"REQUEST ?",   SCPI::COMMON::REQUEST},

    {"DISPLAY",     ProcessDISPLAY},       // Вначале всегда идёт полное слово, потом сокращение.
    {"DISP",        ProcessDISPLAY},       // Это нужно для правильного парсинга.

    {"CHANNEL1",    ProcessCHANNEL},
    {"CHAN1",       ProcessCHANNEL},

    {"CHANNEL2",    ProcessCHANNEL},
    {"CHAN2",       ProcessCHANNEL},

    {"TRIGGER",     ProcessTRIG},
    {"TRIG",        ProcessTRIG},

    {"TBASE",       ProcessTBASE},
    {"TBAS" ,       ProcessTBASE},

    {"KEY",         SCPI::CONTROL::KEY},
    {"GOVERNOR",    SCPI::CONTROL::GOVERNOR},
    {0}
    };
    
    SCPI::ProcessingCommand(commands, buf);
}



void SCPI::ProcessingCommand(const StructCommand *commands, uint8 *buffer) 
{
    int sizeNameCommand = FindNumSymbolsInCommand(buffer);
    if (sizeNameCommand == 0) 
    {
        return;
    }
    for (int i = 0; i < sizeNameCommand; i++)
    {
        buffer[i] = (uint8)std::toupper((int8)buffer[i]);
    }
    int numCommand = -1;
    char *name = 0;
    do 
    {
        numCommand++;   
        name = commands[numCommand].name;
    } while (name != 0 && (!EqualsStrings((char*)buffer, name, sizeNameCommand)));

    if (name != 0) 
    {
        commands[numCommand].func(buffer + sizeNameCommand + 1);
    }
    else
    {
        SCPI_SEND(":COMMAND ERROR");
    }
}



int FindNumSymbolsInCommand(uint8 *buffer)
{
    int pos = 0;
    while ((buffer[pos] != ':') && (buffer[pos] != ' ') && (buffer[pos] != '\x0d'))
    {
        pos++;
    }
    return pos;
}



bool SCPI::FirstIsInt(uint8 *buffer, int *value, int min, int max)
{
    Word param;

    if (GetWord(buffer, &param, 0))
    {
        Buffer<char> data(param.numSymbols + 1);
        std::memcpy(data.Data(), param.address, (uint)param.numSymbols);
        data.Data()[param.numSymbols] = '\0';
        return SU::String2Int(data.Data(), value) && (*value >= min) && (*value <= max);
    }

    return false;
}
