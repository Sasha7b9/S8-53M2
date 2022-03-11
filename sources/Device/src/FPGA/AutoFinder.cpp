// 2022/02/21 12:24:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"


namespace FPGA
{
    namespace AutoFinder
    {
        // Нахоит сигнал на канале ch. Возвращает true, если сигнал найден и параметры сигнала в tBase, range
        static bool FindSignal(Chan, TBase::E *, Range::E *, ModeCouple::E *);
        static bool FullSearchSignal(Chan, const Settings *);

        // Отмасштабировать сигнал. Если onlyReduce - только сжать
        static void ScaleChannel(Chan, bool onlyReduce);

        // Находит частоту на канале ch
        static bool FindFrequency(Chan, float *outFreq, Range::E *, ModeCouple::E *);

        // Рассчитывает TBase, необходимый для отображения задданой частоты
        static TBase::E CalculateTBase(float frequency);
    }
}


void FPGA::AutoFinder::FindSignal()
{
    Settings old = set;

    if (!FullSearchSignal(ChA, &old))
    {
        if (!FullSearchSignal(ChB, &old))
        {
            Display::ShowWarningBad(Warning::SignalNotFound);

            set = old;
            FPGA::Init();
        }
    }
}


static bool FPGA::AutoFinder::FullSearchSignal(Chan ch, const Settings *old) //-V2506
{
    TBase::E tBase = TBase::Count;
    Range::E range = Range::Count;
    ModeCouple::E couple = ModeCouple::Count;

    if (FindSignal(ch, &tBase, &range, &couple))
    {
        set = *old;
        SET_TBASE = tBase;
        SET_RANGE(ch) = range;
        SET_COUPLE(ch) = couple;
        TrigSource::Set((TrigSource::E)ch.value);
        RShift::Set(ch, 0);
        TrigLev::Set((TrigSource::E)ch.value, 0);
        StartMode::Set(StartMode::Wait);

        FPGA::Init();

        ScaleChannel(ChA, ch == Chan::B);
        ScaleChannel(ChB, ch == Chan::A);

        FPGA::Init();

        return true;
    }

    return false;
}


static bool FPGA::AutoFinder::FindSignal(Chan ch, TBase::E *tBase, Range::E *outRange, ModeCouple::E *couple)
{
    float frequency = 0.0F;

    if (FindFrequency(ch, &frequency, outRange, couple))
    {
        *tBase = CalculateTBase(frequency);

        return true;
    }

    return false;
}


static bool FPGA::AutoFinder::FindFrequency(Chan ch, float *outFreq, Range::E *outRange, ModeCouple::E *couple)
{
    FPGA::Stop(false);
    ModeCouple::Set(ch, ModeCouple::AC);
    TrigSource::Set(ch);
    StartMode::Set(StartMode::Wait);
    TrigLev::Set(ch, 0);
    TBase::Set(TBase::_100ns);
    TShift::Set(0);
    RShift::Set(ch, 0);
    TrigInput::Set(TrigInput::Full);

    if (FindFrequencyForRanges(ch, 150, outFreq, outRange))
    {
        *couple = ModeCouple::AC;
        return true;
    }

    ModeCouple::Set(ch, ModeCouple::DC);

    if (FindFrequencyForRanges(ch, 1200, outFreq, outRange))
    {
        *couple = ModeCouple::DC;
        return true;
    }

    return false;
}
