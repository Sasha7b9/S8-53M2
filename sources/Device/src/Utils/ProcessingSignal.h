// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGA_Types.h"


class Processing
{
public:
    // ”становить сигнал дл€ обработки.
    static void SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, int firstPoint, int lastPoint);
    // ѕолучить данные ранее установленного сигнала.
    static void GetData(uint8 **data0, uint8 **data1, DataSettings **ds);
    // ѕолучить позицию курсора напр€жени€, соответствующю заданной позиции курсора posCurT.
    static float GetCursU(Chan::E, float posCurT);
    // ѕолучить позицию курсора времени, соответствующую заданной позиции курсора напр€жени€ posCurU.
    static float GetCursT(Chan::E, float posCurU, int numCur);
    // јппроксимировать единичное измерение режима рандомизатора функцией sinX/X.
    static void InterpolationSinX_X(uint8 data[FPGA::MAX_POINTS], TBase tBase);
    // ¬озвращает строку автоматического измерени€.
    static char* GetStringMeasure(Measure measure, Chan::E ch, char buffer[20]);
    // –асчитать все измерени€.
    static void CalculateMeasures();
    // ¬озвращает значение горизонтального маркера. ≈сли ERROR_VALUE_INT - маркер рисовать не нужно.
    static int GetMarkerHorizontal(Chan::E, int numMarker);
    // ¬озвращает значение вертикального маркера. ≈сли ERROR_VALUE_INT - маркер рисовать не нужно.
    static int GetMarkerVertical(Chan::E, int numMarker);
private:
    static float CalculateVoltageMax(Chan::E);
    static float CalculateVoltageMin(Chan::E);
    static float CalculateVoltagePic(Chan::E);
    static float CalculateVoltageMaxSteady(Chan::E);
    static float CalculateVoltageMinSteady(Chan::E);
    static float CalculateVoltageAmpl(Chan::E);
    static float CalculateVoltageAverage(Chan::E);
    static float CalculateVoltageRMS(Chan::E);
    static float CalculateVoltageVybrosPlus(Chan::E);
    static float CalculateVoltageVybrosMinus(Chan::E);
    static float CalculatePeriod(Chan::E);
    // “очно вычисл€ет период или целое число периодов в точках сигнала.
    static int   CalculatePeriodAccurately(Chan::E);
    static float CalculateFreq(Chan::E);
    static float CalculateTimeNarastaniya(Chan::E);
    static float CalculateTimeSpada(Chan::E);
    static float CalculateDurationPlus(Chan::E);
    static float CalculateDurationMinus(Chan::E);
    static float CalculateSkvaznostPlus(Chan::E);
    static float CalculateSkvaznostMinus(Chan::E);
    // ¬озвращает минимальное значение относительного сигнала    
    static float CalculateMinRel(Chan::E);
    // ¬озвращает минимальное установившеес€ значение относительного сигнала
    static float CalculateMinSteadyRel(Chan::E);
    // ¬озвращает максимальное значение относительного сигнала
    static float CalculateMaxRel(Chan::E);
    // ¬озвращает максимальное установившеес€ значение относительного сигнала
    static float CalculateMaxSteadyRel(Chan::E);
    // ¬озвращает среденее значение относительного сигнала
    static float CalculateAverageRel(Chan::E);
    static float CalculatePicRel(Chan::E);
    static float CalculateDelayPlus(Chan::E);
    static float CalculateDelayMinus(Chan::E);
    static float CalculatePhazaPlus(Chan::E);
    static float CalculatePhazaMinus(Chan::E);
    // Ќайти точку пересечени€ сигнала с горизонтальной линией, проведЄнной на уровне yLine. numItersection - пор€дковый номер пересечени€, 
    // начинаетс€ с 1. downToTop - если true, ищем пересечение сигнала со средней линией при прохождении из "-" в "+".
    static float FindIntersectionWithHorLine(Chan::E, int numIntersection, bool downToUp, uint8 yLine);

    static void CountedToCurrentSettings();

    typedef float (*pFuncFCh)(Chan::E);

    struct MeasureCalculate
    {
        char        *name;
        pFuncFCh    FuncCalculate;
        pFuncCFBC   FucnConvertate;
        // ≈сли true, нужно показывать знак.
        bool        showSign;
    };

    static const MeasureCalculate measures[Measure_NumMeasures];
};
