// 2021/06/17 15:14:50 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include <stm32f4xx_hal.h>


HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_HCD_DeInit(HCD_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_HCD_HC_Init(HCD_HandleTypeDef *, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_HCD_HC_Halt(HCD_HandleTypeDef *, uint8_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t *,
    uint16_t, uint8_t)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_HCD_ResetPort(HCD_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_HCD_Start(HCD_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_HCD_Stop(HCD_HandleTypeDef *)
{
    return HAL_OK;
}


HCD_URBStateTypeDef HAL_HCD_HC_GetURBState(HCD_HandleTypeDef *, uint8_t)
{
    return URB_DONE;
}


uint32_t HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef *, uint8_t)
{
    return 0;
}


uint32_t HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *)
{
    return 0;
}


uint32_t HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *)
{
    return 0;
}