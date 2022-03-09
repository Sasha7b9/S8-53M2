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

        // ������ ���� ���� ������ 1 (� �������������� ��������������, ���� �����)
        BitSet16 ReadA();

        // ������ ���� ���� ������ 2 (� �������������� ��������������, ���� �����)
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


BitSet16 FPGA::Reader::ReadA()
{
    BitSet16 data(*RD_ADC_A);

    int delta0 = data.byte0 - ValueFPGA::AVE;
    delta0 *= CAL_STRETCH_A;
    
    int byte0 = ValueFPGA::AVE + delta0;

    if (byte0 < 0) { byte0 = 0; }
    else if (byte0 > 255) { byte0 = 255; }

    data.byte0 = (uint8)byte0;

    int byte1 = (int)data.byte1 + SET_BALANCE_ADC_A;

    int delta1 = byte1 - ValueFPGA::AVE;
    delta1 *= CAL_STRETCH_A;
    byte1 = ValueFPGA::AVE + delta1;

    if (byte1 < 0)        { byte1 = 0; }
    else if (byte1 > 255) { byte1 = 255; }

    data.byte1 = (uint8)byte1;

    return data;
}


BitSet16 FPGA::Reader::ReadB()
{
    BitSet16 data(*RD_ADC_B);

    int delta0 = data.byte0 - ValueFPGA::AVE;
    delta0 *= CAL_STRETCH_B;

    int byte0 = ValueFPGA::AVE + delta0;

    if (byte0 < 0) { byte0 = 0; }
    else if (byte0 > 255) { byte0 = 255; }

    data.byte0 = (uint8)byte0;

    int byte1 = (int)data.byte1 + SET_BALANCE_ADC_B;

    int delta1 = byte1 - ValueFPGA::AVE;
    delta1 *= CAL_STRETCH_B;
    byte1 = ValueFPGA::AVE + delta1;

    if (byte1 < 0)        { byte1 = 0; }
    else if (byte1 > 255) { byte1 = 255; }

    data.byte1 = (uint8)byte1;

    return data;
}
