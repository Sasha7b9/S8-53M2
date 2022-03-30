// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Text.h"


struct Word
{
    uint8 *address;
    int8   numSymbols;
};


int GetNumWordsInString(const uint8 *string);                       // Возвращает количество слов в строке. Разделители - пробелы. Строка заканчивается байтами 0x0d, 0x0a.
bool GetWord(const uint8 *string, Word *word, const int numWord);   // Эта команда сразу преобразует к верхенму регистру слово.
bool WordEqualZeroString(Word *word, char* string);
bool EqualsStrings(char *str1, char *str2, int size);
bool EqualsZeroStrings(char *str1, char *str2);


namespace SU
{
    DString Bin2String(uint8 value);

    DString Bin2String16(uint16 value);

    DString FloatFract2String(float value, bool alwaysSign);

    DString Float2String(float value, bool alwaysSign, int numDigits);

    DString Hex8toString(uint8 value);

    DString Int2String(int value, bool alwaysSign, int numMinFields);

    DString Voltage2String(float voltage, bool alwaysSign);
    DString Time2String(float time, bool alwaysSign);
    DString Freq2String(float freq, bool alwaysSign);
    DString Phase2String(float phase, bool alwaysSign);
    DString Float2Db(float value, int numDigits);

    bool   String2Int(char *str, int *value);

    void LogBufferU8(const uint8 *data, int num);
    void LogBufferU8(pchar label, const uint8 *data, int num);
    void LogBufferF(pchar label, const float *data, int num);
};
