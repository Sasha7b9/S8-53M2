// 2022/02/11 17:44:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


typedef struct
{
    int x;
    int y;
    int width;
    int height;
    float fullTime;
    float passedTime;
} ProgressBar;


void ProgressBar_Draw(ProgressBar *bar);
