// 2022/02/11 17:49:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


class Clock
{
public:
    static void Init();
    static bool SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);
    static PackedTime GetPackedTime();

private:
    static const RTC_HandleTypeDef handle;
};
