// 2022/2/11 19:47:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Definition.h"
#include "Hardware/FDrive/FDrive.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Display/Screen/Grid.h"
#include "FPGA/FPGA.h"
#include "Hardware/Sound.h"
#include "Menu/Menu.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Math.h"
#include "Utils/Text/Warnings.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Warnings.h"
#include "Utils/Text/Symbols.h"
#include <cstdio>
#include <cstring>


extern const Page  mpADC;
extern const Page  mpADC_Stretch;
extern const Page  mpADC_Shift;
extern const Page  mpADC_AltRShift;
extern const Page  mpRandomizer;


// В этой структуре будут храниться данные серийного номера при открытой странице ppSerialNumer
static struct StructForSN
{
    int number;     // Соответственно, порядковый номер.
    int year;       // Соответственно, год.
    int curDigt;    // Соответственно, номером (0) или годом (1) управляет ручка УСТАНОВКА.
} structSN;


static const Choice mcStats =
{
    TypeItem::Choice, PageDebug::self, 0,
    {
        "Статистика", "Statistics",
        "Показывать/не показывать время/кадр, кадров в секунду, количество сигналов с последними настройками в памяти/количество сохраняемых в памяти сигналов",
        "To show/not to show a time/shot, frames per second, quantity of signals with the last settings in memory/quantity of the signals kept in memory"
    },
    {
        "Не показывать",    "Hide",
        "Показывать",       "Show"
    },
    (int8 *)&SHOW_STATS
};


static const Governor mgConsole_NumStrings
(
    PageDebug::Console::self, 0,
    "Число строк", "Number strings",
    "",
    "",
    &NUM_STRINGS, 0, 33
);


static const Choice mcConsole_SizeFont =
{
    TypeItem::Choice, PageDebug::Console::self, 0,
    {
        "Размер шрифта", "Size font",
        "",
        ""
    },
    {
        "5",    "5",
        "8",    "8"
    },
    (int8 *)&SIZE_FONT_CONSOLE
};


static const arrayItems itemsConsole =
{
    (void *)&mgConsole_NumStrings,   // ОТЛАДКА - КОНСОЛЬ - Число строк
    (void *)&mcConsole_SizeFont      // ОТЛАДКА - КОНСОЛЬ - Размер шрифта
};


static const Page mpConsole
(
    PageDebug::self, 0,
    "КОНСОЛЬ", "CONSOLE",
    "",
    "",
    NamePage::DebugConsole, &itemsConsole
);


static void OnDraw_SizeSettings(int x, int y)
{
    String<>("Размер %d", sizeof(Settings)).Draw(x + 5, y + 21, Color::BLACK);
}


static const Choice mcSizeSettings =
{
    TypeItem::Choice, PageDebug::self, 0,
    {
        "Размер настроек", "Size settings",
        "Вывод размера структуры Settings",
        "Show size of struct Settings"
    },
    {
        {"Размер", "Size"},
        {"Размер", "Size"}
    },
    0, 0, OnDraw_SizeSettings
};


static bool IsActive_SaveFirmware()
{
    return FDrive::isConnected;
}


static void OnPress_SaveFirmware()
{
    StructForWrite structForWrite;

    FDrive::OpenNewFileForWrite("S8-53.bin", &structForWrite);

    const uint ADDR_START_FIRMWARE = 0x08020000;

    uint8 *address = (uint8 *)ADDR_START_FIRMWARE; //-V566
    uint8 *endAddress = address + 128 * 1024 * 3;

    int sizeBlock = 512;

    while (address < endAddress)
    {
        FDrive::WriteToFile(address, sizeBlock, &structForWrite);
        address += sizeBlock;
    }

    FDrive::CloseFile(&structForWrite);

    Warning::ShowGood(Warning::FirmwareSaved);
}


static const Button mbSaveFirmware
(
    PageDebug::self, IsActive_SaveFirmware,
    "Сохр. прошивку", "Save firmware",
    "Сохранение прошивки - секторов 5, 6, 7 общим объёмом 3 х 128 кБ, где хранится программа",
    "Saving firmware - sectors 5, 6, 7 with a total size of 3 x 128 kB, where the program is stored",
    OnPress_SaveFirmware
);


static void OnPress_EraseData()
{
    HAL_ROM::Data::EraseAll();
}


static const Button bEraseData
(
    PageDebug::self, EmptyFuncBV,
    "Стереть данные", "Erase data",
    "Стирает сектора с данными",
    "Erases data sectors",
    OnPress_EraseData
);


void PageDebug::ADC::ResetCalRShift(Chan ch)
{
    std::memset(&set.chan[ch].cal_rshift[0][0], 0, sizeof(int8) * Range::Count * ModeCouple::Count);
}


static void OnPress_RestRShift()
{
    PageDebug::ADC::ResetCalRShift(ChA);
    PageDebug::ADC::ResetCalRShift(ChB);
}


static const Button bResetShift
(
    &mpADC_Stretch, nullptr,
    "Сброс", "Reset",
    "Сброс коэффициента растяжки в 1", "Сброс коэффициента растяжки в 1",
    OnPress_RestRShift
);


static void OnDraw_ShiftADCA(int x, int y)
{
    String<>("%d", set.chan[ChA].cal_rshift[Range::_2mV][ModeCouple::DC]).Draw(x + 5, y + 21, Color::BLACK);
}


static const Choice mcShiftADCA =
{
    TypeItem::Choice, &mpADC_Shift, nullptr,
    {
        "Смещение к1", "Shift ch1",
        "Смещение первого канала",
        "Смещение первого канала"
    },
    {
        {"Размер", "Size"},
        {"Размер", "Size"}
    },
    0, 0, OnDraw_ShiftADCA
};


static void OnDraw_ShiftADCB(int x, int y)
{
    String<>("%d", set.chan[ChB].cal_rshift[Range::_2mV][ModeCouple::DC]).Draw(x + 5, y + 21, Color::BLACK);
}


static const Choice mcShiftADCB =
{
    TypeItem::Choice, &mpADC_Shift, nullptr,
    {
        "Смещение к1", "Shift ch1",
        "Смещение первого канала",
        "Смещение первого канала"
    },
    {
        {"Размер", "Size"},
        {"Размер", "Size"}
    },
    0, 0, OnDraw_ShiftADCB
};


static const arrayItems itemsADC_Shift =
{
    (void *)&bResetShift,
    (void *)&mcShiftADCA,
    (void *)&mcShiftADCB
};


static const Page mpADC_Shift
(
    &mpADC, 0,
    "СМЕЩЕНИЕ", "SHIFT",
    "",
    "",
    NamePage::DebugADCshift, &itemsADC_Shift
);


void OnChanged_Compact(bool)
{
    TBase::Set(SET_TBASE);
}


static const Choice mcADC_Compact =
{
    TypeItem::Choice, &mpADC, nullptr,
    {
        "Уплотнение", "Compress",
        "При включении считывается больше точек",
        "Read more points when turned on",
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&SET_FPGA_COMPACT, OnChanged_Compact
};


static const Governor mgADC_FirstByte
(
    &mpADC, nullptr,
    "Первый байт", "First byte",
    "", "",
    &set.debug.first_byte, -5, 5, nullptr, nullptr
);


static const arrayItems itemsADC =
{
    (void *)PageDebug::ADC::Balance::self,
    (void *)&mpADC_Stretch,
    (void *)&mpADC_Shift,
    (void *)&mpADC_AltRShift,
    (void *)&mcADC_Compact,
    (void *)&mgADC_FirstByte
};


static const Page mpADC
(
    PageDebug::self, 0,
    "АЦП", "ADC",
    "",
    "",
    NamePage::DebugADC, &itemsADC
);


static const Governor mgADC_Balance_ShiftA
(
    PageDebug::ADC::Balance::self, nullptr,
    "Смещение 1", "Offset 1",
    "",
    "",
    &SET_BALANCE_ADC_A, -125, 125, nullptr
);


static const Governor mgADC_Balance_ShiftB
(
    PageDebug::ADC::Balance::self, nullptr,
    "Смещение 2", "Offset 2",
    "",
    "",
    &SET_BALANCE_ADC_B, -125, 125, nullptr
);


static const arrayItems itemsADC_Balance =
{
    (void*)&mgADC_Balance_ShiftA,   // ОТЛАДКА - АЦП - БАЛАНС - Смещение 1
    (void*)&mgADC_Balance_ShiftB    // ОТЛАДКА - АЦП - БАЛАНС - Смещение 2
};

static const Page mpADC_Balance
(
    &mpADC, 0,
    "БАЛАНС", "BALANCE",
    "",
    "",
    NamePage::DebugADCbalance, &itemsADC_Balance
);


void PageDebug::ADC::ResetCalStretch(Chan ch)
{
    CAL_STRETCH(ch) = 1.0f;
}


static void OnPress_ResetStretch()
{
    PageDebug::ADC::ResetCalStretch(ChA);
    PageDebug::ADC::ResetCalStretch(ChB);
}


static const Button bResetStretch
(
    &mpADC_Stretch, nullptr,
    "Сброс", "Reset",
    "Сброс коэффициента растяжки в 1", "Сброс коэффициента растяжки в 1",
    OnPress_ResetStretch
);


static void OnDraw_StretchADCA(int x, int y)
{
    String<>("%f", CAL_STRETCH_A).Draw(x + 5, y + 21, Color::BLACK);
}


static const Choice mcStretchADCA =
{
    TypeItem::Choice, &mpADC_Stretch, nullptr,
    {
        "Растяжка к1", "Stretch ch1",
        "Растяжка первого канала",
        "Растяжка первого канала"
    },
    {
        {"Размер", "Size"},
        {"Размер", "Size"}
    },
    0, 0, OnDraw_StretchADCA
};


static void OnDraw_StretchADCB(int x, int y)
{
    String<>("%f", CAL_STRETCH_B).Draw(x + 5, y + 21, Color::BLACK);
}


static const Choice mcStretchADCB =
{
    TypeItem::Choice, &mpADC_Stretch, nullptr,
    {
        "Растяжка к2", "Stretch ch2",
        "Растяжка первого канала",
        "Растяжка первого канала"
    },
    {
        {"Размер", "Size"},
        {"Размер", "Size"}
    },
    0, 0, OnDraw_StretchADCB
};


static const arrayItems itemsADC_Stretch =
{
    (void *)&bResetStretch,
    (void *)&mcStretchADCA,
    (void *)&mcStretchADCB
};

static const Page mpADC_Stretch
(
    &mpADC, 0,
    "РАСТЯЖКА", "STRETCH",
    "",
    "",
    NamePage::DebugADCstretch, &itemsADC_Stretch
);


static void OnPress_ADC_AltRShift_Reset()
{
    for (int ch = 0; ch < 2; ch++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < Range::Count; range++)
            {
                RSHIFT_HAND(ch, range, mode) = 0;
            }
        }
    }

    RShift::Set(Chan::A, SET_RSHIFT_A);
    RShift::Set(Chan::B, SET_RSHIFT_B);
}


static const Button mbADC_AltRShift_Reset
(
    &mpADC_AltRShift, 0,
    "Сброс", "Reset",
    "", "",
    OnPress_ADC_AltRShift_Reset
);


static void OnChanged_ADC_AltRShift_A()
{
    RShift::Set(Chan::A, SET_RSHIFT_A);
}


static const Governor mbADC_AltRShift_2mV_DC_A
(
    &mpADC_AltRShift, 0,
    "См 1к 2мВ пост", "Shift 1ch 2mV DC",
    "",
    "",
    &RSHIFT_HAND(Chan::A, Range::_2mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_A
);


static void OnChanged_ADC_AltRShift_B()
{
    RShift::Set(Chan::B, SET_RSHIFT_B);
}


static const Governor mbADC_AltRShift_2mV_DC_B
(
    &mpADC_AltRShift, 0,
    "См 2к 2мВ пост", "Shift 2ch 2mV DC",
    "",
    "",
    &RSHIFT_HAND(Chan::B, Range::_2mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_B
);


static const Governor mbADC_AltRShift_5mV_DC_A
(
    &mpADC_AltRShift, 0,
    "См 1к 5мВ пост", "Shift 1ch 5mV DC",
    "",
    "",
    &RSHIFT_HAND(Chan::A, Range::_5mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_A
);


static const Governor mbADC_AltRShift_5mV_DC_B
(
    &mpADC_AltRShift, 0,
    "См 2к 5мВ пост", "Shift 2ch 5mV DC",
    "",
    "",
    &RSHIFT_HAND(Chan::B, Range::_5mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_B
);


static const Governor mbADC_AltRShift_10mV_DC_A
(
    &mpADC_AltRShift, 0,
    "См 1к 10мВ пост", "Shift 1ch 10mV DC",
    "",
    "",
    &RSHIFT_HAND(Chan::A, Range::_10mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_A
);


static const Governor mbADC_AltRShift_10mV_DC_B
(
    &mpADC_AltRShift, 0,
    "См 2к 10мВ пост", "Shift 2ch 10mV DC",
    "",
    "",
    &RSHIFT_HAND(Chan::B, Range::_10mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_B
);


static const arrayItems itemsADC_AltRShift =
{
    (void*)&mbADC_AltRShift_Reset,          // ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс
    (void*)&mbADC_AltRShift_2mV_DC_A,       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост
    (void*)&mbADC_AltRShift_2mV_DC_B,       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост
    (void*)&mbADC_AltRShift_5mV_DC_A,       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост
    (void*)&mbADC_AltRShift_5mV_DC_B,       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост
    (void*)&mbADC_AltRShift_10mV_DC_A,      // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост
    (void*)&mbADC_AltRShift_10mV_DC_B       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост    
};


static const Page mpADC_AltRShift
(
    &mpADC, 0,
    "ДОП СМЕЩ", "ADD RSHFIT",
    "",
    "",
    NamePage::DebugADCrShift, &itemsADC_AltRShift
);


static void OnChanged_Randomizer_SamplesForGates()
{
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
}


static const Governor mgRandomizer_SamplesForGates
(
    &mpRandomizer, 0,
    "Выб-к/ворота", "Samples/gates",
    "",
    "",
    &NUM_MEAS_FOR_GATES, 1, 2500, OnChanged_Randomizer_SamplesForGates
);


static const Governor mgRandomizer_Average
(
    &mpRandomizer, 0,
    "Усредн.", "Average",
    "",
    "",
    &set.debug.numAveForRand, 1, 32
);


static const Governor mgGatesMin
(
    &mpRandomizer, nullptr,
    "Ворота мин", "Gates min",
    "",
    "",
    &FPGA_GATES_MIN, 0, 1000
);


static const Governor mgGatesMax
(
    &mpRandomizer, nullptr,
    "Ворота макс", "Gates max",
    "",
    "",
    &FPGA_GATES_MAX, 0, 1000
);


static const arrayItems itemsRandomizer =
{
    (void *)&mgRandomizer_SamplesForGates,
    (void *)&mgRandomizer_Average,
    (void *)&mgGatesMin,
    (void *)&mgGatesMax
};


static const Page mpRandomizer
(
    PageDebug::self, 0,
    "РАНД-ТОР", "RANDOMIZER",
    "",
    "",
    NamePage::DebugRandomizer, &itemsRandomizer
);


static void OnPress_SerialNumber_Exit()
{
    Display::RemoveAddDrawFunction();
}


static const SmallButton bSerialNumber_Exit
(
    PageDebug::SerialNumber::self,
    COMMON_BEGIN_SB_EXIT,
    OnPress_SerialNumber_Exit,
    DrawSB_Exit
);


static void OnPress_SerialNumber_Change()
{
    structSN.curDigt++;
    structSN.curDigt %= 2;

    Color::ResetFlash();
}


static void Draw_SerialNumber_Change(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_TAB);
    Font::Set(TypeFont::_8);
}


static const SmallButton bSerialNumber_Change
(
    PageDebug::SerialNumber::self, 0,
    "Вставить", "Insert",
    "Вставляет выбраный символ",
    "Inserts the chosen symbol",
    OnPress_SerialNumber_Change,
    Draw_SerialNumber_Change
);


static void OnPress_SerialNumber_Save()
{
    char stringSN[20];

    std::snprintf(stringSN, 19, "%02d %04d", structSN.number, structSN.year);

    if (!OTP::SaveSerialNumber(stringSN))
    {
        Warning::ShowBad(Warning::FullyCompletedOTP);
    }
}


static void Draw_SerialNumber_Save(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Font::Set(TypeFont::_8);
}


static const SmallButton bSerialNumber_Save
(
    PageDebug::SerialNumber::self, 0,
    "Сохранить", "Save",
    "Записывает серийный номер в OTP",
    "Records the serial number in OTP",
    OnPress_SerialNumber_Save,
    Draw_SerialNumber_Save
);


static const arrayItems itemsSerialNumber =
{
    (void*)&bSerialNumber_Exit,     // ОТЛАДКА - С/Н - Выход
    (void*)&bSerialNumber_Change,   // ОТЛАДКА - С/Н - Перейти
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)&bSerialNumber_Save      // ОТЛАДКА - С/Н - Сохранить    
};


static void Draw_EnterSerialNumber()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Rectangle(width, height).Draw(x0, y0, COLOR_FILL);
    Region(width - 2, height - 2).Fill(x0 + 1, y0 + 1, COLOR_BACK);

    int deltaX = 10;

    bool selNumber = (structSN.curDigt == 0);

    Color::E colorText = COLOR_FILL;
    Color::E colorBackground = COLOR_BACK;

    if (selNumber)
    {
        colorText = Color::FLASH_01;
        colorBackground = Color::FLASH_10;
    }

    int y = y0 + 50;

    Color::SetCurrent(colorText);
    int x = String<>("%02d", structSN.number).DrawOnBackground(x0 + deltaX, y, colorBackground);

    colorText = Color::FLASH_01;
    colorBackground = Color::FLASH_10;

    if (selNumber)
    {
        colorText = COLOR_FILL;
        colorBackground = COLOR_BACK;
    }

    Color::SetCurrent(colorText);
    String<>("%04d", structSN.year).DrawOnBackground(x + 5, y, colorBackground);

    // Теперь выведем информацию об оставшемся месте в OTP-памяти для записи

    char buffer[20];

    int allShots = OTP::GetSerialNumber(buffer);

    String<>("Текущий сохранённый номер %s", buffer[0] == 0 ? "-- ----" : buffer).Draw(x0 + deltaX, y0 + 130, COLOR_FILL);

    String<>("Осталось места для %d попыток", allShots).Draw(x0 + deltaX, y0 + 100, COLOR_FILL);
}


static void OnPress_SerialNumber()
{
    PageDebug::SerialNumber::self->OpenAndSetCurrent();
    Display::SetAddDrawFunction(Draw_EnterSerialNumber);

    structSN.number = 01;
    structSN.year = 2022;
    structSN.curDigt = 0;
}


static void OnRegSet_SerialNumber(int angle)
{
    typedef int(*pFunc)(int *, int, int);

    pFunc p = angle > 0 ? Math::CircleIncrease<int> : Math::CircleDecrease<int>;

    if (structSN.curDigt == 0)
    {
        p(&structSN.number, 1, 99);
    }
    else
    {
        p(&structSN.year, 2014, 2050);
    }
    Sound::GovernorChangedValue();
}


static const Page ppSerialNumber
(
    PageDebug::self, 0,
    "С/Н", "S/N",
    "Запись серийного номера в OTP-память. ВНИМАНИЕ!!! ОТP-память - память с однократной записью.",
    "Serial number recording in OTP-memory. ATTENTION!!! OTP memory is a one-time programming memory.",
    NamePage::SB_SerialNumber, &itemsSerialNumber, OnPress_SerialNumber, 0, OnRegSet_SerialNumber
);


static const arrayItems itemsDebug =
{
    (void *)&mcStats,            // ОТЛАДКА - Статистика
    (void *)&mpConsole,          // ОТЛАДКА - КОНСОЛЬ
    (void *)&mpADC,              // ОТЛАДКА - АЦП
    (void *)&mpRandomizer,       // ОТЛАДКА - РАНД-ТОР
    (void *)&mcSizeSettings,     // ОТЛАДКА - Размер настроек
    (void *)&mbSaveFirmware,     // ОТЛАДКА - Сохр. прошивку
    (void *)&bEraseData,         // ОТЛАДКА - Стереть данные
    (void *)&ppSerialNumber      // ОТЛАДКА - С/Н
};

static const Page pDebug
(
    PageMain::self, 0,
    "ОТЛАДКА", "DEBUG",
    "",
    "",
    NamePage::Debug, &itemsDebug
);


const Page *PageDebug::self = &pDebug;
const Page *PageDebug::Console::self = &mpConsole;
const Page *PageDebug::ADC::Balance::self = &mpADC_Balance;
const Page *PageDebug::SerialNumber::self = &ppSerialNumber;

