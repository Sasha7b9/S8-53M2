// 2022/02/11 17:50:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


#define ADDR_BANK  0x60000000
#define ADDR_FPGA  ((uint8*)(ADDR_BANK + 0x00c80000))  // Адрес записи в аппаратные регистры.

