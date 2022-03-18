#pragma once
#include "Data/Data.h"


// Усреднитель
namespace Averager
{
    // Добавить новые данные для усреднения
    void Append(DataSettings *ds);

    // Получить усреднённые данные по нескольким измерениям.
    void GetData(DataStruct &);

    // Сбросить настройки на настройки, соответствующие текущим
    void Reset();
}

