// 2022/02/22 16:16:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Data/Storage.h"
#include "Utils/Containers/Queue.h"
#include "Panel/Panel.h"


namespace FPGA
{
    extern bool IN_PROCESS_READ;
    extern bool TRIG_AUTO_FIND;

    namespace Reader
    {
        Mutex mutex_read;

        // Чтение двух байт канала 1 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadA();

        // Чтение двух байт канала 2 (с калибровочными коэффициентами, само собой)
        BitSet16 ReadB();

        namespace P2P
        {
            BitSet16 bufferA[10];
            BitSet16 bufferB[10];
            int pointer = 0;

            Mutex mutex;
        }
    }
}


void FPGA::Reader::ReadPoints(Chan ch, uint8 *first, const uint8 *last)
{
    uint16 address = Reader::CalculateAddressRead();

    HAL_FMC::Write(WR_PRED, address);
    HAL_FMC::Write(WR_ADDR_READ, 0xffff);

    typedef BitSet16(*pFuncRead)();

    pFuncRead funcRead = ch.IsA() ? Reader::ReadA : Reader::ReadB;

    funcRead();                 // Делаем лишнее чтение, потому что адрес первой точки на 1 меньше требуемого (что-то глючит в альтере)

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


void FPGA::Reader::Read1024Points(uint8 buffer[1024], Chan ch)
{
    Timer::PauseOnTime((SET_RANGE(ch) < 2) ? 500U : 100U);

    FPGA::Start();

    std::memset(buffer, 255, 1024);

    Timer::PauseOnTime(8);

    uint16 fl = HAL_FMC::Read(RD_FL);

    while (_GET_BIT(fl, FL_PRED) == 0) { fl = HAL_FMC::Read(RD_FL); }

    FPGA::SwitchingTrig();

    while (_GET_BIT(fl, FL_TRIG) == 0) { fl = HAL_FMC::Read(RD_FL); }

    Timer::PauseOnTime(8);

    while (_GET_BIT(fl, FL_DATA) == 0) { fl = HAL_FMC::Read(RD_FL); }

    FPGA::Stop(false);

    FPGA::Reader::ReadPoints(ch, buffer, &buffer[0] + 1024);
}


void FPGA::Reader::P2P::ReadPoints()
{
    if (!mutex.Locked())
    {
        flag.Read();

        if (_GET_BIT(flag.value, FL_POINT))
        {
            bufferA[pointer] = Reader::ReadA();
            bufferB[pointer] = Reader::ReadB();
            pointer++;
        }
    }
}


void FPGA::Reader::P2P::SavePoints()
{
    mutex.Lock();

    if (pointer > 0)
    {
        int index = 0;

        while (index < pointer)
        {
            Storage::current.AppendPoints(bufferA[index], bufferB[index]);
            index++;
        }

        pointer = 0;
    }

    mutex.Unlock();
}


void FPGA::Reader::DataRead()
{
    Panel::EnableLEDTrig(false);

    IN_PROCESS_READ = true;

    Reader::ReadPoints(ChA);
    Reader::ReadPoints(ChB);

    if (!TBase::InModeRandomizer())
    {
        if (SET_INVERSE_A) Storage::current.Inverse(ChA);
        if (SET_INVERSE_B) Storage::current.Inverse(ChB);
    }

    Storage::AppendNewFrame(Storage::current.frame);

    if (TRIG_MODE_FIND_IS_AUTO && TRIG_AUTO_FIND)
    {
        FPGA::FindAndSetTrigLevel();

        TRIG_AUTO_FIND = false;
    }

    IN_PROCESS_READ = false;
}
