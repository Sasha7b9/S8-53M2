// 2022/2/11 22:18:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Интерфейс коммуниаций с ПК по USB и LAN
namespace InterCom
{
    // Возвращает true, если можно передавать изображение ГУИ
    bool TransmitGUIinProcess();

    void Send(const uint8 *, int);
}



struct CommandBuffer
{
    CommandBuffer(int size, uint8 type);
    ~CommandBuffer();
    void PushByte(uint8 byte)           { data[pointer++] = byte; }
    void PushByte(int byte)             { PushByte((uint8)byte); }
    void PushHalfWord(uint16);
    void PushHalfWord(int half_word)    { PushHalfWord((uint16)half_word); }
    void PushWord(int);
    void Transmit(int num_bytes)        { InterCom::Send(Data(), num_bytes); }
    uint8 *GetByte(int num_byte)        { return &data[num_byte]; }
private:
    uint8 *Data() { return data; };
    uint8 *data;
    int size;
    int pointer;
};
