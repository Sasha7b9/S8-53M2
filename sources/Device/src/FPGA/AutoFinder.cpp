// 2022/02/21 12:24:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"


namespace FPGA
{
    namespace AutoFinder
    {
        static bool FindWave(Chan);

        static Range::E FindRange(Chan);

        static bool AccurateFindParams(Chan);

        static bool ReadingCycle(uint timeWait);
    }
}


void FPGA::AutoFinder::FindSignal()
{
    /*
        �������� ������.
        1. ������������� �������� ���� �� ������, ������������� - ��.
        2. ������������� �������� 20��/���, �������� - 2��/���.
        3. ��������� ���.
        4. ��� �������������. ���� ������������� ��� - ��������� �� ������ �����. ���� ����:
        5. �������� ������� ������� � ��������� ������. ���� ��������� ������ �������� �� ������ ������ 2 ������ -
            ��������� � ������ �� ������� ������. ���� �������� ������ 2 ������:
        6. ������������� ������������ ������� 5��, 10��, 20�� � �����, ���� ��������� ������ ��������� �� �������� �
            �����.
        7. ��������� � ������ TBase.
    */

    /** \todo �������������� �������� � ����� ��������.\n
                1. ������� ������� ����� ����� ������� �������������.\n
                2. ����� ������������� ����� ��������, ����� �� ����� ������ ����������� �� ����� ���� ��������� �������������.\n
                3. ��� �������� ������� ��������� � ������������ ������.
    */

    Settings old = set;

    if (!FindWave(ChA))
    {
        if (!FindWave(ChB))
        {
            Display::ShowWarningBad(Warning::SignalNotFound);
            set = old;
            set.RunAfterLoad();
            FPGA::Init();
        }
    }

    FPGA::Start();
}


static bool FPGA::AutoFinder::FindWave(Chan ch)
{
    TBase::Set((TBase::E)(TBase::MIN_P2P - 1));
    SET_ENABLED(ch) = true;
    TrigSource::Set(ch);
    TrigLev::Set(ch, TrigLev::ZERO);
    RShift::Set(ch, RShift::ZERO);
    ModeCouple::Set(ch, ModeCouple::AC);
    TrigInput::Set(TrigInput::Full);

    Range::E range = FindRange(ch);

    if (range == Range::Count)
    {
        return false;
    }

    Range::Set(ch, range);

    return AccurateFindParams(ch);
}


static Range::E FPGA::AutoFinder::FindRange(Chan ch)
{
    PeackDetMode::E peackDet = SET_PEAKDET;
    TPos::E tPos = SET_TPOS;
    Range::E oldRange = SET_RANGE(ch);

    START_MODE = StartMode::Wait;                // ������������� ������ ����� �������������, ���� ������, ���� �� ������

    Stop(false);

    PeackDetMode::Set(PeackDetMode::Enable);
    Range::Set(ch, Range::_2mV);
    TPos::Set(TPos::Left);

    int range = Range::Count;

    ReadingCycle(1000);

    if (ReadingCycle(2000))                                 // ���� � ������� 2 ������ �� ������ ������, �� ��� ��� �� ���� ������ - �������
    {
        uint8 min = 0;
        uint8 max = 0;

        uint8 bound = GetBound(Storage::GetData_RAM(ch, 0), &min, &max);

        if (bound > (MAX_VALUE - MIN_VALUE) / 10.0 * 2)      // ���� ������ ������� ������ ���� ������ - ���� �������
        {
            START_MODE = StartMode_Auto;

            for (range = 0; range < RangeSize; ++range)
            {
                /// \todo ���� �������� ���������� ��������� "������ �� ������", ���� ��� (range == RangeSize - 1) ������ ������� �� ������� ������

                SetRange(ch, (Range)range);

                ReadingCycle(10000);

                GetBound(Storage::GetData_RAM(ch, 0), &min, &max);

                if (min > MIN_VALUE && max < MAX_VALUE)     // ���� ��� �������� ������ ������
                {
                    break;                                  // �� �� ����� ��� Range - ������� �� �����
                }
            }
        }
    }

    SetRange(ch, oldRange);
    SetPeackDetMode(peackDet);
    SetTPos(tPos);

    return (Range)range;
}


static bool FPGA::AutoFinder::AccurateFindParams(Chan ch)
{
    TBase tBase = TBaseSize;
    FindParams(ch, &tBase);

    return true;
}


bool FPGA::AutoFinder::ReadingCycle(uint timeWait)
{
    Start();
    uint timeStart = HAL_GetTick();
    while (!ProcessingData())
    {
        FuncDrawAutoFind();
        if ((HAL_GetTick() - timeStart) > timeWait)
        {
            Stop(false);

            return false;
        }
    };
    Stop(false);

    return true;
}
