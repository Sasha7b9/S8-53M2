// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Tables.h"
#include "Display/font/Font.h"


namespace Tables
{
    // ��������� ��� �������� ��������� �������� �� �������.
    struct TBaseStruct
    {
        pchar  name[2];    // �������� ��������� � ��������� ����, ��������� ��� ������ �� �����.
    };


    // ������ �������� �������� ��������� �� �������.
    static const TBaseStruct tBases[TBase::Count] =
    {
        {"2\x10��",     "2\x10ns"},
        {"5\x10��",     "5\x10ns"},
        {"10\x10��",    "10\x10ns"},
        {"20\x10��",    "20\x10ns"},
        {"50\x10��",    "50\x10ns"},
        {"0.1\x10���",  "0.1\x10us"},
        {"0.2\x10���",  "0.2\x10us"},
        {"0.5\x10���",  "0.5\x10us"},
        {"1\x10���",    "1\x10us"},
        {"2\x10���",    "2\x10us"},
        {"5\x10���",    "5\x10us"},
        {"10\x10���",   "10\x10us"},
        {"20\x10���",   "20\x10us"},
        {"50\x10���",   "50\x10us"},
        {"0.1\x10��",   "0.1\x10ms"},
        {"0.2\x10��",   "0.2\x10ms"},
        {"0.5\x10��",   "0.5\x10ms"},
        {"1\x10��",     "1\x10ms"},
        {"2\x10��",     "2\x10ms"},
        {"5\x10��",     "5\x10ms"},
        {"10\x10��",    "10\x10ms"},
        {"20\x10��",    "20\x10ms"},
        {"50\x10��",    "50\x10ms"},
        {"0.1\x10�",    "0.1\x10s"},
        {"0.2\x10�",    "0.2\x10s"},
        {"0.5\x10�",    "0.5\x10s"},
        {"1\x10�",      "1\x10s"},
        {"2\x10�",      "2\x10s"},
        {"5\x10�",      "5\x10s"},
        {"10\x10�",     "10\x10s"}
    };

    // ������� �������� �������� � ������������ ��� ����� ������
    pchar  symbolsAlphaBet[0x48] =
    {
        /* 0x00 */ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /* 0x19 */
        /* 0x1a */ " ", "_", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /* 0x26 */
        /* 0x27 */ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /* 0x40 */
        //         0x41  0x42  0x43  0x44  0x45  0x46   0x47   - ��� ���������� ����� �������� � ���� �������
        /* 0x41 */ "%%y", "%%m", "%%d", "%%H", "%%M", "%%S", "%%N" /* 0x47 */
        //         0x01  0x02  0x03  0x04  0x05  0x06   0x07   - ��� ����� ���������� �������� �������� � gset.memory.fileNameMask
    };
}


pchar TBase::ToString(TBase::E tbase)
{
    return Tables::tBases[tbase].name[LANG];
}


pchar TBase::ToStringEN(TBase::E tbase)
{
    return Tables::tBases[tbase].name[Lang::En];
}


pchar Range::ToName(Range::E range)
{
    static pchar  names[Range::Count] =
    {
        "Range::_2mV",
        "Range::_5mV",
        "Range::_10mV",
        "Range::_20mV",
        "Range::_50mV",
        "Range::_100mV",
        "Range::_200mV",
        "Range::_500mV",
        "Range::_1V",
        "Range::_2V",
        "Range::_5V",
        "Range::_10V",
        "Range::_20V"
    };
    return names[range];
}


void Tables::DrawStr(int index, int x, int y)
{
    pchar str = symbolsAlphaBet[index];

    if (index == INDEX_SYMBOL)
    {
        Region(Font::GetLengthText(str), 9).Fill(x - 1, y, Color::FLASH_10);
    }

    String<>(symbolsAlphaBet[index]).Draw(x, y, index == INDEX_SYMBOL ? Color::FLASH_01 : COLOR_FILL);
}
