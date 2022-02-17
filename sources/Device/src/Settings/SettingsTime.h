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
#define SET_TPOS_IS_LEFT                    (SET_TPOS == TPos_Left)
#define SET_TPOS_IS_CENTER                  (SET_TPOS == TPos_Center)
#define SET_TPOS_IS_RIGHT                   (SET_TPOS == TPos_Right)

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

// �������� ������������� � ������.
enum TPos
{
    TPos_Left,      // ������������� ��������� � ������ ������.
    TPos_Center,    // ������������� ��������� � ������ ������.
    TPos_Right      // ������������� ��������� � ����� ������.
};

// ��� ������� ��� ������ �������������.
enum SampleType
{
    SampleType_Real,   // �������� ����� - � ���������� ��������� ������ ������� ��������� �����, ������ �� ��������������.
    SampleType_Equal   // ������������� - ������ �������� �� ��������� ������, ���������� �� �������������.
};


// ����� ����� �������, � ������� ��� ������.
enum ENUM_POINTS_FPGA
{
    FNP_281,
    FNP_512,
    FNP_1024
};


// ��������� ��� X.
struct SettingsTime
{ //-V802
    TBase::E            tBase;          // ������� �� �������.
    int16               tShiftRel;      // �������� �� �������.
    FunctionTime        timeDivXPos;
    TPos                tPos;           // �������� ������������� � ������.
    SampleType          sampleType;     // ��� ������� ��� ������ �������������.
    PeackDetMode::E     peakDet;        // ����� ������ �������� ���������
    bool                selfRecorder;   // ������� �� ����� ���������.
    ENUM_POINTS_FPGA    oldNumPoints;   // \brief ����� ������������� � ����� �������� ���������, ������������� ���������� ����� � 1024, � ���� 
                                        // ���������� ��, ��� ����, ����� ����� ������������.
};



// ��������� ������� �� �������.
void sTime_SetTBase(TBase::E);
// ��������� �������� �� ������� � ������������� ��������.
void sTime_SetTShift(int16 shift);                   
// ������ �������� ���������� ������ ������������� � ������.
int sTime_TPosInPoints(PeackDetMode::E peakDet, int numPoints, TPos tPos);
// �������� �� ������� � ������ ������. �.�. �� ����� � ��� �������� ��� ��� ���������, ��� �������� ���������� �� ����������� � ���������� ����� � ��� ����.
int sTime_TShiftInPoints(PeackDetMode::E peakDet);
// ����������� �������� �� �������, ������� ����� ���� �������� � ���������� �����.
int16 sTime_TShiftMin();
