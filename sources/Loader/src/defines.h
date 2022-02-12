// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <stm32f4xx_hal.h>
#include "globals.h"

typedef unsigned char      uint8;
typedef unsigned int       uint;
typedef signed char        int8;
typedef unsigned short int uint16;
typedef signed short int   int16;
typedef unsigned char      uchar;
typedef const char        *pchar;
typedef pchar const        pstring;

#ifndef __cplusplus
typedef unsigned char bool;
#define false   0
#define true    (!false)
#endif


// Тип цвета
struct Color
{
    enum E
    {
        BLACK             = 0x00,
        WHITE             = 0x01,
        GRID              = 0x02,
        DATA_A            = 0x03,
        DATA_B            = 0x04,
        MENU_FIELD        = 0x05,
        MENU_TITLE        = 0x06,
        MENU_TITLE_DARK   = 0x07,
        MENU_TITLE_BRIGHT = 0x08,
        MENU_ITEM         = 0x09,
        MENU_ITEM_DARK    = 0x0a,
        MENU_ITEM_BRIGHT  = 0x0b,
        MENU_SHADOW       = 0x0c,
        EMPTY             = 0x0d,
        EMPTY_A           = 0x0e,
        EMPTY_B           = 0x0f,
        COUNT,
        FLASH_10,
        FLASH_01,
        SET_INVERSE
    };
};

typedef void    (*pFuncVV)(void);               // ????????? ?? ???????, ?????? ?? ??????????? ? ?????? ?? ????????????.
typedef void    (*pFuncVpV)(void*);             // ????????? ?? ???????, ??????????? ????????? ?? void ? ?????? ?? ????????????.
typedef bool    (*pFuncBV)(void);
typedef void    (*pFuncVB)(bool);
typedef void    (*pFuncVI)(int);
typedef void    (*pFuncVII)(int, int);
typedef void    (*pFuncVI16)(int16);
typedef bool    (*pFuncBU8)(uint8);
typedef void    (*pFuncVI16pI16pI16)(int16, int16*, int16*);
typedef float   (*pFuncFU8)(uint8);
typedef char*   (*pFuncCFB)(float, bool);
typedef char*   (*pFuncCFBC)(float, bool, char*);
typedef void    (*pFuncpU8)(uint8*);
typedef void    (*pFuncVpVIIB)(void*, int, int, bool);

#define ERROR_VALUE_FLOAT   1.111e29f
#define ERROR_VALUE_INT16   SHRT_MAX
#define ERROR_VALUE_UINT8   255
#define ERROR_VALUE_INT     INT_MAX
#define ERROR_STRING_VALUE  "--.--"
#define M_PI                3.14159265358979323846f
