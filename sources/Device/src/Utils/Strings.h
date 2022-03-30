// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Text.h"
#include "Display/String.h"


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
    String<> Bin2String(uint8 value);

    String<> Bin2String16(uint16 value);

    String<> FloatFract2String(float value, bool alwaysSign);

    String<> Float2String(float value, bool alwaysSign, int numDigits);

    String<> Hex8toString(uint8 value);

    String<> Int2String(int value, bool alwaysSign, int numMinFields);

    String<> Voltage2String(float voltage, bool alwaysSign);
    String<> Time2String(float time, bool alwaysSign);
    String<> Freq2String(float freq, bool alwaysSign);
    String<> Phase2String(float phase, bool alwaysSign);
    String<> Float2Db(float value, int numDigits);

    bool   String2Int(char *str, int *value);

    void LogBufferU8(const uint8 *data, int num);
    void LogBufferU8(pchar label, const uint8 *data, int num);
    void LogBufferF(pchar label, const float *data, int num);
};
