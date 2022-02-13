// 2022/02/11 17:44:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct ProgressBar
{
    int x;
    int y;
    int width;
    int height;
    float fullTime;
    float passedTime;
};


void ProgressBar_Draw(ProgressBar *bar);
