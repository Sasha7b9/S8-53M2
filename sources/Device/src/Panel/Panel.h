// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "common/Panel/Controls.h"


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
        void EnableRegSet(bool enable);

        // ��������/��������� ��������� ����� 1.
        void EnableChannelA(bool enable);

        // ��������/��������� ��������� ����� 2.
        void EnableChannelB(bool enable);

        // ��������/��������� ��������� �����.
        void EnableTrig(bool enable);
    }

    namespace Callback
    {
        // ��� ������� ������ ���������� �� ������� ������ SPI5
        void OnReceiveSPI5(const uint8 *data, uint size);
    }
};
