#include "defines.h"
#include "Hardware/InterCom.h"
#include "Hardware/VCP/VCP.h"
#include "Hardware/LAN/LAN.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>


namespace InterCom
{
    struct StateTransmit
    {
        enum E
        {
            Free,               // Нужно передавать
            InProcess           // Не нужно передавать
        };
    };

    static StateTransmit::E stateTransmit = StateTransmit::Free;

    namespace Sender
    {
        bool needSendPalette = false;
        bool needSendFrame = false;
    }
}


void InterCom::BeginScene()
{
    if (Sender::needSendPalette)
    {
        Sender::needSendPalette = false;

        for (int i = 0; i < 16; i++)
        {
            CommandBuffer<6> command(SET_PALETTE);
            command.PushByte(i);
            command.PushWord(COLOR(i));
            command.Transmit();
        }
    }

    if (Sender::needSendFrame)
    {
        Sender::needSendFrame = false;

        stateTransmit = StateTransmit::InProcess;
    }

    if (stateTransmit)
    {
        VCP::_meter.Reset();
        VCP::_meter.Pause();
        VCP::_sended_bytes = 0;
    }
}


void InterCom::EndScene()
{
    if (TransmitGUIinProcess())
    {
        CommandBuffer<1> command(END_SCENE);
        command.Transmit();

        Flush();

        // LOG_WRITE("время ожидания %d ms, байт передано %d", VCP::_meter.ElapsedTime(), VCP::_sended_bytes);
    }

    stateTransmit = StateTransmit::Free;
}


bool InterCom::TransmitGUIinProcess()
{
    return (stateTransmit == StateTransmit::InProcess);
}


void InterCom::Send(const void* pointer, int size)
{
    VCP::Buffer::Send(pointer, size);
    LAN::SendBuffer(pointer, size);
}


void InterCom::SendFormat0D(pchar format, ...)
{
    char buffer[128];
    std::va_list args;
    va_start(args, format);
    std::vsprintf(buffer, format, args);
    va_end(args);
    std::strcat(buffer, "\n");

    VCP::Buffer::Send(buffer, (int)std::strlen(buffer));
    LAN::SendBuffer(buffer, (int)std::strlen(buffer));
}


void InterCom::Flush()
{
    VCP::Buffer::Flush();
}
