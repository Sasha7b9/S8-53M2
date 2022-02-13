// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Word
{
    uint8*   address;
    int8    numSymbols;
};


int GetNumWordsInString(const uint8 *string);                       // ���������� ���������� ���� � ������. ����������� - �������. ������ ������������� ������� 0x0d, 0x0a.
bool GetWord(const uint8 *string, Word *word, const int numWord);   // ��� ������� ����� ����������� � �������� �������� �����.
bool WordEqualZeroString(Word *word, char* string);
bool EqualsStrings(char *str1, char *str2, int size);
bool EqualsZeroStrings(char *str1, char *str2);
