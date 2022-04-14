#include "defines.h"
#include <usbh_core.h>


USBH_StatusTypeDef  USBH_Process(USBH_HandleTypeDef *)
{
    return USBH_FAIL;
}


USBH_StatusTypeDef  USBH_LL_Connect(USBH_HandleTypeDef *)
{
    return USBH_OK;
}


USBH_StatusTypeDef  USBH_LL_Disconnect(USBH_HandleTypeDef *)
{
    return USBH_OK;
}


void  USBH_LL_SetTimer(USBH_HandleTypeDef *, uint32_t)
{
}


void  USBH_LL_IncTimer(USBH_HandleTypeDef *)
{
}


void USBH_LL_PortDisabled(USBH_HandleTypeDef *)
{
}


void USBH_LL_PortEnabled(USBH_HandleTypeDef *)
{
}