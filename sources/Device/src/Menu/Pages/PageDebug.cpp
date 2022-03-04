// 2022/2/11 19:47:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Definition.h"
#include "FDrive/FDrive.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGA.h"
#include "Hardware/Sound.h"
#include "Menu/Menu.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include <stdio.h>


extern const Page  pDebug;
extern const Page  mpConsole;
extern const Page  mpADC;                              // ������� - ���
extern const Page  mpADC_Balance;                      // ������� - ��� - ������
extern const Page  mpADC_Stretch;                      // ������� - ��� - ��������
extern const Page  mpADC_Shift;
extern const Page  mpADC_AltRShift;                    // ������� - ��� - ��� ����
extern const Page  mpRandomizer;                       // ������� - ����-���
extern const Page  ppSerialNumber;                     // ������� - �/�


// � ���� ��������� ����� ��������� ������ ��������� ������ ��� �������� �������� ppSerialNumer
struct StructForSN
{
    int number;     // ��������������, ���������� �����.
    int year;       // ��������������, ���.
    int curDigt;    // ��������������, ������� (0) ��� ����� (1) ��������� ����� ���������.
};


extern Page mainPage;

Page *PageDebug::SerialNumber::GetPointer()
{
    return (Page *)&ppSerialNumber;
}


static const Choice mcStats =
{
    TypeItem::Choice, &pDebug, 0,
    {
        "����������", "Statistics",
        "����������/�� ���������� �����/����, ������ � �������, ���������� �������� � ���������� ����������� � ������/���������� ����������� � ������ ��������",
        "To show/not to show a time/shot, frames per second, quantity of signals with the last settings in memory/quantity of the signals kept in memory"
    },
    {
        "�� ����������",    "Hide",
        "����������",       "Show"
    },
    (int8 *)&SHOW_STATS
};


static const Governor mgConsole_NumStrings
(
    &mpConsole, 0,
    "����� �����", "Number strings",
    "",
    "",
    &NUM_STRINGS, 0, 33
);


static const Choice mcConsole_SizeFont =
{
    TypeItem::Choice, &mpConsole, 0,
    {
        "������ ������", "Size font",
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
    (void *)&mgConsole_NumStrings,   // ������� - ������� - ����� �����
    (void *)&mcConsole_SizeFont      // ������� - ������� - ������ ������
};


static const Page mpConsole
(
    &pDebug, 0,
    "�������", "CONSOLE",
    "",
    "",
    NamePage::DebugConsole, &itemsConsole
);


static void OnDraw_SizeSettings(int x, int y)
{
    PText::DrawFormatText(x + 5, y + 21, Color::BLACK, "������ %d", sizeof(Settings));
}


static const Choice mcSizeSettings =
{
    TypeItem::Choice, &pDebug, 0,
    {
        "������ ��������", "Size settings",
        "����� ������� ��������� Settings",
        "Show size of struct Settings"
    },
    {
        {"������", "Size"},
        {"������", "Size"}
    },
    0, 0, OnDraw_SizeSettings
};


static bool IsActive_SaveFirmware()
{
    return FLASH_DRIVE_IS_CONNECTED == 1;
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

    Display::ShowWarningGood(Warning::FirmwareSaved);
}


static const Button mbSaveFirmware
(
    &pDebug, IsActive_SaveFirmware,
    "����. ��������", "Save firmware",
    "���������� �������� - �������� 5, 6, 7 ����� ������� 3 � 128 ��, ��� �������� ���������",
    "Saving firmware - sectors 5, 6, 7 with a total size of 3 x 128 kB, where the program is stored",
    OnPress_SaveFirmware
);


static void OnPress_EraseData()
{
    HAL_ROM::EraseData();
}


static const Button bEraseData
(
    &pDebug, EmptyFuncBV,
    "������� ������", "Erase data",
    "������� ������� � �������",
    "Erases data sectors",
    OnPress_EraseData
);


static const arrayItems itemsDebug =
{
    (void *)&mcStats,            // ������� - ����������
    (void *)&mpConsole,          // ������� - �������
    (void *)&mpADC,              // ������� - ���
    (void *)&mpRandomizer,       // ������� - ����-���
    (void *)&mcSizeSettings,     // ������� - ������ ��������
    (void *)&mbSaveFirmware,     // ������� - ����. ��������
    (void *)&bEraseData,         // ������� - ������� ������
    (void *)&ppSerialNumber      // ������� - �/�
};

const Page pDebug
(
    &mainPage, 0,
    "�������", "DEBUG",
    "",
    "",
    NamePage::Debug, &itemsDebug
);


static void OnPress_ResetShift()
{
    SET_SHIFT_ADC_A = 0;
    SET_SHIFT_ADC_B = 0;
}


static const Button bResetShift
(
    &mpADC_Stretch, nullptr,
    "�����", "Reset",
    "����� ������������ �������� � 1", "����� ������������ �������� � 1",
    OnPress_ResetShift
);


static void OnDraw_ShiftADCA(int x, int y)
{
    PText::DrawFormatText(x + 5, y + 21, Color::BLACK, "%d", SET_SHIFT_ADC_A);
}


static const Choice mcShiftADCA =
{
    TypeItem::Choice, &pDebug, nullptr,
    {
        "�������� �1", "Shift ch1",
        "�������� ������� ������",
        "�������� ������� ������"
    },
    {
        {"������", "Size"},
        {"������", "Size"}
    },
    0, 0, OnDraw_ShiftADCA
};


static void OnDraw_ShiftADCB(int x, int y)
{
    PText::DrawFormatText(x + 5, y + 21, Color::BLACK, "%d", SET_SHIFT_ADC_B);
}


static const Choice mcShiftADCB =
{
    TypeItem::Choice, &pDebug, nullptr,
    {
        "�������� �1", "Shift ch1",
        "�������� ������� ������",
        "�������� ������� ������"
    },
    {
        {"������", "Size"},
        {"������", "Size"}
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
    "��������", "SHIFT",
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
        "����������", "Compress",
        "��� ��������� ����������� ������ �����",
        "Read more points when turned on",
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&SET_FPGA_COMPACT, OnChanged_Compact
};


static const arrayItems itemsADC =
{
    (void *)&mpADC_Balance,
    (void *)&mpADC_Stretch,
    (void *)&mpADC_Shift,
    (void *)&mpADC_AltRShift,
    (void *)&mcADC_Compact
};


static const Page mpADC
(
    &pDebug, 0,
    "���", "ADC",
    "",
    "",
    NamePage::DebugADC, &itemsADC
);


static const Governor mgADC_Balance_ShiftA
(
    &mpADC_Balance, nullptr,
    "�������� 1", "Offset 1",
    "",
    "",
    &SET_BALANCE_ADC_A, -125, 125, nullptr
);


static const Governor mgADC_Balance_ShiftB
(
    &mpADC_Balance, nullptr,
    "�������� 2", "Offset 2",
    "",
    "",
    &SET_BALANCE_ADC_B, -125, 125, nullptr
);


static const arrayItems itemsADC_Balance =
{
    (void*)&mgADC_Balance_ShiftA,   // ������� - ��� - ������ - �������� 1
    (void*)&mgADC_Balance_ShiftB    // ������� - ��� - ������ - �������� 2
};

static const Page mpADC_Balance
(
    &mpADC, 0,
    "������", "BALANCE",
    "",
    "",
    NamePage::DebugADCbalance, &itemsADC_Balance
);


static void OnPress_ResetStretch()
{
    SET_STRETCH_ADC_A = 1.0f;
    SET_STRETCH_ADC_B = 1.0f;
}


static const Button bResetStretch
(
    &mpADC_Stretch, nullptr,
    "�����", "Reset",
    "����� ������������ �������� � 1", "����� ������������ �������� � 1",
    OnPress_ResetStretch
);


static void OnDraw_StretchADCA(int x, int y)
{
    PText::DrawFormatText(x + 5, y + 21, Color::BLACK, "%f", SET_STRETCH_ADC_A);
}


static const Choice mcStretchADCA =
{
    TypeItem::Choice, &pDebug, nullptr,
    {
        "�������� �1", "Stretch ch1",
        "�������� ������� ������",
        "�������� ������� ������"
    },
    {
        {"������", "Size"},
        {"������", "Size"}
    },
    0, 0, OnDraw_StretchADCA
};


static void OnDraw_StretchADCB(int x, int y)
{
    PText::DrawFormatText(x + 5, y + 21, Color::BLACK, "%f", SET_STRETCH_ADC_B);
}


static const Choice mcStretchADCB =
{
    TypeItem::Choice, &pDebug, nullptr,
    {
        "�������� �2", "Stretch ch2",
        "�������� ������� ������",
        "�������� ������� ������"
    },
    {
        {"������", "Size"},
        {"������", "Size"}
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
    "��������", "STRETCH",
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
                RSHIFT_ADD(ch, range, mode) = 0;
            }
        }
    }

    RShift::Set(Chan::A, SET_RSHIFT_A);
    RShift::Set(Chan::B, SET_RSHIFT_B);
}


static const Button mbADC_AltRShift_Reset
(
    &mpADC_AltRShift, 0,
    "�����", "Reset",
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
    "�� 1� 2�� ����", "Shift 1ch 2mV DC",
    "",
    "",
    &RSHIFT_ADD(Chan::A, Range::_2mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_A
);


static void OnChanged_ADC_AltRShift_B()
{
    RShift::Set(Chan::B, SET_RSHIFT_B);
}


static const Governor mbADC_AltRShift_2mV_DC_B
(
    &mpADC_AltRShift, 0,
    "�� 2� 2�� ����", "Shift 2ch 2mV DC",
    "",
    "",
    &RSHIFT_ADD(Chan::B, Range::_2mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_B
);


static const Governor mbADC_AltRShift_5mV_DC_A
(
    &mpADC_AltRShift, 0,
    "�� 1� 5�� ����", "Shift 1ch 5mV DC",
    "",
    "",
    &RSHIFT_ADD(Chan::A, Range::_5mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_A
);


static const Governor mbADC_AltRShift_5mV_DC_B
(
    &mpADC_AltRShift, 0,
    "�� 2� 5�� ����", "Shift 2ch 5mV DC",
    "",
    "",
    &RSHIFT_ADD(Chan::B, Range::_5mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_B
);


static const Governor mbADC_AltRShift_10mV_DC_A
(
    &mpADC_AltRShift, 0,
    "�� 1� 10�� ����", "Shift 1ch 10mV DC",
    "",
    "",
    &RSHIFT_ADD(Chan::A, Range::_10mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_A
);


static const Governor mbADC_AltRShift_10mV_DC_B
(
    &mpADC_AltRShift, 0,
    "�� 2� 10�� ����", "Shift 2ch 10mV DC",
    "",
    "",
    &RSHIFT_ADD(Chan::B, Range::_10mV, ModeCouple::DC), -100, 100, OnChanged_ADC_AltRShift_B
);


static const arrayItems itemsADC_AltRShift =
{
    (void*)&mbADC_AltRShift_Reset,          // ������� - ��� - ��� ���� - �����
    (void*)&mbADC_AltRShift_2mV_DC_A,       // ������� - ��� - ��� ���� - �� 1� 2�� ����
    (void*)&mbADC_AltRShift_2mV_DC_B,       // ������� - ��� - ��� ���� - �� 2� 2�� ����
    (void*)&mbADC_AltRShift_5mV_DC_A,       // ������� - ��� - ��� ���� - �� 1� 5�� ����
    (void*)&mbADC_AltRShift_5mV_DC_B,       // ������� - ��� - ��� ���� - �� 2� 5�� ����
    (void*)&mbADC_AltRShift_10mV_DC_A,      // ������� - ��� - ��� ���� - �� 1� 10�� ����
    (void*)&mbADC_AltRShift_10mV_DC_B       // ������� - ��� - ��� ���� - �� 2� 10�� ����    
};


static const Page mpADC_AltRShift
(
    &mpADC, 0,
    "��� ����", "ADD RSHFIT",
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
    "���-�/������", "Samples/gates",
    "",
    "",
    &NUM_MEAS_FOR_GATES, 1, 2500, OnChanged_Randomizer_SamplesForGates
);


static const Governor mgRandomizer_Average
(
    &mpRandomizer, 0,
    "������.", "Average",
    "",
    "",
    &NUM_AVE_FOR_RAND, 1, 32
);


static const arrayItems itemsRandomizer =
{
    (void*)&mgRandomizer_SamplesForGates,   // ������� - ����-��� - ���-�/������
    (void*)&mgRandomizer_Average            // ������� - ����-��� - ������.
};


static const Page mpRandomizer
(
    &pDebug, 0,
    "����-���", "RANDOMIZER",
    "",
    "",
    NamePage::DebugRandomizer, &itemsRandomizer
);


static void OnPress_SerialNumber_Exit()
{
    Display::RemoveAddDrawFunction();
    FREE_EXTRAMEM();
}


static const SmallButton bSerialNumber_Exit
(
    &ppSerialNumber,
    COMMON_BEGIN_SB_EXIT,
    OnPress_SerialNumber_Exit,
    DrawSB_Exit
);


static void OnPress_SerialNumber_Change()
{
    ACCESS_EXTRAMEM(StructForSN, s);
    ++s->curDigt;
    s->curDigt %= 2;
    Color::ResetFlash();
}


static void Draw_SerialNumber_Change(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_TAB);
    Font::Set(TypeFont::_8);
}


static const SmallButton bSerialNumber_Change
(
    &ppSerialNumber, 0,
    "��������", "Insert",
    "��������� �������� ������",
    "Inserts the chosen symbol",
    OnPress_SerialNumber_Change,
    Draw_SerialNumber_Change
);


static void OnPress_SerialNumber_Save()
{
    ACCESS_EXTRAMEM(StructForSN, s);

    char stringSN[20];

    snprintf(stringSN, 19, "%02d %04d", s->number, s->year);

    if (!OTP::SaveSerialNumber(stringSN))
    {
        Display::ShowWarningBad(Warning::FullyCompletedOTP);
    }
}


static void Draw_SerialNumber_Save(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Font::Set(TypeFont::_8);
}


static const SmallButton bSerialNumber_Save
(
    &ppSerialNumber, 0,
    "���������", "Save",
    "���������� �������� ����� � OTP",
    "Records the serial number in OTP",
    OnPress_SerialNumber_Save,
    Draw_SerialNumber_Save
);


static const arrayItems itemsSerialNumber =
{
    (void*)&bSerialNumber_Exit,     // ������� - �/� - �����
    (void*)&bSerialNumber_Change,   // ������� - �/� - �������
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)&bSerialNumber_Save      // ������� - �/� - ���������    
};


static void Draw_EnterSerialNumber()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Painter::DrawRectangle(x0, y0, width, height, COLOR_FILL);
    Painter::FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, COLOR_BACK);

    int deltaX = 10;

    ACCESS_EXTRAMEM(StructForSN, s);

    bool selNumber = s->curDigt == 0;

    char buffer[20];
    snprintf(buffer, 19, "%02d", s->number);

    Color::E colorText = COLOR_FILL;
    Color::E colorBackground = COLOR_BACK;

    if (selNumber)
    {
        colorText = Color::FLASH_01;
        colorBackground = Color::FLASH_10;
    }

    int y = y0 + 50;

    Color::SetCurrent(colorText);
    int x = PText::DrawTextOnBackground(x0 + deltaX, y, buffer, colorBackground);

    colorText = Color::FLASH_01;
    colorBackground = Color::FLASH_10;

    if (selNumber)
    {
        colorText = COLOR_FILL;
        colorBackground = COLOR_BACK;
    }

    snprintf(buffer, 19, "%04d", s->year);

    Color::SetCurrent(colorText);
    PText::DrawTextOnBackground(x + 5, y, buffer, colorBackground);

    // ������ ������� ���������� �� ���������� ����� � OTP-������ ��� ������

    int allShots = OTP::GetSerialNumber(buffer);

    PText::DrawFormatText(x0 + deltaX, y0 + 130, COLOR_FILL, "������� ���������� ����� %s", buffer[0] == 0 ? "-- ----" : buffer);

    PText::DrawFormatText(x0 + deltaX, y0 + 100, COLOR_FILL, "�������� ����� ��� %d �������", allShots);
}


static void OnPress_SerialNumber()
{
    PageDebug::SerialNumber::GetPointer()->OpenAndSetCurrent();
    Display::SetAddDrawFunction(Draw_EnterSerialNumber);
    MALLOC_EXTRAMEM(StructForSN, s);
    s->number = 01;
    s->year = 2017;
    s->curDigt = 0;
}


static void OnRegSet_SerialNumber(int angle)
{
    typedef int(*pFunc)(int *, int, int);

    pFunc p = angle > 0 ? CircleIncreaseInt : CircleDecreaseInt;

    ACCESS_EXTRAMEM(StructForSN, s);

    if (s->curDigt == 0)
    {
        p(&s->number, 1, 99);
    }
    else
    {
        p(&s->year, 2014, 2050);
    }
    Sound::GovernorChangedValue();
}


static const Page ppSerialNumber
(
    &pDebug, 0,
    "�/�", "S/N",
    "������ ��������� ������ � OTP-������. ��������!!! ��P-������ - ������ � ����������� �������.",
    "Serial number recording in OTP-memory. ATTENTION!!! OTP memory is a one-time programming memory.",
    NamePage::SB_SerialNumber, &itemsSerialNumber, OnPress_SerialNumber, 0, OnRegSet_SerialNumber
);
