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
    uint startTicks = TIME_TICKS;
    while (TIME_TICKS - startTicks < numTicks) {};
}


uint Timer::GetMS()
{
    return HAL_GetTick();
}


uint Timer::GetTicks()
{
    return TIM2->CNT;
}


void Timer::StartMultiMeasurement()
{
    TIM2->CR1 &= (uint16)~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN; 
}


void Timer::StartLogging()
{
    timeStartLogging = TIME_TICKS;
    timePrevPoint = timeStartLogging;
}


uint Timer::LogPointUS(char *name)
{
    uint interval = TIME_TICKS - timePrevPoint;
    timePrevPoint = TIME_TICKS;
    LOG_WRITE("%s %.2f us", name, interval / 120.0f);
    return interval;
}


uint Timer::LogPointMS(char *name)
{
    uint interval = TIME_TICKS - timePrevPoint;
    timePrevPoint = TIME_TICKS;
    LOG_WRITE("%s %.2f ms", name, interval / 120e3);
    return interval;
}


void Timer::Enable(TypeTimer::E type, int timeInMS, pFuncVV eF)
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
    Disable(type);
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


Waiter::Waiter()
{
    Reset();
}


void Waiter::Reset()
{
    time_start = TIME_MS;
}


uint Waiter::ElapsedTime()
{
    return TIME_MS - time_start;
}
