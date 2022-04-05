#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f4xx_hal.h>


namespace HAL_CRC32
{
    static CRC_HandleTypeDef handleCRC = { CRC };
}


void HAL_CRC32::Init()
{
    HAL_CRC_Init(&handleCRC);
}


uint HAL_CRC32::Calculate(uint *first_word, int num_words)
{
    return HAL_CRC_Calculate(&handleCRC, first_word, (uint)num_words);
}
