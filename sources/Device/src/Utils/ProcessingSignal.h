// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA_Types.h"


class Processing
{
public:
    // ”становить сигнал дл€ обработки.
    static void SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, int firstPoint, int lastPoint);
    // ѕолучить данные ранее установленного сигнала.
    static void GetData(uint8 **data0, uint8 **data1, DataSettings **ds);
    // ѕолучить позицию курсора напр€жени€, соответствующю заданной позиции курсора posCurT.
    static float GetCursU(Channel chan, float posCurT);
    // ѕолучить позицию курсора времени, соответствующую заданной позиции курсора напр€жени€ posCurU.
    static float GetCursT(Channel chan, float posCurU, int numCur);
    // јппроксимировать единичное измерение режима рандомизатора функцией sinX/X.
    static void InterpolationSinX_X(uint8 data[FPGA_MAX_POINTS], TBase tBase);
    // ¬озвращает строку автоматического измерени€.
    static char* GetStringMeasure(Measure measure, Chan::E ch, char buffer[20]);
    // –асчитать все измерени€.
    static void CalculateMeasures();
    // ¬озвращает значение горизонтального маркера. ≈сли ERROR_VALUE_INT - маркер рисовать не нужно.
    static int GetMarkerHorizontal(Channel chan, int numMarker);
    // ¬озвращает значение вертикального маркера. ≈сли ERROR_VALUE_INT - маркер рисовать не нужно.
    static int GetMarkerVertical(Channel chan, int numMarker);
private:
    static float CalculateVoltageMax(Chan::E ch);
    static float CalculateVoltageMin(Chan::E ch);
    static float CalculateVoltagePic(Chan::E ch);
    static float CalculateVoltageMaxSteady(Chan::E ch);
    static float CalculateVoltageMinSteady(Chan::E ch);
    static float CalculateVoltageAmpl(Chan::E ch);
    static float CalculateVoltageAverage(Chan::E ch);
    static float CalculateVoltageRMS(Chan::E ch);
    static float CalculateVoltageVybrosPlus(Chan::E ch);
    static float CalculateVoltageVybrosMinus(Chan::E ch);
    static float CalculatePeriod(Chan::E ch);
    // “очно вычисл€ет период или целое число периодов в точках сигнала.
    static int   CalculatePeriodAccurately(Chan::E ch);
    static float CalculateFreq(Chan::E ch);
    static float CalculateTimeNarastaniya(Chan::E ch);
    static float CalculateTimeSpada(Chan::E ch);
    static float CalculateDurationPlus(Chan::E ch);
    static float CalculateDurationMinus(Chan::E ch);
    static float CalculateSkvaznostPlus(Chan::E ch);
    static float CalculateSkvaznostMinus(Chan::E ch);
    // ¬озвращает минимальное значение относительного сигнала    
    static float CalculateMinRel(Chan::E ch);
    // ¬озвращает минимальное установившеес€ значение относительного сигнала
    static float CalculateMinSteadyRel(Chan::E ch);
    // ¬озвращает максимальное значение относительного сигнала
    static float CalculateMaxRel(Chan::E ch);
    // ¬озвращает максимальное установившеес€ значение относительного сигнала
    static float CalculateMaxSteadyRel(Chan::E ch);
    // ¬озвращает среденее значение относительного сигнала
    static float CalculateAverageRel(Chan::E ch);
    static float CalculatePicRel(Chan::E ch);
    static float CalculateDelayPlus(Chan::E ch);
    static float CalculateDelayMinus(Chan::E ch);
    static float CalculatePhazaPlus(Chan::E ch);
    static float CalculatePhazaMinus(Chan::E ch);
    // Ќайти точку пересечени€ сигнала с горизонтальной линией, проведЄнной на уровне yLine. numItersection - пор€дковый номер пересечени€, 
    // начинаетс€ с 1. downToTop - если true, ищем пересечение сигнала со средней линией при прохождении из "-" в "+".
    static float FindIntersectionWithHorLine(Channel chan, int numIntersection, bool downToUp, uint8 yLine);

    static void CountedToCurrentSettings();

    typedef float (*pFuncFCh)(Channel);

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
