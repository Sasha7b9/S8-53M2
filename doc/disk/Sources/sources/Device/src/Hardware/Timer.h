// 2022/02/11 17:52:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"


#define TIME_MS     Timer::GetMS()
#define TIME_TICKS  Timer::GetTicks()
#define TICKS_IN_US 60


struct TypeTimer
{
    enum E
    {
        ShowLevelRShiftA,       // ����� �������������, ����� ���������� ��������� ����� �������� ������ 1.
        ShowLevelRShiftB,       // ����� �������������, ����� ���������� ��������� ����� �������� ������ 2.
        ShowLevelTrigLev,       // ����� �������������, ����� ���������� ��������� ����� ������ �������������
        NumSignalsInSec,        // ��� ��������� ���������� ���������� ������� � �������.
        FlashDisplay,           // ������ ��� ��������� �������� ������ �����-������ �������.
        P2P,                    // ������ ��� ������ ������ � ������ ����������� ������.
        ShowMessages,           // ������ ��� ��������� ����� ������ �������������� � ��������������� ���������.
        MenuAutoHide,           // ������ ��� ������� ������� ��������� ����.
        RShiftMarkersAutoHide,  // ������ ��� ������� ������� ��������� �������������� ������� �����.
        Temp,                   // ���������������, ��� ����� ����.
        StopSound,              // ��������� ����
        UpdateDisplay,          // ���� ������ ����� ������������ �������, ������������ ���� ��������� ���������
        CDC,                    // ��������������� ������ ��� CDC
        DisableTrigLED,         // ������ ��� ���������� �������� �������������
        Count                   // ����� ���������� ��������.
    };
};


namespace Timer
{
    void Init();

    // �������� �� timeMS �����������
    void PauseOnTime(uint timeMS);

    void PauseOnTicks(uint numTicks);

    uint GetMS();

    // ���������� �����, ��������� � ������� ���������� ������ ������� Timer_StartMultiMeasurement().
    // � ����� ������� 120.000.000 �����.������������ ������� �������, ������� ����� ��������� � � ������� - 35 ���.
    // ���������� �����, ��������� � ������� ���������� ������ ������� Timer_StartMultiMeasurement().�� �����(1 << 32).
    uint GetTicks();

    // ������� ���������� �� ���������� ���������� ������� ��� ������ ��������. ����������� ��������� ����� �������� �� 1��.
    void Update1ms();

    // ������� ���������� �� ���������� ���������� ������� ��� ������ ��������. ����������� ���������� ����� �������� �� 10��.
    void Update10ms();

    void Enable(TypeTimer::E, int timeInMS, pFuncVV);

    // ���������� ������.
    void Disable(TypeTimer::E);

    // ������������� ������. ��������� ����� ����� �������� Timer_Continue().
    void Pause(TypeTimer::E);

    // ���������� ������ �������, ����� ����������������� �������� Timer_Pause().
    void Continue(TypeTimer::E);

    // � ������� ���� ������� ����� ������, �������� �� ������.
    bool IsRun(TypeTimer::E);

    // ��������� ������� ��� ��������� ����� �������� �������.
    void StartMultiMeasurement();

    // ������������� ��������� ����� ������������. ����� ������ Timer_LogPoint �������� ��������� ��������� �� ��� �����.
    void StartLogging();

    uint LogPointUS(char *name);

    uint LogPointMS(char *name);
};


// ���������� �������� �������
struct TimeMeterUS
{
    TimeMeterUS();

    void Reset();

    uint ElapsedUS();

    uint ElapsedTicks();

private:

    uint ticks_reset;
};


// ��������� ��� ������� �������
struct TimeMeterMS
{
    TimeMeterMS();

    // ���������� ������ �������
    void Reset();

    void Pause();

    void Continue();

    // ������� ����������� ������ � ������� ������ Reset()
    uint ElapsedTime();

private:

    uint time_reset;        // �� ����� ������� ������������� ElapsedTime()
    uint time_pause;        // � ���� ������ ��������� �� �����
};
