// 2022/03/23 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Screen/LowPart.h"
#include "Display/DisplayTypes.h"
#include "Display/Painter.h"
#include "Display/Screen/Grid.h"
#include "Data/Measures.h"
#include "FPGA/SettingsFPGA.h"
#include "Settings/Settings.h"
#include "Data/Data.h"
#include "FPGA/FPGA.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/FDrive/FDrive.h"
#include "Display/Symbols.h"
#include "Hardware/LAN/LAN.h"
#include "Hardware/Timer.h"
#include "Hardware/VCP/VCP.h"
#include "Data/Processing.h"
#include "Display/String.h"
#include <cstdio>


namespace LowPart
{
    void WriteTextVoltage(Chan, int x, int y);

    void WriteStringAndNumber(pchar text, int x, int y, int number);

    // Вывести текущее время.
    void DrawTime(int x, int y);
}


void LowPart::Draw()
{
    int y0 = SCREEN_HEIGHT - 19;
    int y1 = SCREEN_HEIGHT - 10;
    int x = -1;

    Painter::DrawHLine(Grid::ChannelBottom(), 1, Grid::Left() - Measures::GetDeltaGridLeft() - 2, COLOR_FILL);
    Painter::DrawHLine(Grid::FullBottom(), 1, Grid::Left() - Measures::GetDeltaGridLeft() - 2);

    WriteTextVoltage(Chan::A, x + 2, y0);

    WriteTextVoltage(Chan::B, x + 2, y1);

    Painter::DrawVLine(x + 95, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2, COLOR_FILL);

    x += 98;
    char buffer[100] = {0};

    TBase::E tBase = SET_TBASE;
    int tShift = SET_TSHIFT;

    if (!MODE_WORK_IS_DIRECT)
    {
        const DataSettings &ds = MODE_WORK_IS_LATEST ? *Data::last.ds : *Data::ins.ds;

        if (ds.Valid())
        {
            tBase = ds.tBase;
            tShift = ds.tShift;
        }
    }

    std::sprintf(buffer, "р\xa5%s", TBase::ToString(tBase));
    String(buffer).Draw(x, y0);

    buffer[0] = 0;
    std::sprintf(buffer, "\xa5%s", TShift::ToString(tShift).c_str());
    String(buffer).Draw(x + 35, y0);

    buffer[0] = 0;

    if (MODE_WORK_IS_DIRECT)
    {
        pchar source[3] = {"1", "2", "\x82"};
        std::sprintf(buffer, "с\xa5\x10%s", source[TRIG_SOURCE]);
    }

    String(buffer).Draw(x, y1, ColorTrig());

    buffer[0] = 0;

    static pchar couple[] =
    {
        "\x92",
        "\x91",
        "\x92",
        "\x92"
    };

    static pchar polar[] =
    {
        "\xa7",
        "\xa6"
    };

    static pchar filtr[] =
    {
        "\xb5\xb6",
        "\xb5\xb6",
        "\xb3\xb4",
        "\xb1\xb2"
    };

    if (MODE_WORK_IS_DIRECT)
    {
        std::sprintf(buffer, "\xa5\x10%s\x10\xa5\x10%s\x10\xa5\x10", couple[TRIG_INPUT], polar[TRIG_POLARITY]);
        String(buffer).Draw(x + 18, y1);
        PText::DrawChar(x + 45, y1, filtr[TRIG_INPUT][0]);
        PText::DrawChar(x + 53, y1, filtr[TRIG_INPUT][1]);
    }

    buffer[0] = '\0';
    const char mode[] =
    {
        '\xb7',
        '\xa0',
        '\xb0'
    };

    if (MODE_WORK_IS_DIRECT)
    {
        std::sprintf(buffer, "\xa5\x10%c", mode[START_MODE]);
        String(buffer).Draw(x + 63, y1);
    }

    Painter::DrawVLine(x + 79, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2, COLOR_FILL);

    Painter::DrawHLine(GRID_BOTTOM, GRID_RIGHT + 2, SCREEN_WIDTH - 2);
    Painter::DrawHLine(Grid::ChannelBottom(), GRID_RIGHT + 2, SCREEN_WIDTH - 2);

    x += 82;
    y0 = y0 - 3;
    y1 = y1 - 6;
    int y2 = y1 + 6;
    Font::Set(TypeFont::_5);

    if (MODE_WORK_IS_DIRECT)
    {
        int dy = -6;
        WriteStringAndNumber("накопл", x, y0 + dy, NUM_ACCUM);
        WriteStringAndNumber("усредн", x, y1 + dy, NUM_AVE);
        WriteStringAndNumber("мн\x93мкс", x, y2 + dy, NUM_MIN_MAX);
    }

    x += 42;
    Painter::DrawVLine(x, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    Font::Set(TypeFont::_8);

    if (MODE_WORK_IS_DIRECT)
    {
        char mesFreq[20] = "\x7c=";
        float freq = FPGA::FreqMeter::GetFreq();
        if (freq == -1.0f) //-V550
        {
            std::strcat(mesFreq, "******");
        }
        else
        {
            std::strcat(mesFreq, Freq2String(freq, false).c_str());
        }
        String(mesFreq).Draw(x + 3, GRID_BOTTOM + 2);
    }

    DrawTime(x + 3, GRID_BOTTOM + 11);

    Painter::DrawVLine(x + 55, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    Font::Set(TypeFont::UGO2);

    // Флешка
    if (FDrive::isConnected)
    {
        PText::Draw4SymbolsInRect(x + 57, GRID_BOTTOM + 2, SYMBOL_FLASH_DRIVE);
    }

    // Ethernet
    if ((LAN::clientIsConnected || LAN::cableIsConnected) && TIME_MS > 2000)
    {
        PText::Draw4SymbolsInRect(x + 87, GRID_BOTTOM + 2, SYMBOL_ETHERNET, LAN::clientIsConnected ? COLOR_FILL : Color::FLASH_01);
    }

    if (VCP::connectToHost || VCP::cableIsConnected)
    {
        PText::Draw4SymbolsInRect(x + 72, GRID_BOTTOM + 2, SYMBOL_USB, VCP::connectToHost ? COLOR_FILL : Color::FLASH_01);
    }

    Color::SetCurrent(COLOR_FILL);

    if (!SET_PEAKDET_IS_DISABLED)
    {
        PText::DrawChar(x + 38, GRID_BOTTOM + 11, '\x12');
        PText::DrawChar(x + 46, GRID_BOTTOM + 11, '\x13');
    }

    if (MODE_WORK_IS_DIRECT)
    {
        Font::Set(TypeFont::_5);
        WriteStringAndNumber("СГЛАЖ.:", x + 57, GRID_BOTTOM + 5, Smoothing::ToPoints());
        Font::Set(TypeFont::_8);
    }
}


void LowPart::DrawTime(int x, int y)
{
    int dField = 10;
    int dSeparator = 2;

    PackedTime time = HAL_RTC::GetPackedTime();

    char buffer[20];

    Color::SetCurrent(COLOR_FILL);

    if (MODE_WORK_IS_MEMINT || MODE_WORK_IS_LATEST)
    {
        const DataSettings &ds = MODE_WORK_IS_MEMINT ? *Data::ins.ds : *Data::last.ds;

        if (ds.Valid())
        {
            y -= 9;
            time.day = ds.time.day;
            time.hours = ds.time.hours;
            time.minutes = ds.time.minutes;
            time.seconds = ds.time.seconds;
            time.month = ds.time.month;
            time.year = ds.time.year;
            String(Int2String((int)time.day, false, 2, buffer)).Draw(x, y);
            String(":").Draw(x + dField, y);
            String(Int2String((int)time.month, false, 2, buffer)).Draw(x + dField + dSeparator, y);
            String(":").Draw(x + 2 * dField + dSeparator, y);
            String(Int2String((int)time.year + 2000, false, 4, buffer)).Draw(x + 2 * dField + 2 * dSeparator, y);
            y += 9;
        }
        else
        {
            return;
        }
    }

    String(Int2String((int)time.hours, false, 2, buffer)).Draw(x, y);
    String(":").Draw(x + dField, y);
    String(Int2String((int)time.minutes, false, 2, buffer)).Draw(x + dField + dSeparator, y);
    String(":").Draw(x + 2 * dField + dSeparator, y);
    String(Int2String((int)time.seconds, false, 2, buffer)).Draw(x + 2 * dField + 2 * dSeparator, y);
}


void LowPart::WriteTextVoltage(Chan ch, int x, int y)
{
    static pchar couple[] =
    {
        "\x92",
        "\x91",
        "\x90"
    };
    Color::E color = ColorChannel(ch);

    bool inverse = SET_INVERSE(ch);
    ModeCouple::E modeCouple = SET_COUPLE(ch);
    Divider::E multiplier = SET_DIVIDER(ch);
    Range::E range = SET_RANGE(ch);
    RShift rShift = SET_RSHIFT(ch);
    bool enable = SET_ENABLED(ch);

    if (!MODE_WORK_IS_DIRECT)
    {
        const DataSettings &ds = MODE_WORK_IS_DIRECT ? Processing::out.ds : *Data::ins.ds;

        if (ds.Valid())
        {
            inverse = ch.IsA() ? ds.inv_a : ds.inv_b;
            modeCouple = ch.IsA() ? ds.coupleA : ds.coupleB;
            multiplier = ch.IsA() ? ds.div_a : ds.div_b;
            range = ds.range[ch];
            rShift.value = (int16)(ch.IsA() ? ds.rShiftA : ds.rShiftB);
        }
    }

    if (enable)
    {
        const int widthField = 91;
        const int heightField = 8;

        Color::E colorDraw = inverse ? Color::WHITE : color;
        if (inverse)
        {
            Painter::FillRegion(x, y, widthField, heightField, color);
        }

        char buffer[100] = {0};

        std::sprintf(buffer, "%s\xa5%s\xa5%s", (ch == Chan::A) ? (LANG_RU ? "1к" : "1c") : (LANG_RU ? "2к" : "2c"),
            couple[modeCouple], Range::ToString(range, multiplier));

        String(buffer).Draw(x + 1, y, colorDraw);

        std::sprintf(buffer, "\xa5%s", rShift.ToString(range, multiplier).c_str());
        String(buffer).Draw(x + 46, y);
    }
}


void LowPart::WriteStringAndNumber(pchar text, int x, int y, int number)
{
    char buffer[100];
    String(text).Draw(x, y, COLOR_FILL);

    if (number == 0)
    {
        std::sprintf(buffer, "-");
    }
    else
    {
        std::sprintf(buffer, "%d", number);
    }

    PText::DrawRelativelyRight(x + 41, y, buffer);
}
