// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Display/String.h"


namespace GF
{
    char *Bin2String(uint8 value);

    char *Bin2String16(uint16 value);
}



String FloatFract2String(float value, bool alwaysSign);

String Float2String(float value, bool alwaysSign, int numDigits);                         

String Hex8toString(uint8 value);

String Int2String(int value, bool alwaysSign, int numMinFields);

bool    String2Int(char *str, int *value);
String Voltage2String(float voltage, bool alwaysSign);
String Time2String(float time, bool alwaysSign);
String Freq2String(float freq, bool alwaysSign);
String Phase2String(float phase, bool alwaysSign);
String Float2Db(float value, int numDigits);
bool    IntInRange                              // ���������� true, ���� value ������ � �������� [min; max].
                   (int value,                  // ����������� ��������.
                   int min,                     // ������ ������� ���������.
                   int max                      // ������� ������� ���������.
                   );
float   MaxFloat(float val1, float val2, float val3);                   // ���������� ������������ �������� �� ���.
int8    CircleIncreaseInt8(int8 *val, int8 min, int8 max);              // ����������� �������� �� ������ val �� 1. �����, ���� ��������� ��������� max, ������������ ��� � min.
int16   CircleIncreaseInt16(int16 *val, int16 min, int16 max);
int8    CircleDecreaseInt8(int8 *val, int8 min, int8 max);              // �������� �������� �� ������ val �� 1. �����, ���� ��������� ������ min, ������������ ��� max.
int16   CircleDecreaseInt16(int16 *val, int16 min, int16 max);
int     CircleIncreaseInt(int *val, int min, int max);
int     CircleDecreaseInt(int *val, int min, int max);
float   CircleAddFloat(float *val, float delta, float min, float max);  // ����������� �������� �� ������ val �� delta. �����, ���� ��������� ������ max, ������������ ��� min.
float   CircleSubFloat(float *val, float delta, float min, float max);  // ��������� �������� �� ������ val �� delta. �����, ���� ��������� ������ min, ������������ ��� masx.
void    SwapInt(int *value0, int *value1);                      // ���������� ������� ���������� ������ �� ������� value0 � value1.
void    SortInt(int *value0, int *value1);                      // ��������� ���������� value0 � value1 � ������� �����������.
char    GetSymbolForGovernor(int value);                        // ���������� ����������� ����������, ��������������� ��� �������� ���������.
void    EmptyFuncVV();                                      // �������-��������. ������ �� ������.
void    EmptyFuncVI(int);
void    EmptyFuncVpV(void *);                                   // �������-��������. ������ �� ������.
void    EmptyFuncpVII(void *, int, int);                        // �������-��������. ������ �� ������.
void    EmptyFuncVI16(int16);                                   // �������-��������. ������ �� ������.
void    EmptyFuncVB(bool);                                      // �������-��������. ������ �� ������.
bool    EmptyFuncBV();
void    IntToStrCat(char *_buffer, int _value);
