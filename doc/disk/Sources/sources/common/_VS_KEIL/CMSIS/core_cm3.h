#pragma once

#define __sizeof_ptr 4

#include <stdint.h>

#define __IO

void NVIC_SystemReset(void);
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
void __disable_irq(void);
void __set_MSP(uint32_t);
void __enable_irq(void);


typedef struct
{
    uint32_t VTOR;
} _SCB;


extern _SCB *SCB;
