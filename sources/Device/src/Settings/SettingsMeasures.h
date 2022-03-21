// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Data/Measures.h"


#define MEAS_NUM                        (set.measures.number)
#define MEAS_NUM_IS_1                   (MEAS_NUM == MeasuresNumber::_1)
#define MEAS_NUM_IS_1_5                 (MEAS_NUM == MeasuresNumber::_1_5)
#define MEAS_NUM_IS_2_5                 (MEAS_NUM == MeasuresNumber::_2_5)
#define MEAS_NUM_IS_3_5                 (MEAS_NUM == MeasuresNumber::_3_5)
#define MEAS_NUM_IS_6_1                 (MEAS_NUM == MeasuresNumber::_6_1)
#define MEAS_NUM_IS_6_2                 (MEAS_NUM == MeasuresNumber::_6_2)

#define MEAS_SOURCE                     (set.measures.source)
#define MEAS_SOURCE_IS_A                (MEAS_SOURCE == Chan::A)
#define MEAS_SOURCE_IS_B                (MEAS_SOURCE == Chan::B)
#define MEAS_SOURCE_IS_A_B              (MEAS_SOURCE == Chan::A_B)

#define MODE_VIEW_SIGNALS               (set.measures.modeViewSignals)
#define MODE_VIEW_SIGNALS_IS_COMPRESS   (MODE_VIEW_SIGNALS == ModeViewSignals_Compress)

#define MEAS_MARKED                     (set.measures.markedMeasure)
#define MEAS_MARKED_IS_NONE             (MEAS_MARKED == Measure::None)

#define MEASURE(num)                    (set.measures.measures[num])
#define MEASURE_IS_MARKED(num)          (MEASURE(num) == MEAS_MARKED)

#define SHOW_MEASURES                   (set.measures.show)

#define MEAS_FIELD                      (set.measures.field)
#define MEAS_FIELD_IS_HAND              (MEAS_FIELD == MeasuresField_Hand)

#define MEAS_POS_CUR_U(num)             (set.measures.posCurU[num])
#define MEAS_POS_CUR_U0                 (MEAS_POS_CUR_U(0))
#define MEAS_POS_CUR_U1                 (MEAS_POS_CUR_U(1))

#define MEAS_POS_CUR_T(num)             (set.measures.posCurT[num])
#define MEAS_POS_CUR_T0                 (MEAS_POS_CUR_T(0))
#define MEAS_POS_CUR_T1                 (MEAS_POS_CUR_T(1))

#define MEAS_CURS_CNTRL_U               (set.measures.cntrlU)

#define MEAS_CURS_CNTRL_T               (set.measures.cntrlT)

#define MEAS_CURS_ACTIVE                (set.measures.cursActive)
#define MEAS_CURS_ACTIVE_IS_T           (MEAS_CURS_ACTIVE == CursActive::T)




// ���������� � ������������ �� ������ ����������� ���������.
struct MeasuresNumber
{
    enum E
    {
        _1,                       // 1 ��������� ����� �����.
        _2,                       // 2 ��������� ����� �����.
        _1_5,                     // 1 ������ � 5 �����������.
        _2_5,                     // 2 ������ �� 5 ���������.
        _3_5,                     // 3 ������ �� 5 ���������.
        _6_1,                     // 6 ����� �� 1 ���������.
        _6_2                      // 6 ����� �� 2 ���������.
    };
};


// ����, �� ������� ��������� ��������
enum MeasuresField
{
    MeasuresField_Screen,       // ��������� ����� ������������� �� ��� ����� �������, ������� ����� �� ������.
    MeasuresField_AllMemory,    // ��������� ����� ������������� �� ����� �������.
    MeasuresField_Hand          // ��������� ����� ������������� �� ����, ����������� �������������.
};


// ��������� ���� ���������
struct SettingsMeasures
{
    MeasuresNumber::E number;          // ������� ��������� ��������.
    Chan::E           source;          // ��� ����� ������� �������� ���������.
    ModeViewSignals   modeViewSignals; // ������� �� ������� ��� ������ ���������.
    Measure::E        measures[15];    // ��������� ��� ��������� ���������.
    bool              show;            // ���������� �� ���������.
    MeasuresField     field;           // ����� �������, �� ������� ������� �������� ��� ������� ���������.
    int16             posCurU[2];      // ������� ��������, ������� ������ �������, �� ������� ������� �������� ��� ������� ��������� ��� field == MeasuresField_Hand.
    int16             posCurT[2];      // ������� ��������, ������� ������ �������, �� ������� ������� �������� ��� ������� ��������� ��� field == MeasuresField_Hand.
    CursCntrl::E      cntrlU;          // �������� ������� ����������.
    CursCntrl::E      cntrlT;          // �������� ������� �������.
    CursActive::E     cursActive;      // ����� ������� ������� - �� ������� ��� ����������.
    Measure::E        markedMeasure;   // ���������, �� ������� ����� �������� �������.
};
