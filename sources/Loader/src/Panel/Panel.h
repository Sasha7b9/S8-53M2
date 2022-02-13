// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 


#include "defines.h"
#include "Controls.h"


void    Panel_Init();
void    Panel_DeInit();
void    Panel_Update();
void    Panel_TransmitData(uint16 data);            // �������� ������� � �� ������ ����������.
void    Panel_Disable();                        // � ���������� ������ ������ ���� ��������� ��������� ���������� pressedButton, �� �������� ������ ������� ��������.
void    Panel_Enable();
uint16  Panel_NextData();
PanelButton Panel_WaitPressingButton();       // ������� ������� �������.
PanelButton Panel_PressedButton();
