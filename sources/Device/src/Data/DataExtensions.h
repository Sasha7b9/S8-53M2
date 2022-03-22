#pragma once
#include "Data/Data.h"


// Усреднитель
namespace Averager
{
    // Добавить новые данные для усреднения
    void Append(DataFrame &);

    // Получить усреднённые данные по нескольким измерениям.
    void GetData(DataStruct &);

    // Сбросить настройки на настройки, соответствующие текущим
    void Reset();
}


// Расчёт мин-макс
namespace Limitator
{
    // Очистка значений мин, макс и сумм
    void ClearLimits();

    void CalculateLimits(const DataSettings *, const uint8 *dataA, const uint8 *dataB);

    // Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху.
    DataStruct &GetLimitation(Chan, int direction, DataStruct &);
}
