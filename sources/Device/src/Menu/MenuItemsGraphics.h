// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "MenuItems.h"




void ItemFormula_Draw(Formula *formula, int x, int y, bool opened);
void ItemChoice_Draw(Choice *choice, int x, int y, bool opened);
void ItemButton_Draw(Button *button, int x, int y);
void ItemSmallButton_Draw(SmallButton *smallButton, int x, int y);
void ItemPage_Draw(Page *page, int x, int y);
void ItemTime_Draw(Time *time, int x, int y, bool opened);
