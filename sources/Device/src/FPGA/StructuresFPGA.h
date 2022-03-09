// 2022/02/23 08:44:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"


struct StateCalibration
{
    enum E
    {
        None,
        RShift0start,
        RShift0inProgress,
        RShift1start,
        RShift1inProgress,
        ErrorCalibration0,
        ErrorCalibration1
    };
};

struct StateWorkFPGA
{
    enum E
    {
        Stop,    // СТОП - не занимается считыванием информации.
        Wait,    // Ждёт поступления синхроимпульса.
        Work,    // Идёт работа.
        Pause    // Это состояние, когда временно приостановлен прибор, например, для чтения данных или для 
                 // записи значений регистров.
    };

    E value;

    StateWorkFPGA(E v = Stop) : value(v) {}

    static StateWorkFPGA::E GetCurrent() { return current; }
    static void SetCurrent(StateWorkFPGA::E v) { current = v; }

private:

    static E current;
};


struct StateFPGA
{
    bool                needCalibration;               // Установленное в true значение означает, что необходимо произвести калибровку.
    StateWorkFPGA       stateWorkBeforeCalibration;
    StateCalibration::E stateCalibration;              // Текущее состояние калибровки. Используется в процессе калибровки.
};


namespace FPGA
{
    namespace Reader
    {
        extern Mutex mutex_read;                    // Мьютекс на чтение данных

        // Эта функция должна вызываться только для чтения первого канала. При чтении второго канала нужно использовать
        // ранее полученное значение
        // Возвращаемое значение на 1 меньше реального. Т.е. первое чтение нужно отбросить - что-то с альтерой
        uint16 CalculateAddressRead();

        // Читать массив точек от first до last
        void ReadPoints(Chan, uint8 *first, const uint8 *last);

        void ReadPoints(Chan);
    }

    namespace Launch
    {
        // Загружает в аппаратную часть пред- и после- запуски
        void Load();

        // Возвращает значение предзапуска, готовое для записи в альтеру
        uint16 PredForWrite();
    }
}
