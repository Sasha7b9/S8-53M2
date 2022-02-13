// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 


#include "defines.h"
#include "Controls.h"


void    Panel_Init();
void    Panel_DeInit();
void    Panel_Update();
void    Panel_TransmitData(uint16 data);            // Передать даннные в мк панели управления.
void    Panel_Disable();                        // В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
void    Panel_Enable();
uint16  Panel_NextData();
PanelButton Panel_WaitPressingButton();       // Ожидать нажатие клавиши.
PanelButton Panel_PressedButton();
