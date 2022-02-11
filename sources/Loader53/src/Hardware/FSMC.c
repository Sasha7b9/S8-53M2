#include "FSMC.h"
#include "Hardware.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/Timer.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_ll_fsmc.h>
#include <stm32f4xx_hal_sram.h>

/*
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
    "WR_CAL_1",
    "WR_CAL_2",
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
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FSMC_Init(void);
uint8 FSMC_Read(uint8 *address);
void FSMC_Write(uint8 *address, uint8 value);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8 FSMC_Read(uint8 *address)
{
    return(*(address));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FSMC_Write(uint8 *address, uint8 value)
{
    *address = value;
}
