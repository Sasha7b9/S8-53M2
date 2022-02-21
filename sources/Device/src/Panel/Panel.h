// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"


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

    // �������� ������� � �� ������ ����������.
    void TransmitData(uint16 data);

    // � ���������� ������ ������ ���� ��������� ��������� ���������� pressedButton, �� �������� ������ ������� ��������.
    void Disable();

    void Enable();

    // ������� ������� �������.
    PanelButton WaitPressingButton();

    // ��� ������� ������ ���������� �� ������� ������ SPI5
    void CallbackOnReceiveSPI5(const uint8 *data, uint size);

    void ProcessingCommandFromPIC(uint16 command);

    uint16 NextData();
};
