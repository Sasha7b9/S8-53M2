// 2022/02/11 17:51:11 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Sound.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Settings/Settings.h"
#include <stm32f429xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#include <stm32f4xx_hal_dac.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_dma.h>
#include <stm32f4xx_hal_dma_ex.h>
#include <math.h>



namespace Sound
{
    static DAC_HandleTypeDef handleDAC =
    {
        DAC
    };

    void *handle = &handleDAC;

    void TIM7_Config(uint16 prescaler, uint16 period);
    uint16 CalculatePeriodForTIM();
    void SetWave();
    void CalculateSine();
    void CalculateMeandr();
    void CalculateTriangle();

    void Stop();
    void Beep(TypeWave typeWave_, float frequency_, float amplitude_, int duration);
    void SetWave();
    uint16 CalculatePeriodForTIM();
    void CalculateSine();
    void CalculateMeandr();
    void CalculateTriangle();
    void TIM7_Config(uint16 prescaler, uint16 period);

    const int POINTS_IN_PERIOD = 10;
    uint8 points[POINTS_IN_PERIOD] = {0};
    float frequency = 0.0f;
    float amplitude = 0.0f;
    TypeWave typeWave = TypeWave_Sine;
}


void Sound::Init()
{
    DAC_ChannelConfTypeDef config =
    {
        DAC_TRIGGER_T7_TRGO,
        DAC_OUTPUTBUFFER_ENABLE
    };

    HAL_DAC_DeInit(&handleDAC);

    HAL_DAC_Init(&handleDAC);

    HAL_DAC_ConfigChannel(&handleDAC, &config, DAC_CHANNEL_1);
}


void Sound::Stop()
{
    HAL_DAC_Stop_DMA(&handleDAC, DAC_CHANNEL_1);
    SOUND_IS_BEEP = 0;
    SOUND_WARN_IS_BEEP = 0;
}


void Sound::Beep(TypeWave typeWave_, float frequency_, float amplitude_, int duration)
{
    if (SOUND_WARN_IS_BEEP)
    {
        return;
    }
    if (!SOUND_ENABLED)
    {
        return;
    }
    if (frequency != frequency_ || amplitude != amplitude_ || typeWave != typeWave_)
    {
        frequency = frequency_;
        amplitude = amplitude_;
        typeWave = typeWave_;
        
        Stop();
        SetWave();
    }

    Stop();
    
    SOUND_IS_BEEP = 1;
    HAL_DAC_Start_DMA(&handleDAC, DAC_CHANNEL_1, (uint32_t*)points, POINTS_IN_PERIOD, DAC_ALIGN_8B_R); //-V641 //-V1032

    Timer::Enable(TypeTimer::StopSound, duration, Stop);
}


void Sound::ButtonPress()
{
    return;
    Beep(TypeWave_Sine, 2000.0f, 0.5f, 50);
    BUTTON_IS_PRESSED = 1;
}


void Sound::ButtonRelease()
{
    return;
    if (BUTTON_IS_PRESSED)
    {
        Beep(TypeWave_Sine, 1000.0f, 0.25f, 50);
        BUTTON_IS_PRESSED = 0;
    }
}


void Sound::GovernorChangedValue()
{
    Beep(TypeWave_Sine, 1000.0f, 0.5f, 50);
    BUTTON_IS_PRESSED = 0;
}


void Sound::RegulatorShiftRotate()
{
    Beep(TypeWave_Sine, 1.0f, 0.35f, 3);
    BUTTON_IS_PRESSED = 0;
}


void Sound::RegulatorSwitchRotate()
{
    Beep(TypeWave_Triangle, 2500.0f, 0.5f, 25);
    BUTTON_IS_PRESSED = 0;
}


void Sound::WarnBeepBad()
{
    Beep(TypeWave_Meandr, 250.0f, 1.0f, 500);
    SOUND_WARN_IS_BEEP = 1;
    BUTTON_IS_PRESSED = 0;
}


void Sound::WarnBeepGood()
{
    Beep(TypeWave_Triangle, 1000.0f, 0.5f, 250);
    BUTTON_IS_PRESSED = 0;
}


void Sound::SetWave()
{
    TIM7_Config(0, CalculatePeriodForTIM());

    if(typeWave == TypeWave_Sine)
    {
        CalculateSine();
    }
    else if(typeWave == TypeWave_Meandr)
    {
        CalculateMeandr();
    }
    else if(typeWave == TypeWave_Triangle)
    {
        CalculateTriangle();
    }
}


void Sound::TIM7_Config(uint16 prescaler, uint16 period)
{
    static TIM_HandleTypeDef htim =
    {
        TIM7,
        {
            0,
            TIM_COUNTERMODE_UP,
        }
    };

    htim.Init.Prescaler = prescaler;
    htim.Init.Period = period;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    HAL_TIM_Base_Init(&htim);

    TIM_MasterConfigTypeDef masterConfig =
    {
        TIM_TRGO_UPDATE,
        TIM_MASTERSLAVEMODE_DISABLE
    };

    HAL_TIMEx_MasterConfigSynchronization(&htim, &masterConfig);

    HAL_TIM_Base_Stop(&htim);
    HAL_TIM_Base_Start(&htim);
}


uint16 Sound::CalculatePeriodForTIM()
{
    return 120e6f / frequency / POINTS_IN_PERIOD;
}


void Sound::CalculateSine()
{
    for (int i = 0; i < POINTS_IN_PERIOD; i++)
    {
        float step = 2.0f * 3.1415926f / (POINTS_IN_PERIOD - 1);
        float value = (sin(i * step) + 1.0f) / 2.0f;
        points[i] = value * amplitude * 255;
    }
}


void Sound::CalculateMeandr()
{
    for (int i = 0; i < POINTS_IN_PERIOD / 2; i++)
    {
        points[i] = 255 * amplitude;
    }
    for (int i = POINTS_IN_PERIOD / 2; i < POINTS_IN_PERIOD; i++)
    {
        points[i] = 0;
    }
}


void Sound::CalculateTriangle()
{
    float k = 255.0 / POINTS_IN_PERIOD;
    for (int i = 0; i < POINTS_IN_PERIOD; i++)
    {
        points[i] = k * i * amplitude;
    }
}
