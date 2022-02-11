// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 


#include "defines.h"
#include "Controls.h"


void    Panel_Init(void);
void    Panel_DeInit(void);
void    Panel_Update(void);
void    Panel_TransmitData(uint16 data);            // �������� ������� � �� ������ ����������.
void    Panel_Disable(void);                        // � ���������� ������ ������ ���� ��������� ��������� ���������� pressedButton, �� �������� ������ ������� ��������.
void    Panel_Enable(void);
uint16  Panel_NextData(void);
PanelButton Panel_WaitPressingButton(void);       // ������� ������� �������.
PanelButton Panel_PressedButton(void);
