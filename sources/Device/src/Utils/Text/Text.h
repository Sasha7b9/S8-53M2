// 2022/02/15 09:14:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Utils/Text/String.h"


class Char
{
public:
    Char(char s) : symbol(s) {}
    int Draw(int x, int y, Color::E = Color::Count);
private:
    char symbol;
};


struct Word
{
    uint8 *address;
    int8   numSymbols;

    // ��� ������� ����� ����������� � �������� �������� �����.
    bool GetWord(const uint8 *string, const int numWord);
};


// ���������� ���������� ���� � ������. ����������� - �������. ������ ������������� ������� 0x0d, 0x0a.
int GetNumWordsInString(const uint8 *string);



namespace Text
{
    // ���������� ������ ������, ������� ����� ����� text, ��� ������ �� left �� right � ���������� height. ����
    // bool == false, �� ����� �� ������ �� ����� 
    bool GetHeightTextWithTransfers(int left, int top, int right, pchar text, int *height);

    int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight);

    void Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

    void Draw4SymbolsInRect(int x, int y, char eChar, Color::E = Color::Count);

    void Draw10SymbolsInRect(int x, int y, char eChar);

    void DrawBig(int x, int y, int size, pchar text);

    String<> GetWord(pchar firstSymbol, int *length);

    // ���� draw == false, �� �������� ������ �� ����, ������ ������������ ������ ��� ����������
    int DrawPartWord(char *word, int x, int y, int xRight, bool draw);
}


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

    bool WordEqualZeroString(Word *word, char *string);
    bool EqualsStrings(char *str1, char *str2, int size);
    bool EqualsZeroStrings(char *str1, char *str2);

    void LogBufferU8(const uint8 *data, int num);
    void LogBufferU8(pchar label, const uint8 *data, int num);
    void LogBufferF(pchar label, const float *data, int num);
};
