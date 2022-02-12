// 2021/06/20 20:26:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define __disable_irq()
#define __set_MSP(x)
#define __enable_irq();

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

#define __IO volatile
#define __STATIC_INLINE inline
#define __CLZ

inline uint32_t __RBIT(uint32_t value)
{
    return value;
}
    