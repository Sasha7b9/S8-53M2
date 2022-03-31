// 2021/06/24 14:23:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "common/Hardware/FDrive/USBH_.h"
#include <usbh_def.h>


USBH_HandleTypeDef handleUSBH;

void *USBH::handle = &handleUSBH;

