// 2022/2/12 9:46:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"


namespace HAL_FMC
{
    static const uint _ADDR_BANK = 0x60000000;
    static const uint _ADDR_RAM = _ADDR_BANK + 0x04000000;

    // ����� ������ � ���������� ��������. 0x100000 - ��� �������� ����� ��� ��������� A19 �� FPGA � 1. ���� �� �������
    // ������ ���� 0x80000 (������ ��� a18 �������� �� 0x40000)
    uint16* const _ADDR_FPGA = (uint16*)(_ADDR_BANK + 0x00000000 + 0x100000); //-V566

    uint16* const _ADDR_RAM_DISPLAY_FRONT = (uint16*)(_ADDR_RAM + 1024 * 1024 - 320 * 240); //-V566

    static void InitRAM();

    static void InitFPGA();
}


void HAL_FMC::Init()
{
    InitFPGA();

    InitRAM();
}


void HAL_FMC::InitFPGA()
{
    __FMC_CLK_ENABLE();

    HAL_PINS::FMC_::Init();

    static SRAM_HandleTypeDef gSramHandle =
    {
        FMC_NORSRAM_DEVICE,
        FMC_NORSRAM_EXTENDED_DEVICE,
        {
            FMC_NORSRAM_BANK1,                 // Init.NSBank
            FMC_DATA_ADDRESS_MUX_DISABLE,      // Init.DataAddressMux
            FMC_MEMORY_TYPE_NOR,               // Init.MemoryType
            FMC_NORSRAM_MEM_BUS_WIDTH_16,      // Init.MemoryDataWidth
            FMC_BURST_ACCESS_MODE_DISABLE,     // Init.BurstAccessMode
            FMC_WAIT_SIGNAL_POLARITY_LOW,      // Init.WaitSignalPolarity
            FMC_WRAP_MODE_DISABLE,             // Init.WrapMode
            FMC_WAIT_TIMING_BEFORE_WS,         // Init.WaitSignalActive
            FMC_WRITE_OPERATION_ENABLE,        // Init.WriteOperation
            FMC_WAIT_SIGNAL_DISABLE,           // Init.WaitSignal
            FMC_EXTENDED_MODE_DISABLE,         // Init.ExtendedMode
            FMC_ASYNCHRONOUS_WAIT_DISABLE,     // Init.AsynchronousWait
            FMC_WRITE_BURST_DISABLE            // Init.WriteBurst
        }
    };

    static const FMC_NORSRAM_TimingTypeDef sramTiming =
    {
        1,                  // FSMC_AddressSetupTime
        1,                  // FSMC_AddressHoldTime
        2,                  // FSMC_DataSetupTime
        1,                  // FSMC_BusTurnAroundDuration
        1,                  // FSMC_CLKDivision
        1,                  // FSMC_DataLatency
        FMC_ACCESS_MODE_C   // FSMC_AccessMode
    };

    FMC_NORSRAM_TimingTypeDef* timing = const_cast<FMC_NORSRAM_TimingTypeDef*>(&sramTiming);

    HAL_SRAM_Init(&gSramHandle, timing, timing);
}


void HAL_FMC::InitRAM()
{
    // RAM ������� �� 1�� ������
    // �� - A0...A18, �� - D0...D15

    __FMC_CLK_ENABLE();

    HAL_PINS::FMC_::Init();

    static SRAM_HandleTypeDef gSramHandle =
    {
        FMC_NORSRAM_DEVICE,
        FMC_NORSRAM_EXTENDED_DEVICE,
        {
            FMC_NORSRAM_BANK2,                 // Init.NSBank
            FMC_DATA_ADDRESS_MUX_DISABLE,      // Init.DataAddressMux
            FMC_MEMORY_TYPE_NOR,               // Init.MemoryType
            FMC_NORSRAM_MEM_BUS_WIDTH_16,      // Init.MemoryDataWidth
            FMC_BURST_ACCESS_MODE_DISABLE,     // Init.BurstAccessMode
            FMC_WAIT_SIGNAL_POLARITY_LOW,      // Init.WaitSignalPolarity
            FMC_WRAP_MODE_DISABLE,             // Init.WrapMode
            FMC_WAIT_TIMING_BEFORE_WS,         // Init.WaitSignalActive
            FMC_WRITE_OPERATION_ENABLE,        // Init.WriteOperation
            FMC_WAIT_SIGNAL_DISABLE,           // Init.WaitSignal
            FMC_EXTENDED_MODE_DISABLE,         // Init.ExtendedMode
            FMC_ASYNCHRONOUS_WAIT_DISABLE,     // Init.AsynchronousWait
            FMC_WRITE_BURST_DISABLE            // Init.WriteBurst
        }
    };

    static const FMC_NORSRAM_TimingTypeDef sramTiming =
    {
        1,                 // FSMC_AddressSetupTime
        1,                 // FSMC_AddressHoldTime
        2,                 // FSMC_DataSetupTime
        1,                 // FSMC_BusTurnAroundDuration
        1,                 // FSMC_CLKDivision
        1,                 // FSMC_DataLatency
        FMC_ACCESS_MODE_C  // FSMC_AccessMode
    };

    FMC_NORSRAM_TimingTypeDef* timing = const_cast<FMC_NORSRAM_TimingTypeDef*>(&sramTiming);

    HAL_SRAM_Init(&gSramHandle, timing, timing);
}


uint8 HAL_FMC::Read(uint8*)
{
    return 0;
}


void HAL_FMC::Write(uint8 *, uint8)
{

}
