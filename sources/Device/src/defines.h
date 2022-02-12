// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifdef WIN32
    #define __attribute__(x)
    #define __packed__
#else
    #define nullptr 0 //-V1059
#endif


#include <stm32f4xx_hal.h>


#define NUM_VER "1.3"


typedef signed      char        int8;
typedef signed      short   int int16;
typedef unsigned    char        uint8;
typedef unsigned    short   int uint16;
typedef unsigned    int         uint;
typedef unsigned long long int  uint64;
typedef unsigned    char        uchar;
typedef const char             *pchar;


// ��� �����
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
    } value;

    static void ResetFlash();
};


typedef void    (*pFuncVV)();               // ��������� �� �������, ������ �� ����������� � ������ �� ������������.
typedef void    (*pFuncVpV)(void*);             // ��������� �� �������, ����������� ��������� �� void � ������ �� ������������.
typedef bool    (*pFuncBV)();
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

#define _GET_BIT(value, bit) (((value) >> (bit)) & 0x01)
#define _SET_BIT(value, bit) ((value) |= (1 << (bit)))
#define _CLEAR_BIT(value, bit) ((value) &= (~(1 << (bit))))

// ����������� �������� 32 ����
typedef union
{
    uint  word;
    uint8 byte[4]; //-V112
} BitSet32;

// ����������� �������� 64 ����
typedef union
{
    long long unsigned int  dword;
    unsigned int            word[2];
} BitSet64;

typedef struct
{
    int16 rel;
    float abs;
} StructRelAbs;

#define _bitset(bits)                               \
  ((uint8)(                                         \
  (((uint8)((uint)bits / 01)        % 010) << 0) |  \
  (((uint8)((uint)bits / 010)       % 010) << 1) |  \
  (((uint8)((uint)bits / 0100)      % 010) << 2) |  \
  (((uint8)((uint)bits / 01000)     % 010) << 3) |  \
  (((uint8)((uint)bits / 010000)    % 010) << 4) |  \
  (((uint8)((uint)bits / 0100000)   % 010) << 5) |  \
  (((uint8)((uint)bits / 01000000)  % 010) << 6) |  \
  (((uint8)((uint)bits / 010000000) % 010) << 7)))

#define BINARY_U8( bits ) _bitset(0##bits)

#define DISABLE_RU  "����"
#define DISABLE_EN  "Disable"
#define ENABLE_RU   "���"
#define ENABLE_EN   "Enable"

#define ERROR_VALUE_FLOAT   1.111e29f
#define ERROR_VALUE_INT16   SHRT_MAX
#define ERROR_VALUE_UINT8   255
#define ERROR_VALUE_INT     INT_MAX
#define ERROR_STRING_VALUE  "--.--"
#define M_PI                3.14159265358979323846

#define HARDWARE_ERROR HardwareErrorHandler(__FILE__, __FUNCTION__, __LINE__);
void HardwareErrorHandler(const char *file, const char *function, int line);

#include "Globals.h"
