#include "defines.h"
#include <stm32f4xx_hal.h>


void HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *, uint32_t, uint32_t)
{

}


uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *, uint32_t)
{
    return 0;
}


HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *, RTC_TimeTypeDef *, uint32_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *, RTC_TimeTypeDef *, uint32_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *, RTC_DateTypeDef *, uint32_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *, RTC_DateTypeDef *, uint32_t)
{
    return HAL_OK;
}
