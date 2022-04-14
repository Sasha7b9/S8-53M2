#pragma once
#include "stm32f4xx_hal.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>


#ifdef WIN32
#define USBD_CLASS_BOS_ENABLED 0
#ifndef __STATIC_INLINE
    #define __STATIC_INLINE inline
#endif
#endif


#define USBD_MAX_NUM_INTERFACES               1
#define USBD_MAX_NUM_CONFIGURATION            1
#define USBD_MAX_STR_DESC_SIZ                 0x100
#define USBD_SUPPORT_USER_STRING              0
#define USBD_SELF_POWERED                     1
#define USBD_DEBUG_LEVEL                      0

#define USBD_LPM_ENABLED 0
 
#define USBD_malloc         std::malloc
#define USBD_free           std::free
#define USBD_memset         std::memset
#define USBD_memcpy         std::memcpy
    
#if (USBD_DEBUG_LEVEL > 0)
#define  USBD_UsrLog(...)   printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)

#define  USBD_ErrLog(...)   printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 2)
#define  USBD_DbgLog(...)   printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)
#endif
