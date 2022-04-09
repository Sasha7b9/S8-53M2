// 2022/02/11 17:48:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"

#ifdef _MS_VS
#undef _WIN32
#endif

#include <ff.h>

#ifdef _MS_VS
#define _WIN32
#endif

struct File
{
    static const int SIZE_FLASH_TEMP_BUFFER = 512;
    uint8 tempBuffer[SIZE_FLASH_TEMP_BUFFER];
    int sizeData;
    FIL fileObj;
    char name[255];

    // ������� ������� ���� ��� ������. ���� ����� ���� ��� ����������, ����� ���, ������� ����� ������� ����� � ������� ���
    bool OpenNewForWrite(pchar  fullPathToFile);

    bool Write(void *data, int sizeData);

    bool Close();
};


class FileName : public String<>
{
public:
    FileName() : String() { }
    FileName(char *name) : String<>(name) { }

    // ���������� ��������� �� ���������� ��� �����, ���� ��� �������� � ���� ���������� ����
    pchar Extract();
};


struct Directory
{
    String<256> path;
    DIR dir;

    bool GetFirstName(pchar fuulPath, int numDir, char *nameDirOut);

    bool GetNextName(char *nameDirOut);

    bool GetFirstNameFile(pchar fullPath, int numFile, FileName &);

    bool GetNextNameFile(FileName &);

    void Close();

    static void GetNumDirsAndFiles(pchar  fullPath, int *numDirs, int *numFiles);
};


namespace FDrive
{
    extern bool isConnected;
    extern bool needSave;           // ���� true, �� ����� ��������� ����� ��������� �� ������.

    void Init();

    void Update();
};


// ����� ��� �������������� �������� �������� ������ � �������� �����
class DisplayString : public String<>
{
public:
    DisplayString(char *string) : String<>(string) {}

    String<> &Decode();
};
