// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA.h"


namespace Processing
{
    extern DataStruct out;      // Здесь хранятся данные, готовые для вывода - преобразованные из in

    // Установить данные для обработки в соотетствии с текущими настройками и состоянием
    // Если for_windows_memory - установить данные для окна памяти
    void SetDataForProcessing(ModeWork::E, bool for_window_memory);

    // Получить позицию курсора напряжения, соответствующю заданной позиции курсора posCurT.
    float GetCursU(Chan, float posCurT);

    // Получить позицию курсора времени, соответствующую заданной позиции курсора напряжения posCurU.
    float GetCursT(Chan, float posCurU, int numCur);

    // Аппроксимировать единичное измерение режима рандомизатора функцией sinX/X.
    void InterpolationSinX_X(uint8 data[FPGA::MAX_POINTS * 2], TBase::E tbase);

    // Возвращает строку автоматического измерения.
    String GetStringMeasure(Measure::E measure, Chan);

    // Расчитать все измерения.
    void CalculateMeasures();

    // Возвращает значение горизонтального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
    int GetMarkerVoltage(Chan, int numMarker);

    // Возвращает значение вертикального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
    int GetMarkerTime(Chan, int numMarker);

    // Обработать сигнал из in. Результат сохранён в Data::out
    // Если mode_p2p == true, то будет установлен признак поточечного фрейма
    void SetData(const DataStruct &in, bool mode_p2p = false);

    // Привести к текущим настройкам данные, из inA, inB. Данные сохраняются в out
    void CountedToCurrentSettings(const DataSettings &, const uint8 *inA, const uint8 *inB, DataStruct &out);
};
