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
#include "Hardware/FDrive/FDrive.h"
#include "Menu/Pages/Definition.h"
#include "Data/Processing.h"
#include "Utils/Math.h"
#include "Display/Symbols.h"
#include "Hardware/LAN/LAN.h"
#include "Hardware/VCP/VCP.h"
#include "Data/Storage.h"
#include "Display/Screen/LowPart.h"
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

    // Ќарисовать курсоры курсорных измерений.
    void DrawCursors();

    // ¬ывести значени€ автоматических измерений.
    void DrawMeasures();

    void FuncOnTimerDisableShowLevelRShiftA();

    void FuncOnTimerDisableShowLevelRShiftB();

    void DrawStringNavigation();

    bool NeedForClearScreen();

    void DrawTimeForFrame(uint timeTicks);

    // Ќарисовать горизонтальный курсор курсорных измерений.
    // y - числовое значение курсора.
    // xTearing - координата места, в котором необходимо сделать разрыв дл€ квадрата пересечени€.
    void DrawHorizontalCursor(int y, int xTearing);

    // Ќарисовать вертикальный курсор курсорных измерений.
    // x - числовое значение курсора.
    // yTearing - координата места, в котором необходимо сделать разрыв дл€ квадрата пересечени€.
    void DrawVerticalCursor(int x, int yTearing);

    // ¬ывести значение уровн€ синхронизации. 
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
        LowPart::Draw();
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
        float trigLev = TRIG_LEVEL_SOURCE.ToAbs(SET_RANGE(TRIG_SOURCE));     // WARN «десь дл€ внешней синхронизации неправильно рассчитываетс€ уровень.

        TrigSource::E trigSource = TRIG_SOURCE;

        if (TRIG_INPUT_IS_AC && trigSource <= TrigSource::ChannelB)
        {
            RShift rShift = SET_RSHIFT((Chan::E)trigSource);
            float rShiftAbs = rShift.ToAbs(SET_RANGE(trigSource));
            trigLev += rShiftAbs;
        }

        char buffer[20];
        std::strcpy(buffer, LANG_RU ? "”р синхр = " : "Trig lvl = ");
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
        bool bothCursors = !CURS_CNTRL_T_IS_DISABLE(source) && !CURS_CNTRL_U_IS_DISABLE(source);  // ѕризнак того, что включены и вертикальные и 
                                                                            // горизонтальные курсоры - надо нарисовать квадраты в местах пересечени€
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
