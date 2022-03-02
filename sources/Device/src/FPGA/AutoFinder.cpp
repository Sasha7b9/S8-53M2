// 2022/02/21 12:24:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


namespace FPGA
{
    void LoadSettings();

    namespace AutoFinder
    {
        void AutoFind();

        bool FindWave(Chan::E ch);

        TBase::E FindTBase(Chan::E);

        Range::E AccurateFindRange(Chan::E);

        TBase::E AccurateFindTBase(Chan::E);

        uint8 CalculateMinWithout0(uint8 buffer[100]);

        uint8 CalculateMaxWithout255(uint8 buffer[100]);

        TBase::E CalculateTBase(float freq_);

        float CalculateFreqFromCounterFreq();

        float CalculateFreqFromCounterPeriod();
    }

    namespace FreqMeter
    {
        extern float freq;           // Частота, намеренная альтерой.

        BitSet32 ReadRegFreq();

        BitSet32 ReadRegPeriod();

        float FreqCounterToValue(BitSet32 *fr);

        float PeriodCounterToValue(BitSet32 *period);
    }
}


void FPGA::AutoFinder::AutoFind()
{
    if (!FindWave(Chan::A))
    {
        if (!FindWave(Chan::B))
        {
            Display::ShowWarningBad(Warning::SignalNotFound);
        }
    }

    Init();
    Start();

    AUTO_FIND_IN_PROGRESS = false;
}



bool FPGA::AutoFinder::FindWave(Chan::E ch)
{
    Settings settings = set;    // Сохраняем предыдущие настройки

    Stop(false);
    SET_ENABLED(ch) = true;
    TrigSource::Set((TrigSource::E)ch);
    TrigLev::Set((TrigSource::E)ch, TrigLev::ZERO);
    RShift::Set(ch, RShift::ZERO);
    ModeCouple::Set(ch, ModeCouple::AC);
    Range::E range = AccurateFindRange(ch);

    if (range != Range::Count)
    {
        SET_RANGE(ch) = range;
        TBase::E tBase = AccurateFindTBase(ch);
        if (tBase != TBase::Count)
        {
            SET_TBASE = tBase;
            TRIG_SOURCE = (TrigSource::E)ch;
            return true;
        }
    }

    set = settings;
    FPGA::LoadSettings();
    return false;
}


Range::E FPGA::AutoFinder::AccurateFindRange(Chan::E ch)
{
    /*
    Алгоритм поиска.
    Устанавливаем развёртку 50 мс.
    Устанавливаем закрытый вход.
    Включаем пиковый детектор.
    1. Устанавливаем растяжку 20В/дел.
    Не дожидаясь СИ, начинаем считывать точки в поточечном режиме и считываем их 50 штук - 2 клетки с небольшим. За это время
        на экране уложится чуть более одного периода частоты 10 Гц.
    Проверяем считанную информацию на выход за пределы экрана. Если вышла - берём предыдущую растяжку (например, 2В вместо 1В) и возвращаем
        её в качестве результата.
    Если не вышла - берём следующую растяжку и повторяем шаги с 1.
    */

    uint8 buffer[100];  // Сюда будем считывать точки

    TBase::Set(TBase::_50ms);
    ModeCouple::Set(ch, ModeCouple::AC);
    PeackDetMode::E peackDetMode = SET_PEAKDET;
    PeackDetMode::Set(PeackDetMode::Enable);

    for (int range = Range::Count - 1; range >= 0; range--)
    {
        FPGA::Stop(false);
        Range::Set(ch, (Range::E)range);
        Timer::PauseOnTime(10);
        FPGA::Start();

        for (int i = 0; i < 50; i++)
        {
            while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_POINT) == 0) {};

            HAL_FMC::Read(RD_ADC_A);
            HAL_FMC::Read(RD_ADC_B);
        }

        BitSet16 data;

        if (ch == Chan::A)
        {
            for (int i = 0; i < 100; i += 2)
            {
                while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_POINT) == 0) {};

                HAL_FMC::Read(RD_ADC_B);

                data.half_word = *RD_ADC_A;
                buffer[i] = data.byte0;
                buffer[i + 1] = data.byte1;
            }
        }
        else
        {
            for (int i = 0; i < 100; i += 2)
            {
                while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_POINT) == 0) {};

                data.half_word = *RD_ADC_B;

                buffer[i] = data.byte0;
                buffer[i + 1] = data.byte1;

                HAL_FMC::Read(RD_ADC_A);
            }
        }

        if (CalculateMinWithout0(buffer) < ValueFPGA::MIN || CalculateMaxWithout255(buffer) > ValueFPGA::MAX)
        {
            if (range < Range::_20V)
            {
                range++;
            }
            PeackDetMode::Set(peackDetMode);
            return (Range::E)range;
        }

        uint8 min = ValueFPGA::AVE - 30;
        uint8 max = ValueFPGA::AVE + 30;

        if (range == Range::_2mV && CalculateMinWithout0(buffer) > min && CalculateMaxWithout255(buffer) < max)
        {
            PeackDetMode::Set(peackDetMode);
            return Range::Count;
        }
    }

    PeackDetMode::Set(peackDetMode);
    return Range::Count;
}


TBase::E FPGA::AutoFinder::AccurateFindTBase(Chan::E ch)
{
    for (int i = 0; i < 5; i++)
    {
        TBase::E tBase = FindTBase(ch);
        TBase::E secondTBase = FindTBase(ch); //-V656

        if (tBase == secondTBase && tBase != TBase::Count)
        {
            return tBase;
        }
    }
    return TBase::Count;
}


TBase::E FPGA::AutoFinder::FindTBase(Chan::E ch)
{
    TrigInput::Set(TrigInput::Full);
    Timer::PauseOnTime(10);
    FPGA::Stop(false);
    float fr = CalculateFreqFromCounterFreq();

    TrigInput::Set(fr < 1e6f ? TrigInput::LPF : TrigInput::Full);

    fr = CalculateFreqFromCounterFreq();

    TBase::E tBase = TBase::Count;

    if (fr >= 50.0f)
    {
        tBase = CalculateTBase(fr);
        TBase::Set(tBase);
        FPGA::Start();
        TrigInput::Set(fr < 500e3f ? TrigInput::LPF : TrigInput::HPF);
        return tBase;
    }
    else
    {
        TrigInput::Set(TrigInput::LPF);
        FreqMeter::freq = CalculateFreqFromCounterPeriod();

        if (fr > 0.0f)
        {
            tBase = CalculateTBase(fr);
            TBase::Set(tBase);
            Timer::PauseOnTime(10);
            FPGA::Start();
            return tBase;
        }
    }

    return TBase::Count;
}


TBase::E FPGA::AutoFinder::CalculateTBase(float freq_)
{
    if (freq_ >= 100e6f)        { return TBase::_2ns; }
    else if (freq_ >= 40e6f)    { return TBase::_5ns; }
    else if (freq_ >= 20e6f)    { return TBase::_10ns; }
    else if (freq_ >= 10e6f)    { return TBase::_20ns; }
    else if (freq_ >= 3e6f)     { return TBase::_50ns; }
    else if (freq_ >= 2e6f)     { return TBase::_100ns; }
    else if (freq_ >= 900e3f)   { return TBase::_200ns; }
    else if (freq_ >= 400e3f)   { return TBase::_500ns; }
    else if (freq_ >= 200e3f)   { return TBase::_1us; }
    else if (freq_ >= 90e3f)    { return TBase::_2us; }
    else if (freq_ >= 30e3f)    { return TBase::_5us; }
    else if (freq_ >= 20e3f)    { return TBase::_10us; }
    else if (freq_ >= 10e3f)    { return TBase::_20us; }
    else if (freq_ >= 4e3f)     { return TBase::_50us; }
    else if (freq_ >= 2e3f)     { return TBase::_100us; }
    else if (freq_ >= 1e3f)     { return TBase::_200us; }
    else if (freq_ >= 350.0f)   { return TBase::_500us; }
    else if (freq_ >= 200.0f)   { return TBase::_1ms; }
    else if (freq_ >= 100.0f)   { return TBase::_2ms; }
    else if (freq_ >= 40.0f)    { return TBase::_5ms; }
    else if (freq_ >= 20.0f)    { return TBase::_10ms; }
    else if (freq_ >= 10.0f)    { return TBase::_20ms; }
    else if (freq_ >= 4.0f)     { return TBase::_50ms; }
    else if (freq_ >= 2.0f)     { return TBase::_100ms; }
    return TBase::_200ms;
}


uint8 FPGA::AutoFinder::CalculateMinWithout0(uint8 buffer[100])
{
    // \todo На одном экземпляре был страшенныый глюк, когда без сигнала выбивались значения 0 и 255 в рандомных местах
    // Вот такой кастыиль на скорую ногу, чтобы нули выкинуть.
    uint8 min = 255;

    for (int i = 1; i < 100; i++)
    {
        if (buffer[i] > 0 && buffer[i] < min)
        {
            min = buffer[i];
        }
    }
    return min;
}


uint8 FPGA::AutoFinder::CalculateMaxWithout255(uint8 buffer[100])
{
    // \todo На одном экземпляре был страшенныый глюк, когда без сигнала выбивались значения 0 и 255 в рандомных местах
    // Вот такой кастыиль на скорую ногу, чтобы нули выкинуть.
    uint8 max = 0;
    for (int i = 1; i < 100; i++)
    {
        if (buffer[i] < 255 && buffer[i] > max)
        {
            max = buffer[i];
        }
    }
    return max;
}


void FPGA::AutoFinder::StartAutoFind()
{
    AUTO_FIND_IN_PROGRESS = true;
}


float FPGA::AutoFinder::CalculateFreqFromCounterFreq()
{
    while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_FREQ) == 0) {};
    FreqMeter::ReadRegFreq();

    while (_GET_BIT(HAL_FMC::Read(RD_FL), FL_FREQ) == 0) {};
    BitSet32 fr = FreqMeter::ReadRegFreq();

    if (fr.word >= 5)
    {
        return FreqMeter::FreqCounterToValue(&fr);
    }

    return 0.0f;
}


float FPGA::AutoFinder::CalculateFreqFromCounterPeriod()
{
    uint time = TIME_MS;

    while (TIME_MS - time < 1000 && _GET_BIT(HAL_FMC::Read(RD_FL), FL_PERIOD) == 0) {};
    FreqMeter::ReadRegPeriod();

    time = TIME_MS;

    while (TIME_MS - time < 1000 && _GET_BIT(HAL_FMC::Read(RD_FL), FL_PERIOD) == 0) {};
    BitSet32 period = FreqMeter::ReadRegPeriod();

    if (period.word > 0 && (TIME_MS - time < 1000))
    {
        return FreqMeter::PeriodCounterToValue(&period);
    }

    return 0.0f;
}
