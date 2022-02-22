// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "SettingsTypes.h"
#include "defines.h"
#include "FPGA/SettingsFPGA.h"


#define SET_TBASE                           (set.time.tBase)        // SettingTime.tBase

#define TSHIFT                              (set.time.tShiftRel)    // SettingsTime.tShiftRel

#define TIME_DIV_XPOS                       (set.time.timeDivXPos)  // SettingsTime.timeDivXPos
#define TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY    (TIME_DIV_XPOS == FunctionTime_ShiftInMemory)

#define SET_TPOS                            (set.time.tPos)         // SettingsTime.tPos
#define SET_TPOS_IS_LEFT                    (SET_TPOS == TPos::Left)
#define SET_TPOS_IS_CENTER                  (SET_TPOS == TPos::Center)
#define SET_TPOS_IS_RIGHT                   (SET_TPOS == TPos::Right)

#define SAMPLE_TYPE                         (set.time.sampleType)   // SettingsTime.sampleType
#define SAMPLE_TYPE_IS_EQUAL                (SAMPLE_TYPE == SampleType_Equal)
#define SAMPLE_TYPE_IS_REAL                 (SAMPLE_TYPE == SampleType_Real)

#define PEAKDET                             (set.time.peakDet)      // SettingsTime.peakDet
#define PEAKDET_IS_DISABLE                  (PEAKDET == PeackDetMode::Disable)
#define PEAKDET_IS_ENABLE                   (PEAKDET == PeackDetMode::Enable)

#define SET_SELFRECORDER                    (set.time.selfRecorder) // SettingsTime.selfRecorder




// ������� ����� �����/���.
enum FunctionTime
{
    FunctionTime_Time,              // ����� ��������� ��������� �� �������.
    FunctionTime_ShiftInMemory      // ����� ��������� ������������ �� ������ �������� ������.
};


// ��� ������� ��� ������ �������������.
enum SampleType
{
    SampleType_Real,   // �������� ����� - � ���������� ��������� ������ ������� ��������� �����, ������ �� ��������������.
    SampleType_Equal   // ������������� - ������ �������� �� ��������� ������, ���������� �� �������������.
};


// ����� ����� �������, � ������� ��� ������.
struct ENUM_POINTS_FPGA
{
    enum E
    {
        _281,
        _512,
        _1024
    };

    static int ToNumPoints(bool forCalculate);

    static ENUM_POINTS_FPGA::E FromNumPoints(int numPoints);
};


// ��������� ��� X.
struct SettingsTime
{ //-V802
    TBase::E            tBase;          // ������� �� �������.
    int16               tShiftRel;      // �������� �� �������.
    FunctionTime        timeDivXPos;
    TPos::E             tPos;           // �������� ������������� � ������.
    SampleType          sampleType;     // ��� ������� ��� ������ �������������.
    PeackDetMode::E     peakDet;        // ����� ������ �������� ���������
    bool                selfRecorder;   // ������� �� ����� ���������.
    ENUM_POINTS_FPGA::E oldNumPoints;   // ����� ������������� � ����� �������� ���������, �������������
                                        // ���������� ����� � 1024, � ���� 
                                        // ���������� ��, ��� ����, ����� ����� ������������.
};
