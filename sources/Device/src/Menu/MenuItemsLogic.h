// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "MenuItems.h"



void    IPaddress_ChangeValue(IPaddress *ip, int delta);        // Изменяет значение в текущей позиции при открытом элементе
void    IPaddress_GetNumPosIPvalue(int *numIP, int *selPos);        // Возвращает номер текущего байта (4 - номер порта) и номер текущей позиции в байте.

void    MACaddress_ChangeValue(MACaddress *mac, int delta);

void    ItemTime_SetOpened(Time *time);
void    ItemTime_SetNewTime(const Time *time);
void    ItemTime_SelectNextPosition(Time *time);
void    ItemTime_IncCurrentPosition(const Time *time);
void    ItemTime_DecCurrentPosition(const Time *time);

void    GovernorColor_ChangeValue(GovernorColor *governor, int delta);  // Изменить яркость цвета в governor

extern int8 gCurDigit;
