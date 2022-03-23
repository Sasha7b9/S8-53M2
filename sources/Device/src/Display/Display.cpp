// 2022/02/11 17:43:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Menu.h"
#include "Display/Screen/Grid.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Display/DataPainter.h"
#include "Display/Screen/HiPart.h"
#include "Display/Screen/Console.h"
#include "FDrive/FDrive.h"
#include "Menu/Pages/Definition.h"
#include "Data/Processing.h"
#include "Utils/Math.h"
#include "Display/Symbols.h"
#include "Hardware/LAN/LAN.h"
#include "Hardware/VCP/VCP.h"
#include "Data/Storage.h"
#include <cstdio>


namespace Display
{
    pFuncVV funcOnHand       = 0;
    pFuncVV funcAdditionDraw = 0;
    pFuncVV funcAfterDraw    = 0;

    bool framesElapsed = false;
    int  numDrawingSignals = 0;

    int topMeasures = GRID_BOTTOM;

    void DrawCursorsWindow();

    // Написать информацию под сеткой - в нижней части дисплея.
    void DrawLowPart();

    // Вывести текущее время.
    void DrawTime(int x, int y);

    // Нарисовать курсоры курсорных измерений.
    void DrawCursors();

    // Вывести значения автоматических измерений.
    void DrawMeasures();

    void WriteTextVoltage(Chan, int x, int y);

    void FuncOnTimerDisableShowLevelRShiftA();

    void FuncOnTimerDisableShowLevelRShiftB();

    void DrawStringNavigation();

    bool NeedForClearScreen();

    void WriteStringAndNumber(pchar text, int x, int y, int number);

    void DrawTimeForFrame(uint timeTicks);

    // Нарисовать горизонтальный курсор курсорных измерений.
    // y - числовое значение курсора.
    // xTearing - координата места, в котором необходимо сделать разрыв для квадрата пересечения.
    void DrawHorizontalCursor(int y, int xTearing);

    // Нарисовать вертикальный курсор курсорных измерений.
    // x - числовое значение курсора.
    // yTearing - координата места, в котором необходимо сделать разрыв для квадрата пересечения.
    void DrawVerticalCursor(int x, int yTearing);

    // Вывести значение уровня синхронизации. 
    void WriteValueTrigLevel();
}


void Display::Init() 
{
    HAL_DAC1::Init();

    Color::ResetFlash();

    HAL_LTDC::Init(HAL_FMC::ADDR_RAM_DISPLAY_FRONT, Display::back_buffer);

    Painter::BeginScene(Color::BLACK);

    Painter::EndScene();

#ifdef DEVICE

//    PageDisplay::PageSettings::LoadBrightness();
    HAL_DAC1::SetBrightness(100);

#else

    HAL_DAC1::SetBrightness(100);

#endif
}



void Display::DrawStringNavigation() 
{
    char buffer[100];
    char *string = Menu::StringNavigation(buffer);

    if(string) 
    {
        int length = Font::GetLengthText(string);
        int height = 10;
        Painter::DrawRectangle(Grid::Left(), GRID_TOP, length + 2, height, COLOR_FILL);
        Painter::FillRegion(Grid::Left() + 1, GRID_TOP + 1, length, height - 2, COLOR_BACK);
        PText::Draw(Grid::Left() + 2, GRID_TOP + 1, string, COLOR_FILL);
    }
}


void Display::RotateRShift(Chan ch)
{
    LAST_AFFECTED_CHANNEL = ch;

    if(TIME_SHOW_LEVELS)
    {
        RShift::showLevel[ch] = true;

        Timer::Enable((ch == Chan::A) ?
            TypeTimer::ShowLevelRShiftA :
            TypeTimer::ShowLevelRShiftB, TIME_SHOW_LEVELS  * 1000,
            (ch == Chan::A) ?
            FuncOnTimerDisableShowLevelRShiftA :
            FuncOnTimerDisableShowLevelRShiftB);
    };

    Flags::needFinishDraw = true;
};


void Display::FuncOnTimerDisableShowLevelRShiftA()
{
    RShift::DisableShowLevel(ChA);
}


void Display::FuncOnTimerDisableShowLevelRShiftB()
{
    RShift::DisableShowLevel(ChB);
}


void Display::RotateTrigLev()
{
    if (TIME_SHOW_LEVELS)
    {
        TrigLev::showLevel = true;
        Timer::Enable(TypeTimer::ShowLevelTrigLev, TIME_SHOW_LEVELS * 1000, TrigLev::DisableShowLevel);
    }

    Flags::needFinishDraw = true;
}


void Display::DrawTime(int x, int y)
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
            PText::Draw(x, y, Int2String((int)time.day, false, 2, buffer));
            PText::Draw(x + dField, y, ":");
            PText::Draw(x + dField + dSeparator, y, Int2String((int)time.month, false, 2, buffer));
            PText::Draw(x + 2 * dField + dSeparator, y, ":");
            PText::Draw(x + 2 * dField + 2 * dSeparator, y, Int2String((int)time.year + 2000, false, 4, buffer));
            y += 9;
        }
        else
        {
            return;
        }
    }
    
    
    PText::Draw(x, y, Int2String((int)time.hours, false, 2, buffer));
    PText::Draw(x + dField, y, ":");
    PText::Draw(x + dField + dSeparator, y, Int2String((int)time.minutes, false, 2, buffer));
    PText::Draw(x + 2 * dField + dSeparator, y, ":");
    PText::Draw(x + 2 * dField + 2 * dSeparator, y, Int2String((int)time.seconds, false, 2, buffer));
}


bool Display::NeedForClearScreen()
{
    int numAccum = NUM_ACCUM;

    if (TBase::InModeRandomizer() || numAccum == 1 || MODE_ACCUM_IS_NORESET || SET_SELFRECORDER)
    {
        return true;
    }

    if (Flags::needFinishDraw)
    {
        Flags::needFinishDraw = false;
        return true;
    }

    if (MODE_ACCUM_IS_RESET && Display::numDrawingSignals >= numAccum)
    {
        Display::numDrawingSignals = 0;
        return true;
    }

    return false;
}


void Display::Update(bool endScene)
{
    TimeMeterUS timeMeter;

    if (funcOnHand != 0)
    {
        funcOnHand();
        return;
    }

    bool needClear = NeedForClearScreen();

    if (needClear)
    {
        Painter::BeginScene(COLOR_BACK);
        DataPainter::MemoryWindow::Draw();
        Grid::Draw();
    }

    DataPainter::DrawData();

    if (needClear)
    {
        DataPainter::DrawMath();
        DataPainter::Spectrum::Draw();
        DrawCursors();
        HiPart::Draw();
        DrawLowPart();
        DrawCursorsWindow();
        TShift::Draw();
        TrigLev::Draw();
        RShift::Draw();
        DrawMeasures();
        DrawStringNavigation();
    }
    
    Menu::Draw();

    if (needClear)
    {
        Warning::DrawWarnings();

        if (funcAdditionDraw)
        {
            funcAdditionDraw();
        }
    }

    Console::Draw();

    if (needClear)    
    {
        WriteValueTrigLevel();
    }

    DrawTimeForFrame(timeMeter.ElapsedTicks());

    Color::SetCurrent(COLOR_FILL);

    Painter::EndScene(endScene);

    if (FDrive::needSave)
    {
        if (Painter::SaveScreenToFlashDrive())
        {
            Warning::ShowGood(Warning::FileIsSaved);
        }
        FDrive::needSave = false;
    }

    if (funcAfterDraw)
    {
        funcAfterDraw();
        funcAfterDraw = 0;
    }
}


void Display::WriteValueTrigLevel()
{
    if (TrigLev::showLevel && MODE_WORK_IS_DIRECT)
    {
        float trigLev = TRIG_LEVEL_SOURCE.ToAbs(SET_RANGE(TRIG_SOURCE));     // WARN Здесь для внешней синхронизации неправильно рассчитывается уровень.

        TrigSource::E trigSource = TRIG_SOURCE;

        if (TRIG_INPUT_IS_AC && trigSource <= TrigSource::ChannelB)
        {
            RShift rShift = SET_RSHIFT((Chan::E)trigSource);
            float rShiftAbs = rShift.ToAbs(SET_RANGE(trigSource));
            trigLev += rShiftAbs;
        }

        char buffer[20];
        std::strcpy(buffer, LANG_RU ? "Ур синхр = " : "Trig lvl = ");
        char bufForVolt[20];
        std::strcat(buffer, Voltage2String(trigLev, true, bufForVolt));
        int width = 96;
        int x = (Grid::Width() - width) / 2 + Grid::Left();
        int y = Grid::BottomMessages() - 20;
        Painter::DrawRectangle(x, y, width, 10, COLOR_FILL);
        Painter::FillRegion(x + 1, y + 1, width - 2, 8, COLOR_BACK);
        PText::Draw(x + 2, y + 1, buffer, COLOR_FILL);
    }
}


void Display::DrawCursorsWindow()
{
    if((!Menu::IsMinimize() || !Menu::IsShown()) && RShift::drawMarkers)
    {
        Painter::DrawScaleLine(2, false);
    }
}


void Display::DrawHorizontalCursor(int y, int xTearing)
{
    y += GRID_TOP;
    if(xTearing == -1)
    {
        Painter::DrawDashedHLine(y, Grid::Left() + 2, Grid::Right() - 1, 1, 1, 0);
    }
    else
    {
        Painter::DrawDashedHLine(y, Grid::Left() + 2, xTearing - 2, 1, 1, 0);
        Painter::DrawDashedHLine(y, xTearing + 2, Grid::Right() - 1, 1, 1, 0);
    }
    Painter::DrawRectangle(Grid::Left() - 1, y - 1, 2, 2);
    Painter::DrawRectangle(Grid::Right() - 1, y - 1, 2, 2);
}


void Display::DrawVerticalCursor(int x, int yTearing)
{
    x += Grid::Left();
    if(yTearing == -1)
    {
        Painter::DrawDashedVLine(x, GRID_TOP + 2, Grid::ChannelBottom() - 1, 1, 1, 0);
    }
    else
    {
        Painter::DrawDashedVLine(x, GRID_TOP + 2, yTearing - 2, 1, 1, 0);
        Painter::DrawDashedVLine(x, yTearing + 2, Grid::ChannelBottom() - 1, 1, 1, 0);
    }
    Painter::DrawRectangle(x - 1, GRID_TOP - 1, 2, 2);
    Painter::DrawRectangle(x - 1, Grid::ChannelBottom() - 1, 2, 2);
}


void Display::DrawCursors()
{
    Chan::E source = CURS_SOURCE;
    Color::SetCurrent(ColorCursors(source));

    if (PageCursors::NecessaryDrawCursors())
    {
        bool bothCursors = !CURS_CNTRL_T_IS_DISABLE(source) && !CURS_CNTRL_U_IS_DISABLE(source);  // Признак того, что включены и вертикальные и 
                                                                            // горизонтальные курсоры - надо нарисовать квадраты в местах пересечения
        int x0 = -1;
        int x1 = -1;
        int y0 = -1;
        int y1 = -1;

        if (bothCursors)
        {
            x0 = (int)(Grid::Left() + CURS_POS_T0(source));
            x1 = (int)(Grid::Left() + CURS_POS_T1(source));
            y0 = (int)(GRID_TOP + PageCursors::GetCursPosU(source, 0));
            y1 = (int)(GRID_TOP + PageCursors::GetCursPosU(source, 1));

            Painter::DrawRectangle(x0 - 2, y0 - 2, 4, 4);
            Painter::DrawRectangle(x1 - 2, y1 - 2, 4, 4);
        }

        CursCntrl::E cntrl = CURS_CNTRL_T(source);

        if (cntrl != CursCntrl::Disable)
        {
            DrawVerticalCursor((int)CURS_POS_T0(source), y0);
            DrawVerticalCursor((int)CURS_POS_T1(source), y1);
        }

        cntrl = CURsU_CNTRL;

        if (cntrl != CursCntrl::Disable)
        {
            DrawHorizontalCursor((int)PageCursors::GetCursPosU(source, 0), x0);
            DrawHorizontalCursor((int)PageCursors::GetCursPosU(source, 1), x1);
        }
    }
}


void Display::DrawMeasures()
{
    if(!SHOW_MEASURES)
    {
        topMeasures = GRID_BOTTOM;
        return;
    }

    Processing::CalculateMeasures();

    if(MEAS_FIELD_IS_HAND)
    {
        int x0 = MEAS_POS_CUR_T0 - SHIFT_IN_MEMORY + Grid::Left();
        int y0 = MEAS_POS_CUR_U0 + GRID_TOP;
        int x1 = MEAS_POS_CUR_T1 - SHIFT_IN_MEMORY + Grid::Left();
        int y1 = MEAS_POS_CUR_U1 + GRID_TOP;
        SortInt(&x0, &x1);
        SortInt(&y0, &y1);
        Painter::DrawRectangle(x0, y0, x1 - x0, y1 - y0, COLOR_FILL);
    }

    int x0 = Grid::Left() - Measures::GetDeltaGridLeft();
    int dX = Measures::GetDX();
    int dY = Measures::GetDY();
    int y0 = Measures::GetTopTable();

    int numRows = Measures::NumRows();
    int numCols = Measures::NumCols();

    for(int str = 0; str < numRows; str++)
    {
        for(int elem = 0; elem < numCols; elem++)
        {
            int x = x0 + dX * elem;
            int y = y0 + str * dY;
            bool active = Measures::IsActive(str, elem) && (Page::NameOpened() == NamePage::SB_MeasTuneMeas);
            Color::E color = active ? COLOR_BACK : COLOR_FILL;
            Measure::E meas = Measures::Type(str, elem);

            if(meas != Measure::None)
            {
                Painter::FillRegion(x, y, dX, dY, COLOR_BACK);
                Painter::DrawRectangle(x, y, dX, dY, COLOR_FILL);
                topMeasures = Math::MinFrom2Int(topMeasures, y);
            }

            if(active)
            {
                Painter::FillRegion(x + 2, y + 2, dX - 4, dY - 4, COLOR_FILL);
            }

            if(meas != Measure::None)
            {
                char buffer[20];
                PText::Draw(x + 4, y + 2, Measures::Name(str, elem), color);

                if(meas == MEAS_MARKED)
                {
                    Painter::FillRegion(x + 1, y + 1, dX - 2, 9, active ? COLOR_BACK : COLOR_FILL);
                    PText::Draw(x + 4, y + 2, Measures::Name(str, elem), active ? COLOR_FILL : COLOR_BACK);
                }

                if(MEAS_SOURCE_IS_A)
                {
                    PText::Draw(x + 2, y + 11, Processing::GetStringMeasure(meas, Chan::A, buffer), ColorChannel(Chan::A));
                }
                else if(MEAS_SOURCE_IS_B)
                {
                    PText::Draw(x + 2, y + 11, Processing::GetStringMeasure(meas, Chan::B, buffer), ColorChannel(Chan::B));
                }
                else
                {
                    PText::Draw(x + 2, y + 11, Processing::GetStringMeasure(meas, Chan::A, buffer), ColorChannel(Chan::A));
                    PText::Draw(x + 2, y + 20, Processing::GetStringMeasure(meas, Chan::B, buffer), ColorChannel(Chan::B));
                }
            }
        }
    }

    if(Page::NameOpened() == NamePage::SB_MeasTuneMeas)
    {
        Measures::DrawPageChoice();
    }
}



void Display::WriteTextVoltage(Chan ch, int x, int y)
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

    if(enable)
    {
        const int widthField = 91;
        const int heightField = 8;

        Color::E colorDraw = inverse ? Color::WHITE : color;
        if(inverse)
        {
            Painter::FillRegion(x, y, widthField, heightField, color);
        }

        char buffer[100] = {0};

        std::sprintf(buffer, "%s\xa5%s\xa5%s", (ch == Chan::A) ? (LANG_RU ? "1к" : "1c") : (LANG_RU ? "2к" : "2c"),
            couple[modeCouple], Range::ToString(range, multiplier));

        PText::Draw(x + 1, y, buffer, colorDraw);

        char bufferTemp[20];
        std::sprintf(buffer, "\xa5%s", rShift.ToString(range, multiplier, bufferTemp));
        PText::Draw(x + 46, y, buffer);
    }
}



void Display::WriteStringAndNumber(pchar text, int x, int y, int number)
{
    char buffer[100];
    PText::Draw(x, y, text, COLOR_FILL);

    if(number == 0)
    {
        std::sprintf(buffer, "-");
    }
    else
    {
        std::sprintf(buffer, "%d", number);
    }

    PText::DrawRelativelyRight(x + 41, y, buffer);
}



void Display::DrawLowPart()
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
    PText::Draw(x, y0, buffer);

    buffer[0] = 0;
    char bufForVal[20];
    std::sprintf(buffer, "\xa5%s", TShift::ToString(tShift, bufForVal));
    PText::Draw(x + 35, y0, buffer);

    buffer[0] = 0;

    if (MODE_WORK_IS_DIRECT)
    {
        pchar source[3] = {"1", "2", "\x82"};
        std::sprintf(buffer, "с\xa5\x10%s", source[TRIG_SOURCE]);
    }

    PText::Draw(x, y1, buffer, ColorTrig());

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
        PText::Draw(x + 18, y1, buffer);
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
        PText::Draw(x + 63, y1, buffer);
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
        char buf[20];
        float freq = FPGA::FreqMeter::GetFreq();
        if (freq == -1.0f) //-V550
        {
            std::strcat(mesFreq, "******");
        }
        else
        {
            std::strcat(mesFreq, Freq2String(freq, false, buf));
        }
        PText::Draw(x + 3, GRID_BOTTOM + 2, mesFreq);
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

    if(!SET_PEAKDET_IS_DISABLED)
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


void Display::DrawTimeForFrame(uint timeTicks)
{
    if(!SHOW_STATS)
    {
        return;
    }

    static char buffer[10];
    static bool first = true;
    static uint timeMSstartCalculation = 0;
    static int numFrames = 0;
    static float numMS = 0.0f;

    if(first)
    {
        timeMSstartCalculation = TIME_MS;
        first = false;
    }

    numMS += timeTicks / TICKS_IN_US / 1e3f;
    numFrames++;
    
    if((TIME_MS - timeMSstartCalculation) >= 500)
    {
        std::sprintf(buffer, "%.1fms/%d", numMS / numFrames, numFrames * 2);
        timeMSstartCalculation = TIME_MS;
        numMS = 0.0f;
        numFrames = 0;
    }

    Painter::DrawRectangle(Grid::Left(), Grid::FullBottom() - 10, 84, 10, COLOR_FILL);
    Painter::FillRegion(Grid::Left() + 1, Grid::FullBottom() - 9, 82, 8, COLOR_BACK);
    PText::Draw(Grid::Left() + 2, Grid::FullBottom() - 9, buffer, COLOR_FILL);

    char message[20] = {0};
    std::sprintf(message, "%d", Storage::NumFramesWithSameSettings());
    std::strcat(message, "/");
    char numAvail[10] = {0};
    std::sprintf(numAvail, "%d", Storage::NumberAvailableEntries());
    std::strcat(message, numAvail);
    PText::Draw(Grid::Left() + 50, Grid::FullBottom() - 9, message);
}


void RShift::DisableShowLevel(Chan ch)
{
    showLevel[ch] = false;
    Timer::Disable(ch.IsA() ? TypeTimer::ShowLevelRShiftA : TypeTimer::ShowLevelRShiftB);
}


void TrigLev::DisableShowLevel()
{
    TrigLev::showLevel = false;
    Timer::Disable(TypeTimer::ShowLevelTrigLev);
}


void Display::EnableTrigLabel(bool enable)
{
    TrigLev::fireLED = enable;
}


void Display::SetDrawMode(DrawMode::E mode, pFuncVV func)
{
    funcOnHand = func;

    if (mode == DrawMode::Hand)
    {
        Timer::Enable(TypeTimer::UpdateDisplay, 40, func);
    }
    else
    {
        Timer::Disable(TypeTimer::UpdateDisplay);
    }
}


void Display::SetAddDrawFunction(pFuncVV func)
{
    funcAdditionDraw = func;
}


void Display::RemoveAddDrawFunction()
{
    funcAdditionDraw = 0;
}


void Display::Clear()
{
    Painter::FillRegion(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 2, COLOR_BACK);
}


void Display::ShiftScreen(int delta)
{
    LIMITATION(SHIFT_IN_MEMORY, (int16)(SHIFT_IN_MEMORY + delta), 0, (int16)(ENUM_POINTS_FPGA::ToNumPoints() - 282));
}


void Display::RunAfterDraw(pFuncVV func)
{
    funcAfterDraw = func;
}
