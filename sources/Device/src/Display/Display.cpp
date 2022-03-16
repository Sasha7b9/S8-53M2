// 2022/02/11 17:43:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "LAN/LAN.h"
#include "Symbols.h"
#include "Display.h"
#include "Grid.h"
#include "Painter.h"
#include "font/Font.h"
#include "Colors.h"
#include "FDrive/FDrive.h"
#include "FPGA/FPGA.h"
#include "Data/Storage.h"
#include "Hardware/Sound.h"
#include "VCP/VCP.h"
#include "Menu/Menu.h"
#include "Data/Measures.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Data/Processing.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Pages/Definition.h"
#include "Hardware/Timer.h"
#include "Data/Data.h"
#include <cmath>
#include <climits>
#include <cstring>
#include <cstdio>


namespace Display
{
    const int MAX_NUM_STRINGS         = 35;
    const int SIZE_BUFFER_FOR_STRINGS = 2000;

    char  *strings[MAX_NUM_STRINGS] = {0};
    char  bufferForStrings[SIZE_BUFFER_FOR_STRINGS] = {0};

    const int NUM_WARNINGS = 10;
    const char *warnings[NUM_WARNINGS] = {0};       // Здесь предупреждающие сообщения.
    uint        timeWarnings[NUM_WARNINGS] = {0};   // Здесь время, когда предупреждающее сообщение поступило на экран.

    pFuncVV funcOnHand       = 0;
    pFuncVV funcAdditionDraw = 0;
    pFuncVV funcAfterDraw    = 0;

    bool framesElapsed = false;
    int  numDrawingSignals = 0;

    int topMeasures = GRID_BOTTOM;

    void ShowWarn(pchar  message);

    // Нарисовать сетку.
    void DrawGrid(int left, int top, int width, int height);

    // Нарисовать полную сетку.
    void DrawFullGrid();

    void DrawCursorsWindow();

    void DrawCursorsRShift();

    // Нарисовать маркеры смещения по напряжению
    void DrawCursorRShift(Chan);

    // Нарисовать маркер уровня синхронизации.
    void DrawCursorTrigLevel();

    // Написать информацию под сеткой - в нижней части дисплея.
    void DrawLowPart();

    void DrawHiPart();

    void DrawHiRightPart();

    void DrawMath();

    void DrawSpectrum();

    // Вывести текущее время.
    void DrawTime(int x, int y);

    // Нарисовать курсоры курсорных измерений.
    void DrawCursors();

    // Вывести значения курсоров курсорных измерений.
    void WriteCursors();

    // Вывести значения автоматических измерений.
    void DrawMeasures();

    // Написать предупреждения.
    void DrawWarnings();

    int CalculateCountV();

    int CalculateFreeSize();

    bool ChannelNeedForDraw(const uint8* data, Chan, const DataSettings &);

    void DrawDataChannel(uint8* data, Chan, DataSettings &, int minY, int maxY);

    void DrawBothChannels(uint8* data0, uint8* data1);

    void DrawDataMemInt();

    void DrawDataInModeWorkLatest();

    void DrawDataMinMax();

    void DrawDataNormal();

    void DrawData();

    // Нарисовать окно памяти
    void DrawMemoryWindow();

    void DRAW_SPECTRUM(const uint8 *data, int numPoints, Chan);

    void DrawGridSpectrum();

    void DrawMarkersForMeasure(float scale, Chan);

    void DrawScaleLine(int x, bool forTrigLev);

    void WriteParametersFFT(Chan, float freq0, float density0, float freq1, float density1);

    void WriteTextVoltage(Chan, int x, int y);

    void FuncOnTimerDisableShowLevelRShiftA();

    void FuncOnTimerDisableShowLevelRShiftB();

    void FuncOnTimerDisableShowLevelTrigLev();

    void FuncOnTimerRShiftMarkersAutoHide();

    void OnTimerShowWarning();

    void DrawSignalLined(const uint8* data, const DataSettings* ds, int startPoint, int endPoint, int minY, int maxY,
        float scaleY, float scaleX, bool calculateFiltr);

    void DrawSignalPointed(const uint8* data, const DataSettings* ds, int startPoint, int endPoint, int minY, int maxY,
        float scaleY, float scaleX);

    void DrawSpectrumChannel(const float* spectrum, Color::E);

    void DrawStringInRectangle(int x, int y, char const* text);

    void DrawStringNavigation();

    int FirstEmptyString();

    bool NeedForClearScreen();

    void WriteStringAndNumber(pchar text, int x, int y, int number);

    void DrawGridType3(int left, int top, int right, int bottom, int centerX, int centerY, int deltaX, int deltaY,
        int stepX, int stepY);

    // shiftForPeakDet - если рисуем информацию с пикового детектора - то через shiftForPeakDet точек расположена
    // иниформация о максимумах.
    void DrawChannelInWindowMemory(int timeWindowRectWidth, int xVert0, int xVert1, int startI, int endI,
        const uint8* data, int rightX, Chan::E ch, int shiftForPeakDet);

    // shiftForPeakDet - если рисуем информацию с пикового детектора - то через shiftForPeakDet точек расположена
    // иниформация о максимумах.
    void DrawDataInRect(int x, int width, const uint8* data, int numElems, Chan::E ch, int shiftForPeakDet);

    void DrawTimeForFrame(uint timeMS);

    void DeleteFirstString();

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

    void AddString(pchar  string);

    int CalculateCountH();

    void DrawGridType1(int left, int top, int right, int bottom, float centerX, float centerY, float deltaX,
        float deltaY, float stepX, float stepY);

    void DrawGridType2(int left, int top, int right, int bottom, int deltaX, int deltaY, int stepX, int stepY);
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
    Storage::P2P::Reset();

    LAST_AFFECTED_CHANNEL = ch;

    if(TIME_SHOW_LEVELS)
    {
        (ch == Chan::A) ? (RShift::showLevelA = true) : (RShift::showLevelB = true);
        Timer::Enable((ch == Chan::A) ?
            TypeTimer::ShowLevelRShift0 :
            TypeTimer::ShowLevelRShift1, TIME_SHOW_LEVELS  * 1000,
            (ch == Chan::A) ?
            FuncOnTimerDisableShowLevelRShiftA :
            FuncOnTimerDisableShowLevelRShiftB);
    };

    Flags::needFinishDraw = true;
};


void Display::FuncOnTimerDisableShowLevelRShiftA()
{
    DisableShowLevelRShiftA();
}


void Display::FuncOnTimerDisableShowLevelRShiftB()
{
    Display::DisableShowLevelRShiftB();
}


void Display::RotateTrigLev()
{
    if (TIME_SHOW_LEVELS)
    {
        TrigLev::showLevel = true;
        Timer::Enable(TypeTimer::ShowLevelTrigLev, TIME_SHOW_LEVELS * 1000, FuncOnTimerDisableShowLevelTrigLev);
    }

    Flags::needFinishDraw = true;
}


void Display::FuncOnTimerDisableShowLevelTrigLev()
{
    Display::DisableShowLevelTrigLev();
}


bool Display::ChannelNeedForDraw(const uint8 *data, Chan ch, const DataSettings &ds)
{
    if (!data)
    {
        return false;
    }

    if (MODE_WORK_IS_DIRECT)
    {
        if (!ch.Enabled())
        {
            return false;
        }
    }
    else if (ds.Valid())
    {
        if ((ch == Chan::A && !ds.en_a) || (ch == Chan::B && !ds.en_b))
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}



void Display::DrawMarkersForMeasure(float scale, Chan ch)
{
    if (ch == Chan::Math)
    {
        return;
    }
    Color::SetCurrent(ColorCursors(ch));
    for(int numMarker = 0; numMarker < 2; numMarker++)
    {
        int pos = Processing::GetMarkerHorizontal(ch, numMarker);
        if(pos != ERROR_VALUE_INT && pos > 0 && pos < 200)
        {
            Painter::DrawDashedHLine((int)(Grid::FullBottom() - pos * scale), Grid::Left(), Grid::Right(), 3, 2, 0);
        }

        pos = Processing::GetMarkerVertical(ch, numMarker);
        if (pos != ERROR_VALUE_INT && pos > 0 && pos < Grid::Right())
        {
            Painter::DrawDashedVLine((int)(Grid::Left() + pos * scale), GRID_TOP, Grid::FullBottom(), 3, 2, 0);
        }
       
    }
}


#define CONVERT_DATA_TO_DISPLAY(out, in)                \
    out = (uint8)(maxY - ((in) - ValueFPGA::MIN) * scaleY);  \
    if(out < minY)          { out = (uint8)minY; }      \
    else if (out > maxY)    { out = (uint8)maxY; };


void Display::DrawSignalLined(const uint8 *data, const DataSettings *ds, int startPoint, int endPoint, int minY,
    int maxY, float scaleY, float scaleX, bool calculateFiltr)
{
    if (endPoint < startPoint)
    {
        return;
    }

    uint8 dataCD[281];

    int gridLeft = Grid::Left();
    int gridRight = Grid::Right();
    
    int numPoints = ENUM_POINTS_FPGA::ToNumPoints();
    int numSmoothing = Smoothing::ToPoints();

    if (ds->peakDet == PeackDetMode::Disable)
    {
        for (int i = startPoint; i < endPoint; i++)
        {
            float x0 = gridLeft + (i - startPoint) * scaleX;
            if (x0 >= gridLeft && x0 <= gridRight)
            {
                int index = i - startPoint;
                int y = calculateFiltr ? Math_CalculateFiltr(data, i, numPoints, numSmoothing) : data[i];
                CONVERT_DATA_TO_DISPLAY(dataCD[index], y);
            }
        }
    }
    else
    {
        int shift = (int)ds->PointsInChannel();

        int yMinNext = -1;
        int yMaxNext = -1;
       
        for (int i = startPoint; i < endPoint; i++)
        {
            float x = gridLeft + (i - startPoint) * scaleX;

            if (x >= gridLeft && x <= gridRight)
            {
                int yMin = yMinNext;

                if (yMin == -1)
                {
                    CONVERT_DATA_TO_DISPLAY(yMin, data[i + shift]);
                }

                int yMax = yMaxNext;

                if (yMax == -1)
                {
                    CONVERT_DATA_TO_DISPLAY(yMax, data[i]);
                }

                CONVERT_DATA_TO_DISPLAY(yMaxNext, data[i + 1]);

                if (yMaxNext < yMin)
                {
                    yMin = yMaxNext + 1;
                }

                CONVERT_DATA_TO_DISPLAY(yMinNext, data[i + shift + 1]);

                if (yMinNext > yMax)
                {
                    yMax = yMinNext - 1;
                }

                Painter::DrawVLine((int)x, yMin, yMax);
            }
        }
    }

    if(endPoint - startPoint < 281)
    {
        int _numPoints = 281 - (endPoint - startPoint);
        for(int i = 0; i < _numPoints; i++)
        {
            int index = endPoint - startPoint + i;
            CONVERT_DATA_TO_DISPLAY(dataCD[index], ValueFPGA::MIN);
        }
    }

    if(ds->peakDet == PeackDetMode::Disable)
    {
        CONVERT_DATA_TO_DISPLAY(dataCD[280], data[endPoint]);
        Painter::DrawSignal(Grid::Left(), dataCD, true);
    }
}



void Display::DrawSignalPointed(const uint8 *data, const DataSettings *ds, int startPoint, int endPoint, int minY,
    int maxY, float scaleY, float scaleX)
{
    int numPoints = ENUM_POINTS_FPGA::ToNumPoints();
    int numSmoothing = Smoothing::ToPoints();
    
    uint8 dataCD[281];

    if (scaleX == 1.0f) //-V550
    {
        for (int i = startPoint; i < endPoint; i++)
        {
            int index = i - startPoint;
            CONVERT_DATA_TO_DISPLAY(dataCD[index], Math_CalculateFiltr(data, i, numPoints, numSmoothing));
        }
        Painter::DrawSignal(Grid::Left(), dataCD, false);

        if (ds->peakDet)
        {
            int size = endPoint - startPoint;
            startPoint += numPoints;
            endPoint = startPoint + size;;

            for (int i = startPoint; i < endPoint; i++)
            {
                int index = i - startPoint;
                CONVERT_DATA_TO_DISPLAY(dataCD[index], Math_CalculateFiltr(data, i, numPoints, numSmoothing));
            }
            Painter::DrawSignal(Grid::Left(), dataCD, false);
        }
    }
    else
    {
        for (int i = startPoint; i < endPoint; i++)
        {
            int index = i - startPoint;
            int dat = 0;
            CONVERT_DATA_TO_DISPLAY(dat, Math_CalculateFiltr(data, i, numPoints, numSmoothing));
            Painter::SetPoint((int)(Grid::Left() + index * scaleX), dat);
        }
    }
}


// Если data == 0, то данные брать из GetData
void Display::DrawDataChannel(uint8 *data, Chan ch, DataSettings &ds, int minY, int maxY)
{
    bool calculateFiltr = true;

    if (!ChannelNeedForDraw(data, ch, ds))
    {
        return;
    }

    float scaleY = (float)(maxY - minY) / (ValueFPGA::MAX - ValueFPGA::MIN);
    float scaleX = (float)Grid::Width() / 280.0f;

    if(SHOW_MEASURES)
    {
        DrawMarkersForMeasure(scaleY, ch);
    }

    BitSet32 points = SettingsDisplay::PointsOnDisplay();
    int first = points.half_iword[0];
    int last = points.half_iword[1];

    if (ds.InModeP2P())
    {
        if (ds.rec_point < Grid::Width())
        {
            first = 0;
            last = ds.rec_point + 1;
        }
        else
        {
            last = ds.rec_point + 1;
            first = last - Grid::Width();
        }
    }

    Color::SetCurrent(ColorChannel(ch));

    if(MODE_DRAW_IS_SIGNAL_LINES)
    {
        DrawSignalLined(data, &ds, first, last, minY, maxY, scaleY, scaleX, calculateFiltr);
    }
    else
    {
        DrawSignalPointed(data, &ds, first, last, minY, maxY, scaleY, scaleX);
    }
}


void Display::DrawMath()
{
    if (DISABLED_DRAW_MATH || Storage::GetData(Chan::A, 0) == 0 || Storage::GetData(Chan::B, 0) == 0)
    {
        return;
    }

    DataStruct data;

    Processing::GetData(data);

    float dataAbs0[FPGA::MAX_POINTS * 2];
    float dataAbs1[FPGA::MAX_POINTS * 2];

    ValueFPGA::ToVoltage(data.A.Data(), data.ds.BytesInChannel(), data.ds.range[Chan::A], (int16)data.ds.rShiftA, dataAbs0);
    ValueFPGA::ToVoltage(data.B.Data(), data.ds.BytesInChannel(), data.ds.range[Chan::B], (int16)data.ds.rShiftB, dataAbs1);

    Math_CalculateMathFunction(dataAbs0, dataAbs1, data.ds.BytesInChannel());
    
    uint8 points[FPGA::MAX_POINTS * 2];
    ValueFPGA::FromVoltage(dataAbs0, data.ds.BytesInChannel(), SET_RANGE_MATH, SET_RSHIFT_MATH, points);

    DrawDataChannel(points, Chan::Math, data.ds, Grid::MathTop(), Grid::MathBottom());

    static const int width = 71;
    static const int height = 10;
    int delta = (SHOW_STRING_NAVIGATION && MODE_DRAW_MATH_IS_TOGETHER) ? 10 : 0;
    Painter::DrawRectangle(Grid::Left(), Grid::MathTop() + delta, width, height, COLOR_FILL);
    Painter::FillRegion(Grid::Left() + 1, Grid::MathTop() + 1 + delta, width - 2, height - 2, COLOR_BACK);
    Divider::E multiplier = MATH_MULTIPLIER;
    PText::Draw(Grid::Left() + 2, Grid::MathTop() + 1 + delta, Range::ToString(SET_RANGE_MATH, multiplier),
        COLOR_FILL);
    PText::Draw(Grid::Left() + 25, Grid::MathTop() + 1 + delta, ":");
    char buffer[20];
    PText::Draw(Grid::Left() + 27, Grid::MathTop() + 1 + delta, SET_RSHIFT_MATH.ToString(SET_RANGE_MATH, multiplier,
        buffer));
}


void Display::DrawSpectrumChannel(const float *spectrum, Color::E color)
{
    Color::SetCurrent(color);
    int gridLeft = Grid::Left();
    int gridBottom = Grid::MathBottom();
    int gridHeight = Grid::MathHeight();
    for (int i = 0; i < 256; i++)
    {
        Painter::DrawVLine(gridLeft + i, gridBottom, (int)(gridBottom - gridHeight * spectrum[i]));
    }
}


void Display::WriteParametersFFT(Chan ch, float freq0, float density0, float freq1, float density1)
{
    int x = Grid::Left() + 259;
    int y = Grid::ChannelBottom() + 5;
    int dY = 10;

    char buffer[20];
    Color::SetCurrent(COLOR_FILL);
    PText::Draw(x, y, Freq2String(freq0, false, buffer));
    y += dY;
    PText::Draw(x, y, Freq2String(freq1, false, buffer));
    if (ch == Chan::A)
    {
        y += dY + 2;
    }
    else
    {
        y += dY * 3 + 4;
    }
    Color::SetCurrent(ColorChannel(ch));
    PText::Draw(x, y, SCALE_FFT_IS_LOG ? Float2Db(density0, 4, buffer) : Float2String(density0, false, 7, buffer));
    y += dY;
    PText::Draw(x, y, SCALE_FFT_IS_LOG ? Float2Db(density1, 4, buffer) : Float2String(density1, false, 7, buffer));
}



void Display::DRAW_SPECTRUM(const uint8 *data, int numPoints, Chan ch)
{
    if (!ch.Enabled())
    {
        return;
    }

    float dataR[FPGA::MAX_POINTS * 2];
    float spectrum[FPGA::MAX_POINTS * 2];

    float freq0 = 0.0f;
    float freq1 = 0.0f;
    float density0 = 0.0f;
    float density1 = 0.0f;
    int y0 = 0;
    int y1 = 0;
    int s = 2;

    ValueFPGA::ToVoltage(data, numPoints, Data::dir.ds.range[ch], (ch == Chan::A) ?
        (int16)Data::dir.ds.rShiftA :
        (int16)Data::dir.ds.rShiftB, dataR);

    Math_CalculateFFT(dataR, numPoints, spectrum, &freq0, &density0, &freq1, &density1, &y0, &y1);
    DrawSpectrumChannel(spectrum, ColorChannel(ch));

    if (!Menu::IsShown() || Menu::IsMinimize())
    {
        Color::E color = COLOR_FILL;
        WriteParametersFFT(ch, freq0, density0, freq1, density1);
        Painter::DrawRectangle(FFT_POS_CURSOR_0 + Grid::Left() - s, y0 - s, s * 2, s * 2, color);
        Painter::DrawRectangle(FFT_POS_CURSOR_1 + Grid::Left() - s, y1 - s, s * 2, s * 2);

        Painter::DrawVLine(Grid::Left() + FFT_POS_CURSOR_0, Grid::MathBottom(), y0 + s);
        Painter::DrawVLine(Grid::Left() + FFT_POS_CURSOR_1, Grid::MathBottom(), y1 + s);
    }
}


void Display::DrawSpectrum()
{
    if (!ENABLED_FFT)
    {
        return;
    }

    Painter::DrawVLine(Grid::Right(), Grid::ChannelBottom() + 1, Grid::MathBottom() - 1, COLOR_BACK);

    if (MODE_WORK_IS_DIRECT)
    {
        int numPoints = ENUM_POINTS_FPGA::ToNumPoints();
        if (numPoints < 512)
        {
            numPoints = 256;
        }

        if (SOURCE_FFT_IS_A)
        {
            DRAW_SPECTRUM(Data::dir.A.Data(), numPoints, Chan::A);
        }
        else if (SOURCE_FFT_IS_B)
        {
            DRAW_SPECTRUM(Data::dir.B.Data(), numPoints, Chan::B);
        }
        else
        {
            if (LAST_AFFECTED_CHANNEL_IS_A)
            {
                DRAW_SPECTRUM(Data::dir.B.Data(), numPoints, Chan::B);
                DRAW_SPECTRUM(Data::dir.A.Data(), numPoints, Chan::A);
            }
            else
            {
                DRAW_SPECTRUM(Data::dir.A.Data(), numPoints, Chan::A);
                DRAW_SPECTRUM(Data::dir.B.Data(), numPoints, Chan::B);
            }
        }
    }

    Painter::DrawHLine(Grid::ChannelBottom(), Grid::Left(), Grid::Right(), COLOR_FILL);
    Painter::DrawHLine(Grid::MathBottom(), Grid::Left(), Grid::Right());
}



void Display::DrawBothChannels(uint8 *data0, uint8 *data1)
{
	if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawDataChannel(data0, Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(data1, Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
    }
    else
    {
        DrawDataChannel(data1, Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(data0, Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
    }
}


void Display::DrawDataMemInt()
{
    if(Data::ins.Valid())
     {
        DrawDataChannel(Data::ins.A.Data(), Chan::A, Data::ins.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Data::ins.B.Data(), Chan::B, Data::ins.ds, GRID_TOP, Grid::ChannelBottom());
    }
}


void Display::DrawDataInModeWorkLatest()
{
    if (Data::last.Valid())
    {
        DrawDataChannel(Data::last.A.Data(), Chan::A, Data::last.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Data::last.B.Data(), Chan::B, Data::last.ds, GRID_TOP, Grid::ChannelBottom());
    }
}


void Display::DrawDataMinMax()
{
    ModeDrawSignal modeDrawSignalOld = MODE_DRAW_SIGNAL;
    MODE_DRAW_SIGNAL = ModeDrawSignal_Lines;
    if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawDataChannel(Storage::GetLimitation(Chan::A, 0), Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(Chan::A, 1), Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(Chan::B, 0), Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(Chan::B, 1), Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
    }
    else
    {
        DrawDataChannel(Storage::GetLimitation(Chan::B, 0), Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(Chan::B, 1), Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(Chan::A, 0), Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(Chan::A, 1), Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
    }
    MODE_DRAW_SIGNAL = modeDrawSignalOld;
}


void Display::DrawDataNormal()
{
    static void *prevAddr = 0;

    DataStruct data;

    Processing::GetData(data);

    int16 numSignals = (int16)Storage::NumElementsWithSameSettings();
    LIMITATION(numSignals, numSignals, 1, NUM_ACCUM);

    if (numSignals == 1 || ENUM_ACCUM_IS_INFINITY || MODE_ACCUM_IS_RESET || TBase::InModeRandomizer())
    {
        DrawBothChannels(0, 0);

        if (prevAddr == 0 || prevAddr != data.ds.addrPrev)
        {
            Display::numDrawingSignals++;
            prevAddr = data.ds.addrPrev;
        }
    }
    else
    {
        for (int i = 0; i < numSignals; i++)
        {
            DrawBothChannels(Storage::GetData(Chan::A, i), Storage::GetData(Chan::B, i));
        }
    }
}



void Display::DrawData()
{
    if (Storage::NumElements())
    {
        if (MODE_WORK_IS_MEMINT)
        {
            if (!MODE_SHOW_MEMINT_IS_DIRECT)
            {
                DrawDataMemInt();
            }
            if (!MODE_SHOW_MEMINT_IS_SAVED)
            {
                DrawDataNormal();
            }
        }
        else if (MODE_WORK_IS_LATEST)
        {
            DrawDataInModeWorkLatest();
        }
        else
        {
            if (PageMemory::Internal::showAlways)
            {
                DrawDataMemInt();
            }

            DrawDataNormal();
        }

        if (NUM_MIN_MAX != 1)
        {
            DrawDataMinMax();
        }
    }

    Painter::DrawRectangle(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight(), COLOR_FILL);
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
        DataSettings &ds = MODE_WORK_IS_MEMINT ? Data::ins.ds : Data::last.ds;

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


void Display::DrawHiPart()
{
    WriteCursors();
    DrawHiRightPart();
}


void Display::DrawDataInRect(int x, int width, const uint8 *data, int numElems, Chan::E ch, int shiftForPeakDet)
{
    if(numElems == 0)
    {
        return;
    }

    width--;
    float elemsInColumn = (float)numElems / (float)width;

    uint8 min[300];
    uint8 max[300];

    std::memset(min, 255, 300);
    std::memset(max, 0, 300);

    if (SET_TBASE >= TBase::_20ms && SET_PEAKDET_IS_ENABLE)
    {
        for (int col = 0; col < width; col++)
        {
            float firstElem = col * elemsInColumn;
            float lastElem = firstElem + elemsInColumn - 1;
            min[col] = data[(int)firstElem];
            max[col] = data[(int)firstElem + 1];

            for (int elem = (int)(firstElem + 2); elem <= lastElem; elem += 2)
            {
                SET_MIN_IF_LESS(data[elem], min[col]);
                SET_MAX_IF_LARGER(data[elem + 1], max[col]);
            }
        }
    }
    else if (shiftForPeakDet == 0)
    {
        uint8 *iMin = &min[0];
        uint8 *iMax = &max[0];

        for (int col = 0; col < width; col++, iMin++, iMax++)
        {
            int firstElem = (int)(col * elemsInColumn);
            int lastElem = (int)(firstElem + elemsInColumn - 1);
            *iMin = data[firstElem];
            *iMax = data[firstElem];
            for (int elem = firstElem + 1; elem <= lastElem; elem++)
            {
                SET_MIN_IF_LESS(data[elem], *iMin);
                SET_MAX_IF_LARGER(data[elem], *iMax);
            }
        }
    }
    else        // А здесь будет, если пик. дет. включен
    {
        for (int col = 0; col < width; col++)
        {
            float firstElem = col * elemsInColumn;
            float lastElem = firstElem + elemsInColumn - 1;
            min[col] = data[(int)firstElem];
            max[col] = data[(int)firstElem + shiftForPeakDet];

            for (int elem = (int)(firstElem + 1); elem <= lastElem; elem++)
            {
                SET_MIN_IF_LESS(data[elem], min[col]);
                SET_MAX_IF_LARGER(data[elem + shiftForPeakDet], max[col]);
            }
        }
    }

    int bottom = 16;
    int height = 14;
    float scale = (float)height / (float)(ValueFPGA::MAX - ValueFPGA::MIN);

#define ORDINATE(x) (uint8)(bottom - scale * Math::Limitation<int>((x) - ValueFPGA::MIN, 0, 200))

    static const int NUM_POINTS = (300 * 2);
    uint8 points[NUM_POINTS];

    points[0] = ORDINATE(max[0]);
    points[1] = ORDINATE(min[0]);

    for (int i = 1; i < width; i++)
    {
        int value0 = min[i] > max[i - 1] ? max[i - 1] : min[i];
        int value1 = max[i] < min[i - 1] ? min[i - 1] : max[i];
        points[i * 2] = ORDINATE(value1);
        points[i * 2 + 1] = ORDINATE(value0);
    }

    uint8 transparency = ORDINATE(ValueFPGA::NONE);

    if (width < 256)
    {
        Painter::DrawVLineArray(x, width, points, ColorChannel(ch), transparency);
    }
    else
    {
        Painter::DrawVLineArray(x, 255, points, ColorChannel(ch), transparency);
        Painter::DrawVLineArray(x + 255, width - 255, points + 255 * 2, ColorChannel(ch), transparency);
    }
}


void Display::DrawChannelInWindowMemory(int timeWindowRectWidth, int xVert0, int xVert1, int startI, int endI,
    const uint8 *data, int rightX, Chan::E ch, int shiftForPeakDet)
{
    DrawDataInRect(1, xVert0 - 1, &(data[0]), startI, ch, shiftForPeakDet);

    DrawDataInRect(xVert0 + 2, timeWindowRectWidth - 2, &(data[startI]),281, ch, shiftForPeakDet);
    DrawDataInRect(xVert1 + 2, rightX - xVert1 + 2,     &(data[endI + 1]), ENUM_POINTS_FPGA::ToNumPoints() - endI, ch,
        shiftForPeakDet);
}


void Display::DrawMemoryWindow()
{
    DataStruct *dat = &Data::ins;

    if(MODE_WORK_IS_DIRECT || MODE_WORK_IS_LATEST)
    {
        dat = &Data::dir;
    }
    
    int leftX = 3;
    int top = 1;
    int height = GRID_TOP - 3;
    int bottom = top + height;

    static const int rightXses[3] = {276, 285, 247};
    int rightX = rightXses[MODE_WORK];

    if (PageCursors::NecessaryDrawCursors())
    {
        rightX = 68;
    }

    int timeWindowRectWidth = (int)((rightX - leftX) * (282.0f / ENUM_POINTS_FPGA::ToNumPoints()));
    float scaleX = (float)(rightX - leftX + 1) / ENUM_POINTS_FPGA::ToNumPoints();

    int startI = SHIFT_IN_MEMORY;
    int endI = startI + 281;

    const int xVert0 = (int)(leftX + SHIFT_IN_MEMORY * scaleX);
    const int xVert1 = (int)(leftX + SHIFT_IN_MEMORY * scaleX + timeWindowRectWidth);
    bool showFull = set.display.showFullMemoryWindow;
    Painter::DrawRectangle(xVert0, top + (showFull ? 0 : 1), xVert1 - xVert0, bottom - top - (showFull ? 0 : 2),
        COLOR_FILL);

    if (showFull)
    {
        if (dat->A.Size() || dat->A.Size())
        {
            Chan::E chanFirst = LAST_AFFECTED_CHANNEL_IS_A ? Chan::B : Chan::A;
            Chan::E chanSecond = LAST_AFFECTED_CHANNEL_IS_A ? Chan::A : Chan::B;
            const uint8 *dataFirst = LAST_AFFECTED_CHANNEL_IS_A ? dat->B.Data() : dat->A.Data();
            const uint8 *dataSecond = LAST_AFFECTED_CHANNEL_IS_A ? dat->A.Data() : dat->B.Data();

            int shiftForPeakDet = (dat->ds.peakDet == PeackDetMode::Disable) ? 0 : dat->ds.PointsInChannel();

            if (ChannelNeedForDraw(dataFirst, chanFirst, dat->ds))
            {
                DrawChannelInWindowMemory(timeWindowRectWidth, xVert0, xVert1, startI, endI, dataFirst, rightX,
                    chanFirst, shiftForPeakDet);
            }

            if (ChannelNeedForDraw(dataSecond, chanSecond, dat->ds))
            {
                DrawChannelInWindowMemory(timeWindowRectWidth, xVert0, xVert1, startI, endI, dataSecond, rightX,
                    chanSecond, shiftForPeakDet);
            }
        }
    }
    else
    {
        Painter::DrawVLine(leftX - 2, top, bottom, COLOR_FILL);
        Painter::DrawVLine(rightX + 2, top, bottom);
        Painter::DrawHLine((bottom + top) / 2 - 3, leftX, xVert0 - 2);
        Painter::DrawHLine((bottom + top) / 2 + 3, leftX, xVert0 - 2);
        Painter::DrawHLine((bottom + top) / 2 + 3, xVert1 + 2, rightX);
        Painter::DrawHLine((bottom + top) / 2 - 3, xVert1 + 2, rightX);
    }

    int x[] = {leftX, (rightX - leftX) / 2 + leftX + 1, rightX};
    int x0 = x[SET_TPOS];

    // Маркер TPos
    Painter::FillRegion(x0 - 3, 9, 6, 6, COLOR_BACK);
    PText::DrawChar(x0 - 3, 9, SYMBOL_TPOS_1, COLOR_FILL);

    // Маркер tShift
    float scale = (float)(rightX - leftX + 1) / ((float)ENUM_POINTS_FPGA::ToNumPoints() -
        (ENUM_POINTS_FPGA::ToNumPoints() == 281 ? 1 : 0));

    float xShift = 1 + (TPos::InPoints(Data::dir.ds.e_points_in_channel, SET_TPOS) - Data::dir.ds.tShift * 2) * scale;
    
    if(xShift < leftX - 2)
    {
        xShift = (float)(leftX - 2);
    }

    Painter::FillRegion((int)(xShift - 1), 3, 6, 6, COLOR_BACK);
    Painter::FillRegion((int)(xShift), 4, 4, 4, COLOR_FILL);
    Color::SetCurrent(COLOR_BACK);

    if(xShift == leftX - 2)
    {
        xShift = (float)(leftX - 2);
        Painter::DrawLine((int)(xShift + 3), 5, (int)(xShift + 3), 7);
        Painter::DrawLine((int)(xShift + 1), 6, (int)(xShift + 2), 6);
    }
    else if(xShift > rightX - 1)
    {
        xShift = (float)(rightX - 2);
        Painter::DrawLine((int)(xShift + 1), 5, (int)(xShift + 1), 7);
        Painter::DrawLine((int)(xShift + 2), 6, (int)(xShift + 3), 6);
    }
    else
    {
        Painter::DrawLine((int)(xShift + 1), 5, (int)(xShift + 3), 5);
        Painter::DrawLine((int)(xShift + 2), 6, (int)(xShift + 2), 7);
    }
}


void Display::WriteCursors()
{
    char buffer[20];
    int startX = 43;

    if(MODE_WORK_IS_DIRECT)
    {
        startX += 29;
    }

    int x = startX;
    int y1 = 0;
    int y2 = 9;

    if(PageCursors::NecessaryDrawCursors())
    {
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 3;
        Chan::E source = CURS_SOURCE;
        Color::E colorText = ColorChannel(source);

        if(!CURS_CNTRL_U_IS_DISABLE(source))
        {
            PText::Draw(x, y1, "1:", colorText);
            PText::Draw(x, y2, "2:");
            x += 7;
            PText::Draw(x, y1, PageCursors::GetCursVoltage(source, 0, buffer));
            PText::Draw(x, y2, PageCursors::GetCursVoltage(source, 1, buffer));
            x = startX + 49;
            float pos0 = Math::VoltageCursor(PageCursors::GetCursPosU(source, 0), SET_RANGE(source), SET_RSHIFT(source));
            float pos1 = Math::VoltageCursor(PageCursors::GetCursPosU(source, 1), SET_RANGE(source), SET_RSHIFT(source));
            float delta = std::fabsf(pos1 - pos0);
            PText::Draw(x, y1, ":dU=");
            PText::Draw(x + 17, y1, Voltage2String(delta, false, buffer));
            PText::Draw(x, y2, ":");
            PText::Draw(x + 10, y2, PageCursors::GetCursorPercentsU(source, buffer));
        }

        x = startX + 101;
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 3;

        if(!CURS_CNTRL_T_IS_DISABLE(source))
        {
            Color::SetCurrent(colorText);
            PText::Draw(x, y1, "1:");
            PText::Draw(x, y2, "2:");
            x+=7;
            PText::Draw(x, y1, PageCursors::GetCursorTime(source, 0, buffer));
            PText::Draw(x, y2, PageCursors::GetCursorTime(source, 1, buffer));
            x = startX + 153;
            float pos0 = Math::TimeCursor(CURS_POS_T0(source), SET_TBASE);
            float pos1 = Math::TimeCursor(CURS_POS_T1(source), SET_TBASE);
            float delta = std::fabsf(pos1 - pos0);
            PText::Draw(x, y1, ":dT=");
            char buf[20];
            PText::Draw(x + 17, y1, Time2String(delta, false, buf));
            PText::Draw(x, y2, ":");
            PText::Draw(x + 8, y2, PageCursors::GetCursorPercentsT(source, buf));

            if(CURSORS_SHOW_FREQ)
            {
                int width = 65;
                int x0 = Grid::Right() - width;
                Painter::DrawRectangle(x0, GRID_TOP, width, 12, COLOR_FILL);
                Painter::FillRegion(x0 + 1, GRID_TOP + 1, width - 2, 10, COLOR_BACK);
                PText::Draw(x0 + 1, GRID_TOP + 2, "1/dT=", colorText);
                char buff[20];
                PText::Draw(x0 + 25, GRID_TOP + 2, Freq2String(1.0f / delta, false, buff));
            }
        }
    }
}


void Display::DrawHiRightPart()
    {
    // Синхроимпульс
    int y = 2;

    static const int xses[3] = {280, 271, 251};
    int x = xses[MODE_WORK];

    if (!MODE_WORK_IS_LATEST)
    {
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);

        x += 2;

        if (TrigLev::fireLED)
        {
            Painter::FillRegion(x, 1 + y, GRID_TOP - 3, GRID_TOP - 7);
            PText::Draw(x + 3, 3 + y, LANG_RU ? "СИ" : "Tr", COLOR_BACK);
        }
    }

    // Режим работы
    static pchar strings_[][2] =
    {
        {"ИЗМ",     "MEAS"},
        {"ПОСЛ",    "LAST"},
        {"ВНТР",    "INT"}
    };

    if(!MODE_WORK_IS_DIRECT)
    {
        x += 18;
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 2;
        PText::Draw(LANG_RU ? x : x + 3, -1, LANG_RU ? "режим" : "mode");
        PText::DrawStringInCenterRect(x + 1, 9, 25, 8, strings_[MODE_WORK][LANG]);
    }
    else
    {
        x -= 9;
    }

    if (!MODE_WORK_IS_LATEST)
    {

        x += 27;
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);

        x += 2;
        y = 1;
        if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Work)
        {
            PText::Draw4SymbolsInRect(x, 1, SYMBOL_PLAY);
        }
        else if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop)
        {
            Painter::FillRegion(x + 3, y + 3, 10, 10);
        }
        else if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Wait)
        {
            int w = 4;
            int h = 14;
            int delta = 4;
            x = x + 2;
            Painter::FillRegion(x, y + 1, w, h);
            Painter::FillRegion(x + w + delta, y + 1, w, h);
        }
    }
}



void Display::DrawCursorsRShift()
{
    if (!DISABLED_DRAW_MATH)
    {
        DrawCursorRShift(Chan::Math);
    }
    if(LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawCursorRShift(Chan::A);
        DrawCursorRShift(Chan::B);
    }
    else
    {
        DrawCursorRShift(Chan::B);
        DrawCursorRShift(Chan::A);
    }
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
    uint timeStart = TIME_TICKS;

    if (funcOnHand != 0)
    {
        funcOnHand();
        return;
    }

    bool needClear = NeedForClearScreen();

    if (needClear)
    {
        Painter::BeginScene(COLOR_BACK);
        DrawMemoryWindow();
        DrawFullGrid();
    }

    DrawData();

    if (needClear)
    {
        DrawMath();
        DrawSpectrum();
        DrawCursors();
        DrawHiPart();
        DrawLowPart();
        DrawCursorsWindow();
        TShift::Draw();
        DrawCursorTrigLevel();
        DrawCursorsRShift();
        DrawMeasures();
        DrawStringNavigation();
    }
    
    Menu::Draw();

    if (needClear)
    {
        DrawWarnings();

        if (funcAdditionDraw)
        {
            funcAdditionDraw();
        }
    }

    DrawConsole();

    if (needClear)    
    {
        WriteValueTrigLevel();
    }

    DrawTimeForFrame(TIME_TICKS - timeStart);

    Color::SetCurrent(COLOR_FILL);

    Painter::EndScene(endScene);

    if (FDrive::needSave)
    {
        if (Painter::SaveScreenToFlashDrive())
        {
            Display::ShowWarningGood(Warning::FileIsSaved);
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


void Display::DrawGridSpectrum()
{
    if (SCALE_FFT_IS_LOG)
    {
        static const int nums[] = {4, 6, 8};
        static pchar strs[] = {"0", "-10", "-20", "-30", "-40", "-50", "-60", "-70"};
        int numParts = nums[FFT_MAX_DB];
        float scale = (float)Grid::MathHeight() / numParts;
        for (int i = 1; i < numParts; i++)
        {
            int y = (int)(Grid::MathTop() + i * scale);
            Painter::DrawHLine(y, Grid::Left(), Grid::Left() + 256, ColorGrid());
            if (!Menu::IsMinimize())
            {
                Color::SetCurrent(COLOR_FILL);
                PText::Draw(3, y - 4, strs[i]);
            }
        }
        if (!Menu::IsMinimize())
        {
            Color::SetCurrent(COLOR_FILL);
            PText::Draw(5, Grid::MathTop() + 1, "дБ");
        }
    }
    else if (SCALE_FFT_IS_LINEAR)
    {
        static pchar strs[] = {"1.0", "0.8", "0.6", "0.4", "0.2"};
        float scale = (float)Grid::MathHeight() / 5;
        for (int i = 1; i < 5; i++)
        {
            int y = (int)(Grid::MathTop() + i * scale);
            Painter::DrawHLine(y, Grid::Left(), Grid::Left() + 256, ColorGrid());
            if (!Menu::IsMinimize())
            {
                PText::Draw(5, y - 4, strs[i], COLOR_FILL);
            }
        }
    }
    Painter::DrawVLine(Grid::Left() + 256, Grid::MathTop(), Grid::MathBottom(), COLOR_FILL);
}



void Display::DrawFullGrid()
{
    if (SettingsDisplay::IsSeparate())
    {
        DrawGrid(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight() / 2);

        if (ENABLED_FFT)
        {
            DrawGridSpectrum();
        }

        if (!DISABLED_DRAW_MATH)
        {
            DrawGrid(Grid::Left(), GRID_TOP + Grid::FullHeight() / 2, Grid::Width(), Grid::FullHeight() / 2);
        }

        Painter::DrawHLine(GRID_TOP + Grid::FullHeight() / 2, Grid::Left(), Grid::Left() + Grid::Width(), COLOR_FILL);
    }
    else
    {
        DrawGrid(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight());
    }
}



int Display::CalculateCountV()
{
    if (MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if (MEAS_NUM_IS_1_5)
        {
            return MEAS_SOURCE_IS_A_B ? 42 : 44;
        }
        if (MEAS_NUM_IS_2_5)
        {
            return MEAS_SOURCE_IS_A_B ? 69 : 39;
        }
        if (MEAS_NUM_IS_3_5)
        {
            return MEAS_SOURCE_IS_A_B ? 54 : 68;
        }
    }

    return 49;
}



int Display::CalculateCountH()
{
    if (MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if (MEAS_NUM_IS_6_1)
        {
            return 73;
        }
        if (MEAS_NUM_IS_6_2)
        {
            return 83;
        }
    }
    return 69;
}



void Display::DrawGridType1(int left, int top, int right, int bottom, float centerX, float centerY, float deltaX,
    float deltaY, float stepX, float stepY)
{
    uint16 masX[17];
    masX[0] = (uint16)(left + 1);

    for (int i = 1; i < 7; i++)
    {
        masX[i] = (uint16)(left + deltaX * i);
    }
    for (int i = 7; i < 10; i++)
    {
        masX[i] = (uint16)(centerX - 8 + i);
    }
    for (int i = 10; i < 16; i++)
    {
        masX[i] = (uint16)(centerX + deltaX * (i - 9));
    }

    masX[16] = (uint16)(right - 1);

    Painter::DrawMultiVPointLine(17, (int)(top + stepY), masX, (int)stepY, CalculateCountV(), ColorGrid());

    uint8 mas[13];
    mas[0] = (uint8)(top + 1);

    for (int i = 1; i < 5; i++)
    {
        mas[i] = (uint8)(top + deltaY * i);
    }
    for (int i = 5; i < 8; i++)
    {
        mas[i] = (uint8)(centerY - 6 + i);
    }
    for (int i = 8; i < 12; i++)
    {
        mas[i] = (uint8)(centerY + deltaY * (i - 7));
    }

    mas[12] = (uint8)(bottom - 1);

    Painter::DrawMultiHPointLine(13, (int)(left + stepX), mas, (int)stepX, CalculateCountH(), ColorGrid());
}



void Display::DrawGridType2(int left, int top, int right, int bottom, int deltaX, int deltaY, int stepX, int stepY)
{ 
    uint16 masX[15];
    masX[0] = (uint16)(left + 1);
    for (int i = 1; i < 14; i++)
    {
        masX[i] = (uint16)(left + deltaX * i);
    }
    masX[14] = (uint16)(right - 1);
    Painter::DrawMultiVPointLine(15, top + stepY, masX, stepY, CalculateCountV(), ColorGrid());

    uint8 mas[11];
    mas[0] = (uint8)(top + 1);
    for (int i = 1; i < 10; i++)
    {
        mas[i] = (uint8)(top + deltaY * i);
    }
    mas[10] = (uint8)(bottom - 1);
    Painter::DrawMultiHPointLine(11, left + stepX, mas, stepX, CalculateCountH(), ColorGrid());
}



void Display::DrawGridType3(int left, int top, int right, int bottom, int centerX, int centerY, int deltaX, int deltaY,
    int stepX, int stepY)
{
    Painter::DrawHPointLine(centerY, left + stepX, right, stepX);
    uint8 masY[6] = {(uint8)(top + 1), (uint8)(top + 2), (uint8)(centerY - 1), (uint8)(centerY + 1),
        (uint8)(bottom - 2), (uint8)(bottom - 1)};
    Painter::DrawMultiHPointLine(6, left + deltaX, masY, deltaX, (right - top) / deltaX, ColorGrid());
    Painter::DrawVPointLine(centerX, top + stepY, bottom, (float)stepY, ColorGrid());
    uint16 masX[6] = {(uint16)(left + 1), (uint16)(left + 2), (uint16)(centerX - 1), (uint16)(centerX + 1),
        (uint16)(right - 2), (uint16)(right - 1)};
    Painter::DrawMultiVPointLine(6, top + deltaY, masX, deltaY, (bottom - top) / deltaY, ColorGrid());
}



void Display::DrawGrid(int left, int top, int width, int height)
{
    int right = left + width;
    int bottom = top + height;

    Color::SetCurrent(COLOR_FILL);

    if (top == GRID_TOP)
    {
        Painter::DrawHLine(top, 1, left - 2);
        Painter::DrawHLine(top, right + 2, SCREEN_WIDTH - 2);

        if (!Menu::IsMinimize() || !Menu::IsShown())
        {
            Painter::DrawVLine(1, top + 2, bottom - 2);
            Painter::DrawVLine(318, top + 2, bottom - 2);
        }
    }

    float deltaX = Grid::DeltaX() *(float)width / width;
    float deltaY = Grid::DeltaY() * (float)height / height;
    float stepX = deltaX / 5;
    float stepY = deltaY / 5;
    
    int centerX = left + width / 2;
    int centerY = top + height / 2;

    Color::SetCurrent(ColorGrid());

    if (TYPE_GRID_IS_1)
    {
        DrawGridType1(left, top, right, bottom, (float)centerX, (float)centerY, deltaX, deltaY, stepX, stepY);
    }
    else if (TYPE_GRID_IS_2)
    {
        DrawGridType2(left, top, right, bottom, (int)deltaX, (int)deltaY, (int)stepX, (int)stepY);
    }
    else if (TYPE_GRID_IS_3)
    {
        DrawGridType3(left, top, right, bottom, centerX, centerY, (int)deltaX, (int)deltaY, (int)stepX, (int)stepY);
    }
}



#define  DELTA 5


void Display::DrawScaleLine(int x, bool forTrigLev)
{
    if(ALT_MARKERS_HIDE)
    {
        return;
    }
    int width = 6;
    int topY = GRID_TOP + DELTA;
    int x2 = width + x + 2;
    int bottomY  = Grid::ChannelBottom() - DELTA;
    int centerY = (Grid::ChannelBottom() + GRID_TOP) / 2;
    int levels[] =
    {
        topY,
        bottomY,
        centerY,
        centerY - (bottomY - topY) / (forTrigLev ? 8 : 4),
        centerY + (bottomY - topY) / (forTrigLev ? 8 : 4)
    };
    for(int i = 0; i < 5; i++)
    {
        Painter::DrawLine(x + 1, levels[i], x2 - 1, levels[i], COLOR_FILL);
    }
}


void Display::DrawCursorsWindow()
{
    if((!Menu::IsMinimize() || !Menu::IsShown()) && RShift::drawMarkers)
    {
        DrawScaleLine(2, false);
    }
}


void Display::DrawCursorTrigLevel()
{
    TrigSource::E ch = TRIG_SOURCE;

    if (ch == TrigSource::Ext)
    {
        return;
    }

    int trigLev = TRIG_LEVEL(ch) + (SET_RSHIFT(ch) - RShift::ZERO);

    float scale = 1.0f / ((TrigLev::MAX - TrigLev::MIN) / 2.0f / Grid::ChannelHeight());
    int y0 = (int)((GRID_TOP + Grid::ChannelBottom()) / 2 + scale * (TrigLev::ZERO - TrigLev::MIN));
    int y = (int)(y0 - scale * (trigLev - TrigLev::MIN));

    y = (y - Grid::ChannelCenterHeight()) + Grid::ChannelCenterHeight();

    int x = Grid::Right();
    Color::SetCurrent(ColorTrig());
    if(y > Grid::ChannelBottom())
    {
        PText::DrawChar(x + 3, Grid::ChannelBottom() - 11, SYMBOL_TRIG_LEV_LOWER);
        Painter::SetPoint(x + 5, Grid::ChannelBottom() - 2);
        y = Grid::ChannelBottom() - 7;
        x--;
    }
    else if(y < GRID_TOP)
    {
        PText::DrawChar(x + 3, GRID_TOP + 2, SYMBOL_TRIG_LEV_ABOVE);
        Painter::SetPoint(x + 5, GRID_TOP + 2);
        y = GRID_TOP + 7;
        x--;
    }
    else
    {
        PText::DrawChar(x + 1, y - 4, SYMBOL_TRIG_LEV_NORMAL);
    }
    Font::Set(TypeFont::_5);

    const char simbols[3] = {'1', '2', 'В'};
    int dY = 0;
    
    PText::DrawChar(x + 5, y - 9 + dY, simbols[TRIG_SOURCE], COLOR_BACK);
    Font::Set(TypeFont::_8);

    if (RShift::drawMarkers && !Menu::IsMinimize())
    {
        DrawScaleLine(SCREEN_WIDTH - 11, true);
        int left = Grid::Right() + 9;
        int height = Grid::ChannelHeight() - 2 * DELTA;
        int shiftFullMin = RShift::MIN + TrigLev::MIN;
        int shiftFullMax = RShift::MAX + TrigLev::MAX;
        scale = (float)height / (shiftFullMax - shiftFullMin);
        int shiftFull = TRIG_LEVEL_SOURCE.value + (TRIG_SOURCE_IS_EXT ? 0 : (int16)SET_RSHIFT(ch));
        int yFull =(int)(GRID_TOP + DELTA + height - scale * (shiftFull - RShift::MIN - TrigLev::MIN) - 4);
        Painter::FillRegion(left + 2, yFull + 1, 4, 6, ColorTrig());
        Font::Set(TypeFont::_5);
        PText::DrawChar(left + 3, yFull - 5 + dY, simbols[TRIG_SOURCE], COLOR_BACK);
        Font::Set(TypeFont::_8);
    }
}


void Display::DrawCursorRShift(Chan ch)
{
    float x = (float)(Grid::Right() - Grid::Width() - Measures::GetDeltaGridLeft());

    if (ch == Chan::Math)
    {
        int rShift = SET_RSHIFT_MATH;
        float scale = (float)Grid::MathHeight() / 960;
        float y = (Grid::MathTop() + Grid::MathBottom()) / 2.0f - scale * (rShift - RShift::ZERO);
        PText::DrawChar((int)(x - 9), (int)(y - 4), SYMBOL_RSHIFT_NORMAL, COLOR_FILL);
        PText::DrawChar((int)(x - 8), (int)(y - 5), 'm', COLOR_BACK);
        return;
    }

    if(!ch.Enabled())
    {
        return;
    }

    int rShift = SET_RSHIFT(ch);
 
    float scale = (float)Grid::ChannelHeight() / (RShift::STEP * 200);
    float y = Grid::ChannelCenterHeight() - scale * (rShift - RShift::ZERO);

    float scaleFull = (float)Grid::ChannelHeight() / (RShift::MAX - RShift::MIN) *
        (PageService::Math::Enabled() ? 0.9f : 0.91f);

    float yFull = Grid::ChannelCenterHeight() - scaleFull *(rShift - RShift::ZERO);

    if(y > Grid::ChannelBottom())
    {
        PText::DrawChar((int)(x - 7), Grid::ChannelBottom() - 11, SYMBOL_RSHIFT_LOWER, ColorChannel(ch));
        Painter::SetPoint((int)(x - 5), Grid::ChannelBottom() - 2);
        y = (float)(Grid::ChannelBottom() - 7);
        x++;
    }
    else if(y < GRID_TOP)
    {
        PText::DrawChar((int)(x - 7), GRID_TOP + 2, SYMBOL_RSHIFT_ABOVE, ColorChannel(ch));
        Painter::SetPoint((int)(x - 5), GRID_TOP + 2);
        y = GRID_TOP + 7;
        x++;
    }
    else
    {
        PText::DrawChar((int)(x - 8), (int)(y - 4), SYMBOL_RSHIFT_NORMAL, ColorChannel(ch));
        if(((ch == Chan::A) ? (RShift::showLevelA == true) : (RShift::showLevelB == true)) && MODE_WORK_IS_DIRECT)
        {
            Painter::DrawDashedHLine((int)y, Grid::Left(), Grid::Right(), 7, 3, 0);
        }
    }

    Font::Set(TypeFont::_5);
    int dY = 0;

    if((!Menu::IsMinimize() || !Menu::IsShown()) && RShift::drawMarkers)
    {
        Painter::FillRegion(4, (int)(yFull - 3), 4, 6, ColorChannel(ch));
        PText::DrawChar(5, (int)(yFull - 9 + dY), ch == Chan::A ? '1' : '2', COLOR_BACK);
    }
    PText::DrawChar((int)(x - 7), (int)(y - 9 + dY), ch == Chan::A ? '1' : '2', COLOR_BACK);
    Font::Set(TypeFont::_8);
}


void TShift::Draw()
{
    BitSet32 points = SettingsDisplay::PointsOnDisplay();
    int firstPoint = points.half_iword[0];
    int lastPoint = points.half_iword[1];

    // Рисуем TPos
    int shiftTPos = TPos::InPoints(Data::dir.ds.e_points_in_channel, SET_TPOS) - SHIFT_IN_MEMORY;
    float scale = (float)(lastPoint - firstPoint) / Grid::Width();
    int gridLeft = Grid::Left();
    int x = (int)(gridLeft + shiftTPos * scale - 3);

    if (IntInRange(x + 3, gridLeft, Grid::Right() + 1))
    {
        PText::Draw2Symbols(x, GRID_TOP - 1, SYMBOL_TPOS_2, SYMBOL_TPOS_3, COLOR_BACK, COLOR_FILL);
    };

    // Рисуем tShift
    int shiftTShift = TPos::InPoints(Data::dir.ds.e_points_in_channel, SET_TPOS) - SET_TSHIFT * 2;

    if(IntInRange(shiftTShift, firstPoint, lastPoint))
    {
        x = gridLeft + shiftTShift - firstPoint - 3;
        PText::Draw2Symbols(x, GRID_TOP - 1, SYMBOL_TSHIFT_NORM_1, SYMBOL_TSHIFT_NORM_2, COLOR_BACK, COLOR_FILL);
    }
    else if(shiftTShift < firstPoint)
    {
        PText::Draw2Symbols(gridLeft + 1, GRID_TOP, SYMBOL_TSHIFT_LEFT_1, SYMBOL_TSHIFT_LEFT_2, COLOR_BACK, COLOR_FILL);
        Painter::DrawLine(Grid::Left() + 9, GRID_TOP + 1, Grid::Left() + 9, GRID_TOP + 7, COLOR_BACK);
    }
    else if(shiftTShift > lastPoint)
    {
        PText::Draw2Symbols(Grid::Right() - 8, GRID_TOP, SYMBOL_TSHIFT_RIGHT_1, SYMBOL_TSHIFT_RIGHT_2, COLOR_BACK, COLOR_FILL);
        Painter::DrawLine(Grid::Right() - 9, GRID_TOP + 1, Grid::Right() - 9, GRID_TOP + 7, COLOR_BACK);
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

        CursCntrl cntrl = CURS_CNTRL_T(source);
        if (cntrl != CursCntrl_Disable)
        {
            DrawVerticalCursor((int)CURS_POS_T0(source), y0);
            DrawVerticalCursor((int)CURS_POS_T1(source), y1);
        }
        cntrl = CURsU_CNTRL;
        if (cntrl != CursCntrl_Disable)
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
        DataSettings &ds = MODE_WORK_IS_DIRECT ? Data::dir.ds : Data::ins.ds;

        if (ds.Valid())
        {
            inverse = ch.IsA() ? ds.inv_a : ds.inv_b;
            modeCouple = ch.IsA() ? ds.coupleA : ds.coupleB;
            multiplier = ch.IsA() ? ds.div_a : ds.div_b;
            range = ds.range[ch];
            rShift.value = (int16)(ch.IsA() ? ds.rShiftA : ds.rShiftB);
            enable = ch.IsA() ? ds.en_a : ds.en_b;
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
        DataSettings &ds = MODE_WORK_IS_LATEST ? Data::last.ds : Data::ins.ds;

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

    if (VCP::clientIsConnected || VCP::cableIsConnected)
    {
        PText::Draw4SymbolsInRect(x + 72, GRID_BOTTOM + 2, SYMBOL_USB, VCP::clientIsConnected ? COLOR_FILL : Color::FLASH_01);
    }
    
    Color::SetCurrent(COLOR_FILL);

    if(!SET_PEAKDET_IS_DISABLE)
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
    numMS += timeTicks / 120000.0f;
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
    std::sprintf(message, "%d", Storage::NumElementsWithSameSettings());
    std::strcat(message, "/");
    char numAvail[10] = {0};
    std::sprintf(numAvail, "%d", Storage::NumberAvailableEntries());
    std::strcat(message, numAvail);
    PText::Draw(Grid::Left() + 50, Grid::FullBottom() - 9, message);
}


void Display::DisableShowLevelRShiftA()
{
    RShift::showLevelA = false;
    Timer::Disable(TypeTimer::ShowLevelRShift0);
}


void Display::DisableShowLevelRShiftB()
{
    RShift::showLevelB = false;
    Timer::Disable(TypeTimer::ShowLevelRShift1);
}


void Display::DisableShowLevelTrigLev()
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


void Display::ChangedRShiftMarkers()
{
    RShift::drawMarkers = !ALT_MARKERS_HIDE;
    Timer::Enable(TypeTimer::RShiftMarkersAutoHide, 5000, FuncOnTimerRShiftMarkersAutoHide);
}

void Display::FuncOnTimerRShiftMarkersAutoHide()
{
    Display::OnRShiftMarkersAutoHide();
}


void Display::OnRShiftMarkersAutoHide()
{
    RShift::drawMarkers = false;
    Timer::Disable(TypeTimer::RShiftMarkersAutoHide);
}


int Display::FirstEmptyString()
{
    for(int i = 0; i < MAX_NUM_STRINGS; i++)
    {
        if(strings[i] == 0)
        {
            return i;
        }
    }
    return MAX_NUM_STRINGS;
}


int Display::CalculateFreeSize()
{
    int firstEmptyString = FirstEmptyString();
    if(firstEmptyString == 0)
    {
        return SIZE_BUFFER_FOR_STRINGS;
    }
    return (int)(SIZE_BUFFER_FOR_STRINGS - (strings[firstEmptyString - 1] - bufferForStrings) - std::strlen(strings[firstEmptyString - 1]) - 1);
}


void Display::DeleteFirstString()
{
    if(FirstEmptyString() < 2)
    {
        return;
    }
    int delta = (int)std::strlen(strings[0]) + 1;
    int numStrings = FirstEmptyString();
    for(int i = 1; i < numStrings; i++)
    {
        strings[i - 1] = strings[i] - delta;
    }
    for(int i = numStrings - 1; i < MAX_NUM_STRINGS; i++)
    {
        strings[i] = 0; //-V557
    }
    for(int i = 0; i < SIZE_BUFFER_FOR_STRINGS - delta; i++)
    {
        bufferForStrings[i] = bufferForStrings[i + delta];
    }
}



void Display::AddString(pchar string)
{
    static int num = 0;
    char buffer[100];
    std::sprintf(buffer, "%d\x11", num++);
    std::strcat(buffer, string);
    int size = (int)std::strlen(buffer) + 1;

    while(CalculateFreeSize() < size)
    {
        DeleteFirstString();
    }

    if(!strings[0])
    {
        strings[0] = bufferForStrings;
        std::strcpy(strings[0], buffer);
    }
    else
    {
        char *addressLastString = strings[FirstEmptyString() - 1];
        char *address = addressLastString + std::strlen(addressLastString) + 1;
        strings[FirstEmptyString()] = address; //-V557
        std::strcpy(address, buffer);
    }
}



void Display::AddStringToIndicating(pchar string)
{
    if(FirstEmptyString() == MAX_NUM_STRINGS)
    {
        DeleteFirstString();
    }

    AddString(string);
}



void Display::DrawConsole()
{
    Font::Set(Font::GetSizeFontForConsole() == 5 ? TypeFont::_5 : TypeFont::_8);

    int height = Font::GetSize();
    int lastString = FirstEmptyString() - 1;
    int numStr = NUM_STRINGS;

    if(height == 8 && numStr > 22)
    {
        numStr = 22;
    }

    if(SHOW_STRING_NAVIGATION)
    {
        numStr -= ((height == 8) ? 1 : 2);
    }

    int firstString = lastString - numStr + 1;

    if(firstString < 0)
    {
        firstString = 0;
    }

    int y = GRID_TOP + 1;

    for(int numString = firstString; numString <= lastString; numString++)
    {
        int width = Font::GetLengthText(strings[numString]);

        Painter::FillRegion(Grid::Left() + 1, y, width, 5, COLOR_BACK);

        PText::Draw(Grid::Left() + 2, y - 10, strings[numString], COLOR_FILL);

        y += height + 1;
    }

    Font::Set(TypeFont::_8);
}


void Display::ShowWarn(pchar message)
{
    if (warnings[0] == 0)
    {
        Timer::Enable(TypeTimer::ShowMessages, 100, OnTimerShowWarning);
    }
    bool alreadyStored = false;
    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (warnings[i] == 0 && !alreadyStored)
        {
            warnings[i] = message;
            timeWarnings[i] = TIME_MS;
            alreadyStored = true;
        }
        else if (warnings[i] == message)
        {
            timeWarnings[i] = TIME_MS;
            return;
        }
    }
}


void Display::OnTimerShowWarning()
{
    uint time = TIME_MS;
    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (time - timeWarnings[i] > (uint)TIME_MESSAGES * 1000)
        {
            timeWarnings[i] = 0;
            warnings[i] = 0;
        }
    }

    int pointer = 0;
    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (warnings[i] != 0)
        {
            warnings[pointer] = warnings[i];
            timeWarnings[pointer] = timeWarnings[i];
            if (pointer != i)
            {
                timeWarnings[i] = 0;
                warnings[i] = 0;
            }
            pointer++;
        }
    }

    if (pointer == 0)
    {
        Timer::Disable(TypeTimer::ShowMessages);
    }
}


void Display::ClearFromWarnings()
{
    Timer::Disable(TypeTimer::ShowMessages);
    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        warnings[i] = 0;
        timeWarnings[i] = 0;
    }
}



void Display::ShowWarningBad(Warning::E warning)
{
    Color::ResetFlash();
    ShowWarn(Tables::GetWarning(warning));
    Sound::WarnBeepBad();
}



void Display::ShowWarningGood(Warning::E warning)
{
    Color::ResetFlash();
    ShowWarn(Tables::GetWarning(warning));
    Sound::WarnBeepGood();
}


void Display::DrawStringInRectangle(int, int y, char const *text)
{
    int width = Font::GetLengthText(text);
    int height = 8;
    Painter::DrawRectangle(Grid::Left(), y, width + 4, height + 4, COLOR_FILL);
    Painter::DrawRectangle(Grid::Left() + 1, y + 1, width + 2, height + 2, COLOR_BACK);
    Painter::FillRegion(Grid::Left() + 2, y + 2, width, height, Color::FLASH_10);
    PText::Draw(Grid::Left() + 3, y + 2, text, Color::FLASH_01);
}


void Display::DrawWarnings()
{
    int delta = 12;
    int y = Grid::BottomMessages();
    for(int i = 0; i < 10; i++)
    {
        if(warnings[i] != 0)
        {
            DrawStringInRectangle(Grid::Left(), y, warnings[i]);
            y -= delta;
        }
    }
}


void Display::RunAfterDraw(pFuncVV func)
{
    funcAfterDraw = func;
}
