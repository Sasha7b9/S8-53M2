// 2022/2/11 22:18:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct CommandBuffer
{
    CommandBuffer(int size, uint8 type);
    void PushByte(uint8);
    void PushByte(int);
    void PushHalfWord(uint16);
    void PushHalfWord(int);
    void PushWord(int);
    uint8 *Data();
};


// Интерфейс коммуниаций с ПК по USB и LAN
namespace InterCom
{
    // Возвращает true, если можно передавать изображение ГУИ
    bool TransmitGUIinProcess();

    void Send(uint8 *, int);
}
