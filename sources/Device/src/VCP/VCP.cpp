#include "defines.h"
#include "VCP.h"
#include "Log.h"
#include "Utils/Math.h"
#include "SCPI/SCPI.h"
#include "usbd_desc.h"
#include "usbd_cdc_interface.h"
#include "Hardware/Timer.h"
#include <usbd_cdc.h>
#include <usbd_def.h>
#include <cstdarg>
#include <cstring>
#include <cstdio>


namespace VCP
{
    static void SendDataAsinch(uint8 *buffer, int size);

    static bool PrevSendingComplete();

    static const int SIZE_BUFFER_VCP = 256;
    static uint8 buffSend[SIZE_BUFFER_VCP];
    static int sizeBuffer = 0;

    static USBD_HandleTypeDef _handleUSBD;
    static PCD_HandleTypeDef _handlePCD;

    void *handlePCD = &_handlePCD;
    void *handleUSBD = &_handleUSBD;

    bool clientIsConnected = false;
    bool cableIsConnected = false;
}


void VCP::Init()
{
    USBD_Init(&_handleUSBD, &VCP_Desc, 0);
    USBD_RegisterClass(&_handleUSBD, &USBD_CDC);
    USBD_CDC_RegisterInterface(&_handleUSBD, &USBD_CDC_fops);
    USBD_Start(&_handleUSBD);
} 

static bool VCP::PrevSendingComplete()
{
    USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)_handleUSBD.pClassData;
    return pCDC->TxState == 0;
}

void VCP::SendDataAsinch(uint8 *buffer, int size)
{
    const int SIZE_BUFFER = 64;
    static uint8 trBuf[SIZE_BUFFER];

    size = Math_MinInt(size, SIZE_BUFFER);
    while (!PrevSendingComplete())  {};
    memcpy(trBuf, buffer, (uint)size);

    USBD_CDC_SetTxBuffer(&_handleUSBD, trBuf, (uint16)size);
    USBD_CDC_TransmitPacket(&_handleUSBD);
}


void VCP::Flush()
{
    if (sizeBuffer)
    {
        USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)_handleUSBD.pClassData;
        while (pCDC->TxState == 1) {};
        USBD_CDC_SetTxBuffer(&_handleUSBD, buffSend, (uint16)sizeBuffer);
        USBD_CDC_TransmitPacket(&_handleUSBD);
        while (pCDC->TxState == 1) {};
    }
    sizeBuffer = 0;
}


void VCP::SendDataSynch(const uint8 *buffer, int size)
{
    if (VCP::clientIsConnected == false)
    {
        return;
    }

    USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)_handleUSBD.pClassData;

    do 
    {
        if (sizeBuffer + size > SIZE_BUFFER_VCP)
        {
            int reqBytes = SIZE_BUFFER_VCP - sizeBuffer;
            LIMITATION(reqBytes, reqBytes, 0, size);
            while (pCDC->TxState == 1) {};
            memcpy(buffSend + sizeBuffer, buffer, (uint)reqBytes);
            USBD_CDC_SetTxBuffer(&_handleUSBD, buffSend, SIZE_BUFFER_VCP);
            USBD_CDC_TransmitPacket(&_handleUSBD);
            size -= reqBytes;
            buffer += reqBytes;
            sizeBuffer = 0;
        }
        else
        {
            memcpy(buffSend + sizeBuffer, buffer, (uint)size);
            sizeBuffer += size;
            size = 0;
        }
    } while (size);
}


void SendData(const uint8 *, int)
{

}


void VCP::SendStringSynch(char *data)
{
    SendDataSynch((uint8*)data, (int)strlen(data));
}


void VCP::SendStringAsynch(char *format, ...)
{
    static const int SIZE_BUFFER = 200;
    static char buffer[SIZE_BUFFER];

    std::va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    strcat(buffer, "\n");

    SendDataAsinch((uint8*)buffer, (int)strlen(buffer));
}


void VCP::SendFormatStringSynch(char *format, ...) {
    static const int SIZE_BUFFER = 200;
    char buffer[SIZE_BUFFER];
    std::va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    strcat(buffer, "\n");
    SendDataSynch((uint8*)buffer, (int)strlen(buffer));
}
