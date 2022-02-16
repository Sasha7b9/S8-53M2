// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"


class Panel
{
public:

    static void Init();

    static void EnableLEDRegSet(bool enable);

    // Включить/выключить светодиод КАНАЛ 1.
    static void EnableLEDChannel0(bool enable);

    // Включить/выключить светодиод КАНАЛ 2.
    static void EnableLEDChannel1(bool enable);

    // Включить/выключить светодиод СИНХР.
    static void EnableLEDTrig(bool enable);

    // Передать даннные в мк панели управления.
    static void TransmitData(uint16 data);

    // В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
    static void Disable();

    static void Enable();

    // Ожидать нажатие клавиши.
    static PanelButton WaitPressingButton();

    static bool ProcessingCommandFromPIC(uint16 command);

    // Эта функция должна вызываться из приёмной фунции SPI5
    static void CallbackOnReceiveSPI5(const uint8 *data, uint size);

    static uint16 NextData();
};
