// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "main.h"


void FDrive_Init(void);
bool FDrive_Update(void);
bool FDrive_FileExist(char *fileName);
int FDrive_OpenFileForRead(char *fileName);
/// Считывает из открытого файла numBytes байт. Возвращает число реально считанных байт
int FDrive_ReadFromFile(int numBytes, uint8 *buffer);
void FDrive_CloseOpenedFile(void);
