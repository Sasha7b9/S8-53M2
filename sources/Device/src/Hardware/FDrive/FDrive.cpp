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

    void ChangeState();
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
            FM::Init();
            if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
            {
                FDrive::isConnected = false;
            }
            else
            {
                FDrive::isConnected = true;
            }
            FDrive::ChangeState();
            break;
        case HOST_USER_CLASS_SELECTED:
            break;
        case HOST_USER_CONNECTION:
            f_mount(NULL, (TCHAR const*)"", 0);
            break;
        case HOST_USER_DISCONNECTION:
            FDrive::isConnected = false;
            FDrive::ChangeState();
            break;
        default:
            break;
    }
}


void WriteToFile(FIL *file, char *string)
{
    uint bytesWritten;
    f_open(file, "list.txt", FA_OPEN_EXISTING);
    f_write(file, string, strlen(string), &bytesWritten);
    f_close(file);
}


void Directory::GetNumDirsAndFiles(pchar  fullPath, int *numDirs, int *numFiles)
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

    f_closedir(&dir);
}


bool Directory::GetFirstName(pchar fullPath, int numDir, char *nameDirOut)
{
    path.SetFormat(fullPath);

    strcpy(fno.fname, path.c_str());

    if (f_opendir(&dir, path.c_str()) == FR_OK)
    {
        int numDirs = 0;
        FILINFO *pFNO = &fno;
        bool alreadyNull = false;

        while (true)
        {
            if (f_readdir(&dir, pFNO) != FR_OK)
            {
                *nameDirOut = '\0';
                f_closedir(&dir);
                return false;
            }

            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameDirOut = '\0';
                    f_closedir(&dir);
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

    f_closedir(&dir);

    return false;
}


bool Directory::GetNextName(char *nameDirOut)
{
    bool alreadyNull = false;

    while (true)
    {
        if (f_readdir(&dir, &fno) != FR_OK)
        {
            *nameDirOut = '\0';
            f_closedir(&dir);
            return false;
        }
        else if (fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameDirOut = '\0';
                f_closedir(&dir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {

            if (fno.fattrib & AM_DIR)
            {
                strcpy(nameDirOut, fno.fname);
                return true;
            }
        }
    }
}


void Directory::Close()
{
    f_closedir(&dir);
}


bool Directory::GetFirstNameFile(pchar fullPath, int numFile, FileName &fileName)
{
    path.SetFormat(fullPath);

    strcpy(fno.fname, path.c_str());

    if (f_opendir(&dir, path.c_str()) == FR_OK)
    {
        int numFiles = 0;
        bool alreadyNull = false;

        while (true)
        {
            if (f_readdir(&dir, &fno) != FR_OK)
            {
                fileName.SetFormat("");
                f_closedir(&dir);
                return false;
            }

            if (fno.fname[0] == 0)
            {
                if (alreadyNull)
                {
                    fileName.SetFormat("");
                    f_closedir(&dir);
                    return false;
                }

                alreadyNull = true;
            }

            if (numFile == numFiles && (fno.fattrib & AM_DIR) == 0)
            {
                fileName.SetFormat(fno.fname);
                return true;
            }
            if ((fno.fattrib & AM_DIR) == 0 && (fno.fname[0] != '.'))
            {
                numFiles++;
            }
        }
    }

    f_closedir(&dir);

    return false;
}


bool Directory::GetNextNameFile(FileName &fileName)
{
    bool alreadyNull = false;

    while (true)
    {
        if (f_readdir(&dir, &fno) != FR_OK)
        {
            fileName.SetFormat("");
            f_closedir(&dir);
            return false;
        }
        if (fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                fileName.SetFormat("");
                f_closedir(&dir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {
            if ((fno.fattrib & AM_DIR) == 0 && fno.fname[0] != '.')
            {
                fileName.SetFormat(fno.fname);
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


bool File::Write(void *_data, int size)
{
    uint8 *data = (uint8 *)_data;

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



String<> &DisplayString::Decode()
{
    char *text = c_str();

    while (*text)
    {
        char symbol = *text;

        if (symbol > 127)
        {
            if (symbol == 240) { *text = 'Å'; }
            else if (symbol == 241) { *text = (char)0xb8; }
            else if (symbol >= 224)
            {
                *text += 16;
            }
            else
            {
                *text += 64;
            }
        }

        text++;
    }

    return *this;
}


void FDrive::ChangeState()
{
    if (!FDrive::isConnected)
    {
        if (Page::NameOpened() == NamePage::SB_FileManager)
        {
            Item::Opened()->CloseOpened();
        }
    }
    else if (FLASH_AUTOCONNECT)
    {
        FM::needOpen = true;
    }
}


pchar FileName::Extract()
{
    if (Size() == 0)
    {
        return nullptr;
    }

    pchar pointer = &c_str()[Size()];

    do
    {
        pointer--;

        if (*pointer == '\\' || *pointer == '/')
        {
            return pointer + 1;
        }

    } while (pointer != c_str());

    return pointer;
}
