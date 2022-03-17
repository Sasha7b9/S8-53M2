#pragma once
#include "Data/Data.h"


// Усреднитель
namespace Averager
{
    // Добавить новые данные для усреднения
    void Append(DataStruct &);

    // Получить усреднённые данные по нескольким измерениям.
    void GetData(DataStruct &);

    // Сбросить настройки на настройки, соответствующие текущим
    void Reset();
}

