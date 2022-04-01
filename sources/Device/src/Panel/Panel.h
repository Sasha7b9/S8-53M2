// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "common/Panel/Controls.h"


class LED
{
public:
    enum Type
    {
        Trig     = 1,
        RegSet   = 2,
        ChannelA = 3,
        ChannelB = 4,
    };

    LED(Type _type) : type(_type) { }
    void Enable();
    void Disable();
    void Switch(bool enable);
private:
    Type type;
};


extern LED led_Trig;
extern LED led_RegSet;
extern LED led_ChanA;
extern LED led_ChanB;


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
        // Включить/выключить светодиод КАНАЛ 1.
        void EnableChannelA(bool enable);

        // Включить/выключить светодиод КАНАЛ 2.
        void EnableChannelB(bool enable);
    }

    namespace Callback
    {
        // Эта функция должна вызываться из приёмной фунции SPI5
        void OnReceiveSPI5(const uint8 *data, uint size);
    }
};
