// 2022/2/11 22:18:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/DisplayTypes.h"


// ��������� ����������� � �� �� USB � LAN
namespace InterCom
{
    // ���������� true, ���� ����� ���������� ����������� ���
    bool TransmitGUIinProcess();

    void Send(const uint8 *, int);
}



template<int size>
struct CommandBuffer
{
    CommandBuffer(uint8 type) : SIZE(size), pointer(0)
    {
        PushByte(type);
    }

    void PushByte(uint8 byte)           { data[pointer++] = byte; }
    void PushByte(int byte)             { PushByte((uint8)byte); }
    void PushHalfWord(uint16 half_word)
    {
        PushByte((uint8)half_word);
        PushByte((uint8)(half_word >> 8));
    }
    void PushHalfWord(int half_word)    { PushHalfWord((uint16)half_word); }
    void PushWord(uint word)
    {
        PushHalfWord((uint16)word);
        PushHalfWord((uint16)(word >> 16));
    }
    void PushWord(int word) { PushWord((uint)word); }
    void Transmit(int num_bytes)
    {
        if (
            data[0] == DRAW_TEXT ||
            data[0] == DRAW_VLINES_ARRAY ||
            data[0] == DRAW_MULTI_HPOINT_LINES
            )
        {
            InterCom::Send(Data(), num_bytes);
        }
    }
    void Transmit()
    {
        if (
            data[0] == SET_COLOR ||
            data[0] == FILL_REGION ||
            data[0] == END_SCENE ||
            data[0] == DRAW_HLINE ||
            data[0] == DRAW_VLINE ||
            data[0] == SET_POINT ||
            data[0] == DRAW_SIGNAL_LINES ||
            data[0] == SET_PALETTE ||
            data[0] == SET_FONT ||
            data[0] == DRAW_SIGNAL_POINTS
            )
        {
            InterCom::Send(Data(), size);
        }
    }
    uint8 *GetByte(int num_byte)        { return &data[num_byte]; }
private:
    uint8 *Data() { return data; };
    const int SIZE;
    uint8 data[size];
    int pointer;
};
