// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifdef WIN32
    #define __attribute__(x)
    #define __packed__
#else
    #define nullptr 0 //-V1059
    #pragma anon_unions
#endif


#define NUM_VER "1.3"


typedef signed      char        int8;
typedef signed      short   int int16;
typedef unsigned    char        uint8;
typedef unsigned    short   int uint16;
typedef unsigned    int         uint;
typedef unsigned long long int  uint64;
typedef unsigned    char        uchar;
typedef const char             *pchar;


typedef void    (*pFuncVV)();       // ”казатель на функцию, ничего не принимающую и ничего не возвращающую.
typedef void    (*pFuncVpV)(void*); // ”казатель на функцию, принимающую указатель на void и ничего не возвращающую.
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


union BitSet16
{
    uint16 half_word;
    uint8  byte[2];
    struct
    {
        uint8  byte0;
        uint8  byte1;
    };
};

// ќбъединение размером 32 бита
union BitSet32
{
    uint   word;
    union
    {
        uint16 half_word[2];
        int16  half_iword[2];
    };
    uint8  byte[4]; //-V112
};

// ќбъединение размером 64 бита
union BitSet64
{
    long long unsigned int dword;
    unsigned int           word[2];
};

struct StructRelAbs
{
    int16 rel;
    float abs;
};

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

#define DISABLE_RU  "ќткл"
#define DISABLE_EN  "Disable"
#define ENABLE_RU   "¬кл"
#define ENABLE_EN   "Enable"

#define ERROR_VALUE_FLOAT   1.111e29f
#define ERROR_VALUE_INT16   SHRT_MAX
#define ERROR_VALUE_UINT8   255
#define ERROR_VALUE_INT     INT_MAX
#define ERROR_STRING_VALUE  "--.--"
#define M_PI                3.14159265358979323846

#define ERROR_HANDLER() HardwareErrorHandler(__FILE__, __FUNCTION__, __LINE__)
void HardwareErrorHandler(pchar file, pchar function, int line);

#include "Globals.h"
#include "Log.h"
//#include "Utils/Debug.h"
