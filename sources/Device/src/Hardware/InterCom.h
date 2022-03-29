// 2022/2/11 22:18:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Интерфейс коммуниаций с ПК по USB и LAN
namespace InterCom
{
    // Возвращает true, если можно передавать изображение ГУИ
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
    void PushWord(int word)
    {
        PushHalfWord((uint16)word);
        PushHalfWord((uint16)(word >> 16));
    }
    void Transmit(int num_bytes)        { InterCom::Send(Data(), num_bytes); }
    uint8 *GetByte(int num_byte)        { return &data[num_byte]; }
private:
    uint8 *Data() { return data; };
    const int SIZE;
    uint8 data[size];
    int pointer;
};
