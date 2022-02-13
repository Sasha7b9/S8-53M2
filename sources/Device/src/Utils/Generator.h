// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


enum TypeWave
{
    Wave_Sinus,
    Wave_Meander
};

struct SGenerator
{
    void(*SetParametersWave)(Chan::E, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise);    // numWave = 1, 2. Может быть до двух сигналов.
                                                                                                    // amplWave = 0.0f...1.0f, amplNoise = 0.0f...1.0f - относительно амплитуды сигнала.
    void(*StartNewWave)(Chan::E ch);
    uint8(*GetSampleWave)(Chan::E ch);
};

extern const SGenerator Generator;
