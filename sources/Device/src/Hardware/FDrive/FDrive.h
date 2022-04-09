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

class FileName : public String<>
{
public:
    FileName() : String() { }
    FileName(char *name) : String<>(name) { }

    // ���������� ��������� �� ���������� ��� �����, ���� ��� �������� � ���� ���������� ����
    pchar Extract();
};


struct File
{
    static const int SIZE_BUFFER = 512;

    uint8    buffer[SIZE_BUFFER];
    int      sizeData;
    FIL      fileObj;
    FileName name;

    ~File();

    // ������� ������� ���� ��� ������. ���� ����� ���� ��� ����������, ����� ���, ������� ����� ������� ����� � ������� ���
    bool OpenNewForWrite(pchar  fullPathToFile);

    bool Write(void *data, int sizeData);
};


struct Directory
{
    DIR dir;

    ~Directory();

    bool GetFirstDir(pchar fuulPath, int numDir, char *nameDirOut);

    bool GetNextDir(char *nameDirOut);

    bool GetFirstFile(pchar fullPath, int numFile, FileName &);

    bool GetNextFile(FileName &);

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
