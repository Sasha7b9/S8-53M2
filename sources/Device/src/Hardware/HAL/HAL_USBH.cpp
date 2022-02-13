#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <usbh_def.h>


namespace HAL_USBH
{
    USBH_HandleTypeDef handleUSBH;

    void *handle = &handleUSBH;
}
