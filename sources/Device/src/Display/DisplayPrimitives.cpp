// 2022/02/11 17:44:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/DisplayPrimitives.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Display/String.h"
#include <cstdio> 


void ProgressBar_Draw(const ProgressBar *bar)
{
    int x = bar->x;
    int y = bar->y;
    char buffer[100] = {0};
    float passedPercents = bar->fullTime == 0 ? 0 : bar->passedTime / bar->fullTime * 100;
    std::sprintf(buffer, "Завершено %.1f %%", passedPercents);
    String(buffer).DrawInCenterRect(x, y - 15, bar->width, bar->height, COLOR_FILL);
    Rectangle(bar->width, bar->height).Draw(bar->x, bar->y);
    Region((int)(((float)bar->width * passedPercents) / 100.0f), bar->height).Fill(bar->x, bar->y);
    buffer[0] = 0;
    std::sprintf(buffer, "Осталось %.1f с", (bar->fullTime - bar->passedTime) / 1000.0f);
    String(buffer).DrawInCenterRect(x, y + bar->height, bar->width, bar->height);
}
