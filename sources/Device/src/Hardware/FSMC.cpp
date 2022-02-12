// 2022/02/11 17:50:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "FSMC.h"
#include "FPGA/FPGA_Types.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_ll_fsmc.h>
#include <stm32f4xx_hal_sram.h>



static const char *addrNamesForWrite[32] =
{
    "WR_START",         // 0x00
    "NONE",             // 0x01
    "NONE",             // 0x02
    "NONE",             // 0x03
    "WR_RAZV",          // 0x04
    "WR_PRED_LO",       // 0x05
    "WR_PRED_HI",       // 0x06
    "WR_POST_LO",
    "WR_POST_HI",
    "WR_TRIG_F",
    "NONE",
    "NONE",
    "WR_CAL_A",
    "WR_CAL_B",
    "WR_UPR",
    "WR_STOP",
    "RD_ADC_A2",
    "RD_ADC_A1",
    "RD_ADC_B2",
    "RD_ADC_B1",
    "RD_RAND_LOW",
    "RD_RAND_HI",
    "RD_FL",
    "RD_LAST_RECORD_LOW",       // 0x17
    "RD_LAST_RECORD_HI",        // 0x18
    "RD_ADDR_PERIOD_LOW_LOW",   // 0x19
    "RD_ADDR_PERIOD_LOW",       // 0x1a
    "RD_ADDR_PERIOD_MID_WR_ADD_RSHIFT_DAC2",    // 0x1b
    "RD_ADDR_PERIOD_HI_WR_ADD_RSHIFT_DAC1",     // 0x1c
    "RD_ADDR_FREQ_LOW",         // 0x1d
    "RD_ADDR_FREQ_MID",         // 0x1e
    "RD_ADDR_FREQ_HI"           // 0x1f
};



uint8 FSMC::Read(uint8 *address)
{
    return(*(address));
}


void FSMC::Write(uint8 *address, uint8 value)
{
    char buffer[9];
    char buffer8[3];
    if((address != WR_START && address != WR_STOP && address != WR_TRIG_F) && IS_SHOW_REGISTERS_ALL)
    {
        LOG_WRITE("%s-%s\x10->\x10%s", Hex8toString(value, buffer8), Bin2String(value, buffer), addrNamesForWrite[address - ADDR_FPGA]);
    }
    else if (IS_SHOW_REG_TSHIFT && (address == WR_POST_LOW || address == WR_POST_HI || address == WR_PRED_HI || address == WR_PRED_LOW))
    {
        LOG_WRITE("%s-%s\x10->\x10%s", Hex8toString(value, buffer8), Bin2String(value, buffer), addrNamesForWrite[address - ADDR_FPGA]);
    }
    else if(IS_SHOW_REG_TBASE && address == WR_RAZVERTKA)
    {
        LOG_WRITE("%s-%s\x10->\x10%s", Hex8toString(value, buffer8), Bin2String(value, buffer), addrNamesForWrite[address - ADDR_FPGA]);
    };
    *address = value;
}
