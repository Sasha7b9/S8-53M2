// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"


// Вызывается один раз при обнаружении новой флешки.
namespace FM
{
    void Init();

    void Draw();

    void PressLevelUp();

    void PressLevelDown();

    void RotateRegSet(int angle);

    bool GetNameForNewFile(char name[255]);

    void PressTab();
};
