// 2022/02/23 08:44:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"


namespace FPGA
{
    namespace Reader
    {
        extern Mutex mutex_read;                    // Мьютекс на чтение данных

        // Эта функция должна вызываться только для чтения первого канала. При чтении второго канала нужно использовать
        // ранее полученное значение
        uint16 CalculateAddressRead();
    }

    namespace Launch
    {
        // Загружает в аппаратную часть пред- и после- запуски
        void Load();

        // Возвращает значение предзапуска, готовое для записи в альтеру
        uint16 PredForWrite();
    }
}
