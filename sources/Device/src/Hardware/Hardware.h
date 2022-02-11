// 2022/02/11 17:51:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"


#ifdef __cplusplus
extern "C" {
#endif
    
class Hardware
{
public:
    static void Init();
    static uint CalculateCRC32(uint address = 0x08020000, uint numBytes = 128 * 1024 * 3 / 4);
};
    
#ifdef __cplusplus
}
#endif
