// 2022/02/11 17:51:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Timer.h"
#include "Log.h"
#include <stm32f4xx_hal.h>


namespace Timer
{
    void (*f[TypeTimer::Count])() = {0};
    int  reactionTimeMS[TypeTimer::Count] = {0};
    int  currentTimeMS[TypeTimer::Count] = {0};
    bool isRun[TypeTimer::Count] = {false};

    uint timeStartLogging = 0;
    uint timePrevPoint = 0;

    void Pause(TypeTimer);
    void Continue(TypeTimer);
    bool IsRun(TypeTimer);
}


void Timer::PauseOnTime(uint timeMS)
{
    HAL_Delay(timeMS);
};


void Timer::PauseOnTicks(uint numTicks)
{
    uint startTicks = gTimerTics;
    while (gTimerTics - startTicks < numTicks) {};
}


void Timer::StartMultiMeasurement()
{
    TIM2->CR1 &= (uint16)~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN; 
}


void Timer::StartLogging()
{
    timeStartLogging = gTimerTics;
    timePrevPoint = timeStartLogging;
}


uint Timer::LogPointUS(char *name)
{
    uint interval = gTimerTics - timePrevPoint;
    timePrevPoint = gTimerTics;
    LOG_WRITE("%s %.2f us", name, interval / 120.0f);
    return interval;
}


uint Timer::LogPointMS(char *name)
{
    uint interval = gTimerTics - timePrevPoint;
    timePrevPoint = gTimerTics;
    LOG_WRITE("%s %.2f ms", name, interval / 120e3);
    return interval;
}


void Timer::Enable(TypeTimer::E type, int timeInMS, void(*eF)())
{
    f[type] = eF;
    reactionTimeMS[type] = timeInMS;
    currentTimeMS[type] = timeInMS - 1;
    isRun[type] = true;
}


void Timer::Disable(TypeTimer::E type)
{
    isRun[type] = false;
}


void Timer::Pause(TypeTimer::E type)
{
    isRun[type] = false;
}


void Timer::Continue(TypeTimer::E type)
{
    isRun[type] = true;
}


bool Timer::IsRun(TypeTimer::E type)
{
    return isRun[type];
};


void Timer::Update1ms()
{
    for(int num = 0; num < TypeTimer::Count; num++)
    {
        if(isRun[num])
        {
            currentTimeMS[num]--;
            if(currentTimeMS[num] < 0)
            {
                f[num]();
                currentTimeMS[num] = reactionTimeMS[num] - 1;
            }
            
        }
    }
}


void Timer::Update10ms()
{
    for(int num = 0; num < TypeTimer::Count; num++)
    {
        if(isRun[num])
        {
            currentTimeMS[num] -= 10;
            if(currentTimeMS[num] < 0)
            {
                f[num]();
                currentTimeMS[num] = reactionTimeMS[num] - 1;
            }

        }
    }
}
