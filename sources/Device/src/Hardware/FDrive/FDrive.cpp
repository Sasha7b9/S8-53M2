// 2022/02/11 17:48:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Menu/FileManager.h"
#include "Menu/Menu.h"
#include <usbh_def.h>
#include <ff_gen_drv.h>
#include <usbh_diskio.h>
#include <usbh_core.h>
#include <usbh_msc.h>
#include "ffconf.h"
#include "FDrive.h"
#include "Hardware/HAL/HAL.h"



static FATFS USBDISKFatFs;
static char USBDISKPath[4];


static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8 id);


namespace FDrive
{
    bool isConnected = false;
    bool needSave = false;
}


void FDrive::Init()
{
    if(FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == FR_OK) 
    {
        USBH_Init((USBH_HandleTypeDef *)HAL_USBH::handle, USBH_UserProcess, 0);
        USBH_RegisterClass((USBH_HandleTypeDef *)HAL_USBH::handle, USBH_MSC_CLASS);
        USBH_Start((USBH_HandleTypeDef *)HAL_USBH::handle);
    }
    else
    {
        LOG_ERROR_TRACE("Can not %s", __FUNCTION__);
    }
}


void FDrive::Update()
{
    USBH_Process((USBH_HandleTypeDef *)HAL_USBH::handle);
}


void USBH_UserProcess(USBH_HandleTypeDef *, uint8 id)
{
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_CLASS_ACTIVE:
            FDrive::isConnected = true;
            FM::Init();
            Menu::ChangeStateFlashDrive();
            if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
            {
                LOG_ERROR_TRACE("Не могу примонтировать диск");
            }
            break;
        case HOST_USER_CLASS_SELECTED:
            break;
        case HOST_USER_CONNECTION:
            f_mount(NULL, (TCHAR const*)"", 0);
            break;
        case HOST_USER_DISCONNECTION:
            FDrive::isConnected = false;
            Menu::ChangeStateFlashDrive();
            break;
        default:
            break;
    }
}


bool FDrive::AppendStringToFile(pchar)
{
    return false;
}


void WriteToFile(FIL *file, char *string)
{
    //    uint bytesWritten;
    f_open(file, "list.txt", FA_OPEN_EXISTING);
    //f_write(file, string, strlen(string), (void*)&bytesWritten);
    f_puts(string, file);
    f_close(file);
}


void FDrive::GetNumDirsAndFiles(pchar  fullPath, int *numDirs, int *numFiles)
{
    FILINFO fno;
    DIR dir;

    *numDirs = 0;
    *numFiles = 0;
    

    char nameDir[_MAX_LFN + 1];
    memcpy(nameDir, fullPath, strlen(fullPath));
    nameDir[strlen(fullPath)] = '\0';

    fno.fname[0] = '\0';
    fno.fsize = 0;

    if (f_opendir(&dir, nameDir) == FR_OK)
    {
        int numReadingElements = 0;
        bool alreadyNull = false;
        while (true)
        {
            if (f_readdir(&dir, &fno) != FR_OK)
            {
                break;
            }
            if (fno.fname[0] == 0)
            {
                if(alreadyNull)
                {
                    break;
                }
                alreadyNull = true;
                continue;
            }
            numReadingElements++;

            if (fno.fname[0] != '.')
            {
                if (fno.fattrib & AM_DIR)
                {
                    (*numDirs)++;
                }
                else
                {
                    (*numFiles)++;
                }
            }
        }
        f_closedir(&dir);
    }
}


bool Directory::GetName(pchar fullPath, int numDir, char *nameDirOut)
{
    memcpy(nameDir, fullPath, strlen(fullPath));
    nameDir[strlen(fullPath)] = '\0';

    strcpy(fno.fname, lfn);
    fno.fsize = sizeof(lfn);

    DIR *pDir = &dir;
    if (f_opendir(pDir, nameDir) == FR_OK)
    {
        int numDirs = 0;
        FILINFO *pFNO = &fno;
        bool alreadyNull = false;
        while (true)
        {
            if (f_readdir(pDir, pFNO) != FR_OK)
            {
                *nameDirOut = '\0';
                f_closedir(pDir);
                return false;
            }
            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameDirOut = '\0';
                    f_closedir(pDir);
                    return false;
                }
                alreadyNull = true;
            }

            if (numDir == numDirs && (pFNO->fattrib & AM_DIR))
            {
                strcpy(nameDirOut, pFNO->fname);
                return true;
            }
            if ((pFNO->fattrib & AM_DIR) && (pFNO->fname[0] != '.'))
            {
                numDirs++;
            }
        }
    }
    return false;
}


bool Directory::GetNextName(char *nameDirOut)
{
    DIR *pDir = &dir;
    FILINFO *pFNO = &fno;
    bool alreadyNull = false;
    while (true)
    {
        if (f_readdir(pDir, pFNO) != FR_OK)
        {
            *nameDirOut = '\0';
            f_closedir(pDir);
            return false;
        }
        else if (pFNO->fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameDirOut = '\0';
                f_closedir(pDir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {

            if (pFNO->fattrib & AM_DIR)
            {
                strcpy(nameDirOut, pFNO->fname);
                return true;
            }
        }
    }
}


void Directory::CloseCurrent()
{
    f_closedir(&dir);
}


bool Directory::GetNameFile(pchar fullPath, int numFile, char *nameFileOut)
{
    memcpy(nameDir, fullPath, strlen(fullPath));
    nameDir[strlen(fullPath)] = '\0';

    strcpy(fno.fname, lfn);
    fno.fsize = sizeof(lfn);

    DIR *pDir = &dir;
    FILINFO *pFNO = &fno;

    if (f_opendir(pDir, nameDir) == FR_OK)
    {
        int numFiles = 0;
        bool alreadyNull = false;
        while (true)
        {
            if (f_readdir(pDir, pFNO) != FR_OK)
            {
                *nameFileOut = '\0';
                f_closedir(pDir);
                return false;
            }
            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameFileOut = '\0';
                    f_closedir(pDir);
                    return false;
                }
                alreadyNull = true;
            }

            if (numFile == numFiles && (pFNO->fattrib & AM_DIR) == 0)
            {
                strcpy(nameFileOut, pFNO->fname);
                return true;
            }
            if ((pFNO->fattrib & AM_DIR) == 0 && (pFNO->fname[0] != '.'))
            {
                numFiles++;
            }
        }
    }
    return false;
}


bool Directory::GetNextNameFile(char *nameFileOut)
{
    FILINFO *pFNO = &fno;
    bool alreadyNull = false;

    while (true)
    {
        if (f_readdir(&dir, &fno) != FR_OK)
        {
            *nameFileOut = '\0';
            f_closedir(&dir);
            return false;
        }
        if (fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameFileOut = '\0';
                f_closedir(&dir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {
            if ((pFNO->fattrib & AM_DIR) == 0 && pFNO->fname[0] != '.')
            {
                strcpy(nameFileOut, pFNO->fname);
                return true;
            }
        }
    }
}


bool File::OpenNewForWrite(pchar  fullPathToFile)
{
    if (f_open(&fileObj, fullPathToFile, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        return false;
    }

    strcpy(name, fullPathToFile);
    sizeData = 0;
    return true;
}


bool File::Write(uint8* data, int size)
{
    while (size > 0)
    {
        int dataToCopy = size;

        if (size + sizeData > SIZE_FLASH_TEMP_BUFFER)
        {
            dataToCopy = SIZE_FLASH_TEMP_BUFFER - sizeData;
        }

        size -= dataToCopy;
        memcpy(tempBuffer + sizeData, data, (uint)dataToCopy);
        data += dataToCopy;
        sizeData += dataToCopy;

        if (sizeData == SIZE_FLASH_TEMP_BUFFER)
        {
            uint wr = 0;
            if (f_write(&fileObj, tempBuffer, (uint)sizeData, &wr) != FR_OK || (uint)sizeData != wr)
            {
                return false;
            }
            sizeData = 0;
        }
    }

    return true;
}


bool File::Close()
{
    if (sizeData != 0)
    {
        uint wr = 0;

        if (f_write(&fileObj, tempBuffer, (uint)sizeData, &wr) != FR_OK || (uint)sizeData != wr)
        {
            f_close(&fileObj);
            return false;
        }
    }

    f_close(&fileObj);

    FILINFO fno;
    PackedTime time = HAL_RTC::GetPackedTime();
    fno.fdate = (WORD)(((time.year + 20) * 512) | (time.month * 32) | time.day);
    fno.ftime = (WORD)((time.hours * 2048) | (time.minutes * 32) | (time.seconds / 2));
    f_utime(name, &fno);

    return true;
}
