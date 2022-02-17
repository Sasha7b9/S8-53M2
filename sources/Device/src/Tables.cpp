// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Tables.h"
#include "Utils/GlobalFunctions.h"
#include "Display/font/Font.h"


// Структура для описания диапазона масштаба по времени.
struct TBaseStruct
{
    pchar  name[2];    // Название диапазона в текстовом виде, пригодном для вывода на экран.
};


// Массив структур описаний масштабов по времени.
static const TBaseStruct tBases[TBase::Count] =
{
    {"2\x10нс",     "2\x10ns"},
    {"5\x10нс",     "5\x10ns"},
    {"10\x10нс",    "10\x10ns"},
    {"20\x10нс",    "20\x10ns"},
    {"50\x10нс",    "50\x10ns"},
    {"0.1\x10мкс",  "0.1\x10us"},
    {"0.2\x10мкс",  "0.2\x10us"},
    {"0.5\x10мкс",  "0.5\x10us"},
    {"1\x10мкс",    "1\x10us"},
    {"2\x10мкс",    "2\x10us"},
    {"5\x10мкс",    "5\x10us"},
    {"10\x10мкс",   "10\x10us"},
    {"20\x10мкс",   "20\x10us"},
    {"50\x10мкс",   "50\x10us"},
    {"0.1\x10мс",   "0.1\x10ms"},
    {"0.2\x10мс",   "0.2\x10ms"},
    {"0.5\x10мс",   "0.5\x10ms"},
    {"1\x10мс",     "1\x10ms"},
    {"2\x10мс",     "2\x10ms"},
    {"5\x10мс",     "5\x10ms"},
    {"10\x10мс",    "10\x10ms"},
    {"20\x10мс",    "20\x10ms"},
    {"50\x10мс",    "50\x10ms"},
    {"0.1\x10с",    "0.1\x10s"},
    {"0.2\x10с",    "0.2\x10s"},
    {"0.5\x10с",    "0.5\x10s"},
    {"1\x10с",      "1\x10s"},
    {"2\x10с",      "2\x10s"},
    {"5\x10с",      "5\x10s"},
    {"10\x10с",     "10\x10s"}
};

// Таблица символов алфавита и спецсимволов для ввода текста
pchar  symbolsAlphaBet[0x48] =
{
    /* 0x00 */ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /* 0x19 */
    /* 0x1a */ " ", "_", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /* 0x26 */
    /* 0x27 */ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /* 0x40 */
    //         0x41  0x42  0x43  0x44  0x45  0x46   0x47   - это порядковый номер элемента в этом массиве
    /* 0x41 */ "%y", "%m", "%d", "%H", "%M", "%S", "%N" /* 0x47 */
    //         0x01  0x02  0x03  0x04  0x05  0x06   0x07   - под этими значениями элементы хранятся в set.memory.fileNameMask
};


ENumSignalsInSec::E Tables_NumSignalsInSecToENUM(int enumSignalsInSec)
{
    if(enumSignalsInSec == 1)
    {
        return ENumSignalsInSec::_1;
    }
    else if(enumSignalsInSec == 2)
    {
        return ENumSignalsInSec::_2;
    }
    else if(enumSignalsInSec == 5)
    {
        return ENumSignalsInSec::_5;
    }
    else if(enumSignalsInSec == 10)
    {
        return ENumSignalsInSec::_10;
    }
    else if(enumSignalsInSec == 25)
    {
        return ENumSignalsInSec::_25;
    }
    LOG_ERROR("Число сигналов в секунду равно %d", enumSignalsInSec);
    return ENumSignalsInSec::_1;
}


int Tables_ENUMtoNumSignalsInSec(ENumSignalsInSec::E enumSignalsInSec)
{
    static const int fps[] = {25, 10, 5, 2, 1};
    if(enumSignalsInSec <= ENumSignalsInSec::_1)
    {
        return fps[enumSignalsInSec];
    }
    LOG_ERROR("Неправильный параметр %d", (int)enumSignalsInSec);
    return 1;
}


pchar  Tables_GetWarning(Warning warning)
{
    static pchar warnings[Warning_NumWarnings][2] =
    {
        {"ПРЕДЕЛ КАНАЛ 1 - ВОЛЬТ/ДЕЛ",      "LIMIT CHANNEL 1 - VOLTS/DIV"},
        {"ПРЕДЕЛ КАНАЛ 2 - ВОЛЬТ/ДЕЛ",      "LIMIT CHANNEL 2 - VOLTS/DIV"},
        {"ПРЕДЕЛ РАЗВЕРТКА - ВРЕМЯ/ДЕЛ",    "LIMIT SWEEP - TIME/DIV"},
        {"ВКЛЮЧЕН ПИКОВЫЙ ДЕТЕКТОР",        "SET_ENABLED PEAK. DET."},
        {"ПРЕДЕЛ КАНАЛ 1 - \x0d",           "LIMIT CHANNEL 1 - \x0d"},
        {"ПРЕДЕЛ КАНАЛ 2 - \x0d",           "LIMIT CHANNEL 2 - \x0d"},
        {"ПРЕДЕЛ РАЗВЕРТКА - УРОВЕНЬ",      "LIMIT SWEEP - LEVEL"},
        {"ПРЕДЕЛ РАЗВЕРТКА - \x97",         "LIMIT SWEEP - \x97"},
        {"ПИК. ДЕТ. НЕ РАБОТАЕТ НА РАЗВЕРТКАХ МЕНЕЕ 0.5мкс/дел", "PEAK. DET. NOT WORK ON SWEETS LESS THAN 0.5us/div"},
        {"САМОПИСЕЦ НЕ МОЖЕТ РАБОТАТЬ НА БЫСТРЫХ РАЗВЁРТКАХ", "SELF-RECORDER DOES NOT WORK FAST SCANS"},
        {"ФАЙЛ СОХРАНЕН",                   "FILE IS SAVED"},
        {"СИГНАЛ СОХРАНЕН",                 "SIGNAL IS SAVED"},
        {"СИГНЛА УДАЛЕН",                   "SIGNAL IS DELETED"},
        {"МЕНЮ ОТЛАДКА ВКЛЮЧЕНО",           "MENU DEBUG IS SET_ENABLED"},
        {"ПОДОЖДИТЕ НЕСКОЛЬКО СЕКУНД, ИДЕТ ПЕРЕМЕЩЕНИЕ ДАННЫХ", "WAIT A FEW SECONDS, GOES A MOVEMENT DATA"},
        {"ВРЕМЯ НЕ УСТАНОВЛЕНО. МОЖЕТЕ УСТАНОВИТЬ ЕГО СЕЙЧАС", "TIME IS NOT SET. YOU CAN INSTALL IT NOW"},
        {"СИГНАЛ НЕ НАЙДЕН",                "SIGNAL IS NOT FOUND"},
        {"НА РАЗВЕРТКАХ МЕДЛЕННЕЕ 10мс/дел ЖЕЛАТЕЛЬНО УСТАНАВ-", "AT SCANNER SLOWLY 10ms/div DESIRABLY SET \"SCAN - \x7b\" IN"},
        {"ЛИВАТЬ \"РАЗВЕРТКА - \x7b\" В ПОЛОЖЕНИЕ \"Лево\" ДЛЯ УСКОРЕ-", "SWITCH \"Left\" FOR TO ACCELERATE THE OUTPUT SIGNAL"},
        {"НИЯ ВЫВОДА СИГНАЛА",              ""},
        {"ОЧЕНЬ МАЛО ИЗМЕРЕНИЙ",            "VERY SMALL MEASURES"},
        {"ДЛЯ ВСТУПЛЕНИЯ ИЗМЕНЕНИЙ В СИЛУ", "FOR THE INTRODUCTION OF CHANGES"},
        {"ВЫКЛЮЧИТЕ ПРИБОР",                "IN FORCE SWITCH OFF THE DEVICE"},
        {"ОТКЛЮЧИТЕ ВЫЧИСЛЕНИЕ БПФ",        "DISCONNECT CALCULATION OF FFT"},
        {"ОТКЛЮЧИТЕ МАТЕМАТИЧЕСКУЮ ФУНКЦИЮ", "DISCONNECT MATHEMATICAL FUNCTION"},
        {"ПРОШИВКА СОХРАНЕНА",              "FIRMWARE SAVED"},
        {"Память OTP полностью заполена",   "OPT memory fully completed"}
    };
    return warnings[warning][set.common.lang];
}


pchar  Tables_GetTBaseString(TBase::E tBase)
{
    return tBases[tBase].name[set.common.lang];
}


pchar  Tables_GetTBaseStringEN(TBase::E tBase)
{
    return tBases[tBase].name[English];
}


pchar  Tables_RangeNameFromValue(Range::E range)
{
    static pchar  names[Range::Count] =
    {
        "Range::_2mV",
        "Range::_5mV",
        "Range::_10mV",
        "Range_20mV",
        "Range_50mV",
        "Range_100mV",
        "Range_200mV",
        "Range::_500mV",
        "Range_1V",
        "Range_2V",
        "Range_5V",
        "Range_10V",
        "Range_20V"
    };
    return names[range];
}


int Tables_GetNumChannel(Chan::E ch)
{
    return (ch == Chan::A) ? 1 : 2;
}


void DrawStr(int index, int x, int y)
{
    pchar str = symbolsAlphaBet[index];
    if (index == INDEX_SYMBOL)
    {
        Painter::FillRegion(x - 1, y, Font::GetLengthText(str), 9, Color::FLASH_10);
    }
    PText::DrawText(x, y, symbolsAlphaBet[index], index == INDEX_SYMBOL ? Color::FLASH_01 : COLOR_FILL);
}
