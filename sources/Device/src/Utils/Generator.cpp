// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#include "Generator.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"


#include <stdlib.h>
#include <math.h>


static void SetParametersWave(Chan::E ch, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise);
static void StartNewWave(Chan::E ch);
static uint8 GetSampleWave(Chan::E ch);


const SGenerator Generator = 
{
    SetParametersWave,
    StartNewWave,
    GetSampleWave,
};


static float NewNoiseValue(Chan::E ch);
static uint8 GetSampleSinusWave(Chan::E ch, int numSample);
static uint8 GetSampleMeanderWave(Chan::E ch, int numSample);


static TypeWave type[2] = {Wave_Sinus, Wave_Meander};
static float freq[2] = {1000.0f, 500.0f};
static float angle[2] = {0.05f, 0.1f};
static float ampl[2] = {1.0f, 0.5f};
static float amplNoise[2] = {0.1f, 0.1f};
static int numSample[2] = {0, 0};

void SetParametersWave(Chan::E ch, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise_)
{
    type[ch] = typeWave;
    freq[ch] = frequency;
    angle[ch] = startAngle;
    ampl[ch] = amplWave;
    amplNoise[ch] = amplNoise_;
}

void StartNewWave(Chan::E ch)
{
    numSample[0] = numSample[1] = 0;
}

uint8 GetSampleWave(Chan::E ch)
{
    return (type[ch] == Wave_Sinus) ? GetSampleSinusWave(ch, (numSample[ch])++) : GetSampleMeanderWave(ch, (numSample[ch])++);
}

uint8 GetSampleSinusWave(Chan::E ch, int numSample_)
{
    float dT = numSample_ * TShift::ToAbs(1, SET_TBASE);
    float voltage = ampl[ch] * sin(2 * M_PI * freq[ch] * dT + angle[ch]) + NewNoiseValue(ch);
    return Math_VoltageToPoint(voltage, SET_RANGE(ch), SET_RSHIFT(ch));
}

uint8 GetSampleMeanderWave(Chan::E ch, int numSample_)
{
    return 0;
}

float NewNoiseValue(Chan::E ch)
{
    static float prevNoise[2] = {0.0f, 0.0f};            // Здесь хранятся значения шума из предыдущих точек, необходимые для расчёта шума в текущей точке.

    float noise = prevNoise[ch];

    float halfAmplNoiseAbs = ampl[ch] * amplNoise[ch] / 2.0f;

    float deltaRand = halfAmplNoiseAbs;

    noise += Math_RandFloat(-deltaRand, deltaRand);

    while (noise < -halfAmplNoiseAbs)
    {
        noise += Math_RandFloat(0, deltaRand * 2);
    }

    while (noise > halfAmplNoiseAbs)
    {
        noise -= Math_RandFloat(0, deltaRand * 2);
    }

    prevNoise[ch] = noise;

    return noise;
}
