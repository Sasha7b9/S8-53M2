// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "Panel/Controls_Old.h"


namespace Panel
{
    void Init();

    void Update();

    void EnableLEDRegSet(bool enable);

    // ��������/��������� ��������� ����� 1.
    void EnableLEDChannelA(bool enable);

    // ��������/��������� ��������� ����� 2.
    void EnableLEDChannelB(bool enable);

    // ��������/��������� ��������� �����.
    void EnableLEDTrig(bool enable);

    // � ���������� ������ ������ ���� ��������� ��������� ���������� pressedButton, �� �������� ������ ������� ��������.
    void Disable();

    void Enable();

    // ������� ������� �������.
    Key::E WaitPressingButton();

    // ��� ������� ������ ���������� �� ������� ������ SPI5
    void CallbackOnReceiveSPI5(const uint8 *data, uint size);

    void ProcessingCommandFromPIC(uint16 command);

    uint16 NextData();

    // �������� ������� � �� ������ ����������.
    void TransmitData(uint8 data);
};
