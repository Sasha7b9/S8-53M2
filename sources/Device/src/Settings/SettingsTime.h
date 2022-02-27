// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "SettingsTypes.h"
#include "defines.h"
#include "FPGA/SettingsFPGA.h"


#define SET_TBASE                            (set.time.tBase)

#define SET_TSHIFT                           (set.time.tshift_points)

#define SET_TIME_DIV_XPOS                    (set.time.timeDivXPos)
#define SET_TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY (SET_TIME_DIV_XPOS == FunctionTime_ShiftInMemory)

#define SET_TPOS                             (set.time.tPos)
#define SET_TPOS_IS_LEFT                     (SET_TPOS == TPos::Left)
#define SET_TPOS_IS_CENTER                   (SET_TPOS == TPos::Center)
#define SET_TPOS_IS_RIGHT                    (SET_TPOS == TPos::Right)

#define SET_SAMPLE_TYPE                      (set.time.sampleType)
#define SET_SAMPLE_TYPE_IS_EQUAL             (SET_SAMPLE_TYPE == SampleType_Equal)
#define SET_SAMPLE_TYPE_IS_REAL              (SET_SAMPLE_TYPE == SampleType_Real)

#define PEAKDET                              (set.time.peakDet)
#define PEAKDET_IS_DISABLE                   (PEAKDET == PeackDetMode::Disable)
#define PEAKDET_IS_ENABLE                    (PEAKDET == PeackDetMode::Enable)

#define SET_SELFRECORDER                     (set.time.selfRecorder)




// Функция ручки ВРЕМЯ/ДЕЛ.
enum FunctionTime
{
    FunctionTime_Time,              // Ручка управляет смещением по времени.
    FunctionTime_ShiftInMemory      // Ручка управляет отображаемым на экране участком памяти.
};


// Тип выборки для режима рандомизатора.
enum SampleType
{
    SampleType_Real,   // реальное время - в построении участвуют только реально считанные точки, ничего не рассчитывается.
    SampleType_Equal   // эквивалентная - сигнал строится по последним точкам, полученным от рандомизатора.
};


// Настройки оси X.
struct SettingsTime
{ //-V802
    TBase::E            tBase;          // Масштаб по времени.
    int                 tshift_points;  // Смещение по времени в точках
    FunctionTime        timeDivXPos;
    TPos::E             tPos;           // Привязка синхронизации к памяти.
    SampleType          sampleType;     // Тип выборки для режима рандомизатора.
    PeackDetMode::E     peakDet;        // Режим работы пикового детектора
    bool                selfRecorder;   // Включен ли режим самописца.
};
