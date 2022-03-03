// 2022/02/11 17:44:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/DisplayPrimitives.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include <stdio.h> 


void ProgressBar_Draw(ProgressBar *bar)
{
    int x = bar->x;
    int y = bar->y;
    char buffer[100] = {0};
    float passedPercents = bar->fullTime == 0 ? 0 : bar->passedTime / bar->fullTime * 100;
    sprintf(buffer, "Завершено %.1f %%", passedPercents);
    PText::DrawStringInCenterRect(x, y - 15, bar->width, bar->height, buffer, COLOR_FILL);
    Painter::DrawRectangle(bar->x, bar->y, bar->width, bar->height);
    Painter::FillRegion(bar->x, bar->y, (int)(((float)bar->width * passedPercents) / 100.0f), bar->height);
    buffer[0] = 0;
    sprintf(buffer, "Осталось %.1f с", (bar->fullTime - bar->passedTime) / 1000.0f);
    PText::DrawStringInCenterRect(x, y + bar->height, bar->width, bar->height, buffer);
}
