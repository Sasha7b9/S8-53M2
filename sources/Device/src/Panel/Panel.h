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

    // � ���������� ������ ������ ���� ��������� ��������� ���������� pressedButton, �� �������� ������ ������� ��������.
    void Disable();

    void Enable();

    // ������� ������� �������.
    Key::E WaitPressingButton();

    void ProcessEvent(KeyboardEvent);

    uint16 NextData();

    // �������� ������� � �� ������ ����������.
    void TransmitData(uint8 data);

    // ������� ������ ����� ���������� �������
    uint TimePassedAfterLastEvent();

    namespace LED
    {
        // ��������/��������� ��������� ����� 1.
        void EnableChannelA(bool enable);

        // ��������/��������� ��������� ����� 2.
        void EnableChannelB(bool enable);
    }

    namespace Callback
    {
        // ��� ������� ������ ���������� �� ������� ������ SPI5
        void OnReceiveSPI5(const uint8 *data, uint size);
    }
};
