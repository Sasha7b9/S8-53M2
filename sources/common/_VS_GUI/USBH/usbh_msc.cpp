// 2021/06/17 21:16:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include <usbh_def.h>
#include <usbh_msc.h>


uint8_t  USBH_MSC_IsReady(USBH_HandleTypeDef *)
{
    return 0;
}


uint8_t  USBH_MSC_UnitIsReady(USBH_HandleTypeDef *, uint8_t)
{
    return 0;
}


USBH_StatusTypeDef USBH_MSC_Read(USBH_HandleTypeDef *, uint8_t, uint32_t, uint8_t *, uint32_t)
{
    return USBH_OK;
}


USBH_StatusTypeDef USBH_MSC_GetLUNInfo(USBH_HandleTypeDef *, uint8_t, MSC_LUNTypeDef *)
{
    return USBH_OK;
}


USBH_StatusTypeDef USBH_MSC_Write(USBH_HandleTypeDef *, uint8_t, uint32_t, uint8_t *, uint32_t)
{
    return USBH_OK;
}