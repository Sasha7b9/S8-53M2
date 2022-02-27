// 2022/02/23 08:44:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"


namespace FPGA
{
    namespace Reader
    {
        extern Mutex mutex_read;                    // Мьютекс на чтение данных

        // Возвращает адрес, с которого нужно читать первую точку
        uint16 CalculateAddressRead();

        // Смещение первой точки при чтении
        int DeltaRead();
    }

    namespace Launch
    {
        // Загружает в аппаратную часть пред- и после- запуски
        void Load();

        // Возвращает значение предзапуска, готовое для записи в альтеру
        uint16 PredForWrite();
    }
}
