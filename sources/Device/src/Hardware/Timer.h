// 2022/02/11 17:52:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "Utils/GlobalFunctions.h"


enum TypeTimer
{
    kPressKey,                  // ����� �������������, ����� �������� ������� �������.
    kShowLevelRShift0,          // ����� �������������, ����� ���������� ��������� ����� �������� ������ 1.
    kShowLevelRShift1,          // ����� �������������, ����� ���������� ��������� ����� �������� ������ 2.
    kShowLevelTrigLev,          // ����� �������������, ����� ���������� ��������� ����� ������ �������������
    kNumSignalsInSec,           // ��� ��������� ���������� ���������� ������� � �������.
    kFlashDisplay,              // ������ ��� ��������� �������� ������ �����-������ �������.
    kP2P,                       // ������ ��� ������ ������ � ������ ����������� ������.
    kShowMessages,              // ������ ��� ��������� ����� ������ �������������� � ��������������� ���������.
    kMenuAutoHide,              // ������ ��� ������� ������� ��������� ����.
    kRShiftMarkersAutoHide,     // ������ ��� ������� ������� ��������� �������������� ������� �����.
    kTemp,                      // ���������������, ��� ����� ����.
    kStopSound,                 // ��������� ����
    kTemporaryPauseFPGA,        // ��������� ����� ��� �������� ������� �� ������ ����� �������� �����
    kTimerDrawHandFunction,     // ���� ������ ����� ������������ �������, ������������ ���� ��������� ���������
    TypeTimerSize               // ����� ���������� ��������.
};


namespace Timer
{
    // �������� �� timeMS �����������
    void PauseOnTime(uint timeMS);

    void PauseOnTicks(uint numTicks);

    // ������� ���������� �� ���������� ���������� ������� ��� ������ ��������. ����������� ��������� ����� �������� �� 1��.
    void Update1ms();

    // ������� ���������� �� ���������� ���������� ������� ��� ������ ��������. ����������� ���������� ����� �������� �� 10��.
    void Update10ms();

    void Enable(TypeTimer type, int timeInMS, pFuncVV);

    // ���������� ������.
    void Disable(TypeTimer type);

    // ������������� ������. ��������� ����� ����� �������� Timer_Continue().
    void Pause(TypeTimer type);

    // ���������� ������ �������, ����� ����������������� �������� Timer_Pause().
    void Continue(TypeTimer type);

    // � ������� ���� ������� ����� ������, �������� �� ������.
    bool IsRun(TypeTimer type);

    // ��������� ������� ��� ��������� ����� �������� �������.
    void StartMultiMeasurement();

    // ������������� ��������� ����� ������������. ����� ������ Timer_LogPoint �������� ��������� ��������� �� ��� �����.
    void StartLogging();

    uint LogPointUS(char *name);

    uint LogPointMS(char *name);
};

#define gTimerMS HAL_GetTick()

/**
  * gTimerTics - ���������� �����, ��������� � ������� ���������� ������ ������� Timer_StartMultiMeasurement().
  * � ����� ������� 120.000.000 �����. ������������ ������� �������, ������� ����� ��������� � � ������� - 35 ���.
  * ���������� �����, ��������� � ������� ���������� ������ ������� Timer_StartMultiMeasurement(). �� ����� (1 << 32).
***/
#define gTimerTics (TIM2->CNT)
