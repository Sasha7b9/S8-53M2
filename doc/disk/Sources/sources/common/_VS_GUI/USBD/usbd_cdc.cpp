#define _STL_COMPILER_PREPROCESSOR 0
#include "defines.h"
#include <usbd_cdc.h>


static uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    UNUSED(pdev);
    UNUSED(cfgidx);

    return 0;
}


static uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    UNUSED(pdev);
    UNUSED(cfgidx);

    return 0;
}


static uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    UNUSED(pdev);
    UNUSED(req);

    return 0;
}


static uint8_t USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
    UNUSED(pdev);

    return 0;
}


static uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    UNUSED(pdev);
    UNUSED(epnum);

    return 0;
}


static uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    UNUSED(pdev);
    UNUSED(epnum);

    return 0;
}


static uint8_t *USBD_CDC_GetHSCfgDesc(uint16_t *length)
{
    UNUSED(length);

    return 0;
}


static uint8_t *USBD_CDC_GetFSCfgDesc(uint16_t *length)
{
    UNUSED(length);

    return 0;
}


static uint8_t *USBD_CDC_GetOtherSpeedCfgDesc(uint16_t *length)
{
    UNUSED(length);

    return 0;
}


uint8_t *USBD_CDC_GetDeviceQualifierDescriptor(uint16_t *length)
{
    UNUSED(length);

    return 0;
}


USBD_ClassTypeDef  USBD_CDC =
{
  USBD_CDC_Init,
  USBD_CDC_DeInit,
  USBD_CDC_Setup,
  NULL,                 /* EP0_TxSent, */
  USBD_CDC_EP0_RxReady,
  USBD_CDC_DataIn,
  USBD_CDC_DataOut,
  NULL,
  NULL,
  NULL,
  USBD_CDC_GetHSCfgDesc,
  USBD_CDC_GetFSCfgDesc,
  USBD_CDC_GetOtherSpeedCfgDesc,
  USBD_CDC_GetDeviceQualifierDescriptor,
};



uint8_t USBD_CDC_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_CDC_ItfTypeDef *fops)
{
    UNUSED(pdev);
    UNUSED(fops);

    return 0;
}


uint8_t USBD_CDC_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff, uint32_t length)
{
    UNUSED(pdev);
    UNUSED(pbuff);
    UNUSED(length);

    return 0;
}



uint8_t USBD_CDC_TransmitPacket(USBD_HandleTypeDef *pdev)
{
    UNUSED(pdev);

    return 0;
}


uint8_t USBD_CDC_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff)
{
    UNUSED(pdev);
    UNUSED(pbuff);

    return 0;
}


uint8_t USBD_CDC_ReceivePacket(USBD_HandleTypeDef *pdev)
{
    UNUSED(pdev);

    return 0;
}