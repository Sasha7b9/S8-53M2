// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "Panel/Controls_Old.h"


namespace Panel
{
    void Init();

    void Update();

    void EnableLEDRegSet(bool enable);

    // Включить/выключить светодиод КАНАЛ 1.
    void EnableLEDChannelA(bool enable);

    // Включить/выключить светодиод КАНАЛ 2.
    void EnableLEDChannelB(bool enable);

    // Включить/выключить светодиод СИНХР.
    void EnableLEDTrig(bool enable);

    // В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
    void Disable();

    void Enable();

    // Ожидать нажатие клавиши.
    Key::E WaitPressingButton();

    // Эта функция должна вызываться из приёмной фунции SPI5
    void CallbackOnReceiveSPI5(const uint8 *data, uint size);

    void ProcessingCommandFromPIC(uint16 command);

    uint16 NextData();

    // Передать даннные в мк панели управления.
    void TransmitData(uint8 data);
};
