// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "common/Panel/Controls.h"


class LED
{
public:

    static LED Trig;
    static LED RegSet;
    static LED ChanA;
    static LED ChanB;

    enum Type
    {
        _Trig     = 1,
        _RegSet   = 2,
        _ChannelA = 3,
        _ChannelB = 4,
    };

    LED(Type _type) : type(_type) { }
    void Enable();
    void Disable();
    void Switch(bool enable);

    // ���� true, �� �� ������ �������� LED::Trig - ��������� ����������
    static bool dontFireTrig;

private:
    Type type;
};


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

    // �������� ������� � �� ������ ����������.
    void TransmitData(uint8 data);

    // ������� ������ ����� ���������� �������
    uint TimePassedAfterLastEvent();

    namespace Data
    {
        uint16 Next();

        // ���� ������ ��� ��������
        bool Exist();
    }

    namespace Callback
    {
        // ��� ������� ������ ���������� �� ������� ������ SPI5
        void OnReceiveSPI5(const uint8 *data, uint size);
    }
};
