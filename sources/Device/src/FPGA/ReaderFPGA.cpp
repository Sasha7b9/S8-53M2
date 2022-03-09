// 2022/02/22 16:16:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"


namespace FPGA
{
    namespace Reader
    {
        Mutex mutex_read;

        // Чтение двух байт канала 1 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadA();

        // Чтение двух байт канала 2 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadB();
    }
}


void FPGA::Reader::ReadPoints(Chan ch, uint8 *first, const uint8 *last)
{
    uint16 address = Reader::CalculateAddressRead();

    HAL_FMC::Write(WR_PRED, address);
    HAL_FMC::Write(WR_ADDR_READ, 0xffff);

    typedef BitSet16(*pFuncRead)();

    pFuncRead funcRead = ch.IsA() ? Reader::ReadA : Reader::ReadB;

    uint8 *data = first;

    while (data < last)
    {
        BitSet16 bytes = funcRead();

        *data++ = bytes.byte0;
        *data++ = bytes.byte1;
    }
}
