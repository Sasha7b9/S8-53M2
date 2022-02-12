#define _STL_COMPILER_PREPROCESSOR 0
#include "defines.h"
#include <usbd_core.h>


USBD_StatusTypeDef USBD_Init(USBD_HandleTypeDef *pdev, USBD_DescriptorsTypeDef *pdesc, uint8_t id)
{
    UNUSED(pdev);
    UNUSED(pdesc);
    UNUSED(id);

    return USBD_OK;
}


USBD_StatusTypeDef USBD_Start(USBD_HandleTypeDef *pdev)
{
    UNUSED(pdev);

    return USBD_OK;
}


USBD_StatusTypeDef USBD_RegisterClass(USBD_HandleTypeDef *pdev, USBD_ClassTypeDef *pclass)
{
    UNUSED(pdev);
    UNUSED(pclass);

    return USBD_OK;
}