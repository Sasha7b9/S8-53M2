// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Settings.h"
#include "SettingsService.h"


float sService_MathGetFFTmaxDBabs()
{
    static const float db[] = {-40.0f, -60.0f, -80.0f};
    return db[FFT_MAX_DB];
}
