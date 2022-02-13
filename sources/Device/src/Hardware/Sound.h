// 2022/02/11 17:51:45 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


enum TypeWave
{
    TypeWave_Sine,
    TypeWave_Meandr,
    TypeWave_Triangle
};


namespace Sound
{
    void Init();

    void ButtonPress();
    // Функция вызовет звук отпускаемой кнопки только если перед этим проигрывался звук нажатия кнопки.
    void ButtonRelease();

    void GovernorChangedValue();

    void RegulatorShiftRotate();

    void RegulatorSwitchRotate();

    void WarnBeepBad();

    void WarnBeepGood();

    extern void *handle;       // DAC_HandleTypeDef
};
