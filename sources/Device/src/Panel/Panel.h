// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"


class Panel
{
public:

    static void Init();

    static void EnableLEDRegSet(bool enable);
    // ��������/��������� ��������� ����� 1.
    static void EnableLEDChannel0(bool enable);
    // ��������/��������� ��������� ����� 2.
    static void EnableLEDChannel1(bool enable);
    // ��������/��������� ��������� �����.
    static void EnableLEDTrig(bool enable);
    // �������� ������� � �� ������ ����������.
    static void TransmitData(uint16 data);
    // � ���������� ������ ������ ���� ��������� ��������� ���������� pressedButton, �� �������� ������ ������� ��������.
    static void Disable();

    static void Enable();
    // ������� ������� �������.
    static PanelButton WaitPressingButton();

    static bool ProcessingCommandFromPIC(uint16 command);

    static uint16 NextData();
};
