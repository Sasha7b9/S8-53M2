// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "SettingsTypes.h"
#include "defines.h"
#include "FPGA/SettingsFPGA.h"


#define SET_TBASE                            (set.time.tbase)

#define SET_TSHIFT                           (set.time.tshift_points)

#define SET_TIME_DIV_XPOS                    (set.time.timeDivXPos)
#define SET_TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY (SET_TIME_DIV_XPOS == FunctionTime::ShiftInMemory)

#define SET_TPOS                             (set.time.tPos)
#define SET_TPOS_IS_LEFT                     (SET_TPOS == TPos::Left)
#define SET_TPOS_IS_CENTER                   (SET_TPOS == TPos::Center)
#define SET_TPOS_IS_RIGHT                    (SET_TPOS == TPos::Right)

#define SET_SAMPLE_TYPE                      (set.time.sampleType)
#define SET_SAMPLE_TYPE_IS_EQUAL             (SET_SAMPLE_TYPE == SampleType::Equal)
#define SET_SAMPLE_TYPE_IS_REAL              (SET_SAMPLE_TYPE == SampleType::Real)

#define SET_PEAKDET                          (set.time.peakDet)
#define SET_PEAKDET_IS_DISABLED              (SET_PEAKDET == PeackDetMode::Disable)
#define SET_PEAKDET_IS_ENABLED               (SET_PEAKDET == PeackDetMode::Enable)

#define SET_SELFRECORDER                     (set.time.selfRecorder)




// ������� ����� �����/���.
struct FunctionTime
{
    enum E
    {
        Time,              // ����� ��������� ��������� �� �������.
        ShiftInMemory      // ����� ��������� ������������ �� ������ �������� ������.
    };
};


// ��� ������� ��� ������ �������������.
struct SampleType
{
    enum E
    {
        Real,   // �������� ����� - � ���������� ��������� ������ ������� ��������� �����, ������ �� ��������������.
        Equal   // ������������� - ������ �������� �� ��������� ������, ���������� �� �������������.
    };
};


// ��������� ��� X.
struct SettingsTime
{ //-V802
    TBase::E            tbase;          // ������� �� �������.
    int                 tshift_points;  // �������� �� ������� � ������
    FunctionTime::E     timeDivXPos;
    TPos::E             tPos;           // �������� ������������� � ������.
    SampleType::E       sampleType;     // ��� ������� ��� ������ �������������.
    PeackDetMode::E     peakDet;        // ����� ������ �������� ���������
    bool                selfRecorder;   // ������� �� ����� ���������.
};
