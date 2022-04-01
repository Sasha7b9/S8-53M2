// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "common/Panel/Controls.h"


namespace Panel
{
    void Init();

    void Update();

    // В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
    void Disable();

    void Enable();

    // Ожидать нажатие клавиши.
    Key::E WaitPressingButton();

    void ProcessEvent(KeyboardEvent);

    uint16 NextData();

    // Передать даннные в мк панели управления.
    void TransmitData(uint8 data);

    // Времени прошло после последнего события
    uint TimePassedAfterLastEvent();

    namespace LED
    {
        void EnableRegSet(bool enable);

        // Включить/выключить светодиод КАНАЛ 1.
        void EnableChannelA(bool enable);

        // Включить/выключить светодиод КАНАЛ 2.
        void EnableChannelB(bool enable);

        // Включить/выключить светодиод СИНХР.
        void EnableTrig(bool enable);
    }

    namespace Callback
    {
        // Эта функция должна вызываться из приёмной фунции SPI5
        void OnReceiveSPI5(const uint8 *data, uint size);
    }
};
