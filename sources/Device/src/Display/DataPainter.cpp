// 2022/03/16 10:18:22 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/DataPainter.h"
#include "Data/Storage.h"
#include "Menu/Pages/Definition.h"
#include "Display/Grid.h"
#include "Data/Processing.h"
#include "Utils/Math.h"
#include "Display/Symbols.h"
#include "Data/DataExtensions.h"
#include "Hardware/InterCom.h"
#include <climits>


#define CONVERT_DATA_TO_DISPLAY(out, in)                        \
    out = (uint8)(maxY - ((in) - ValueFPGA::MIN) * scaleY);     \
    if(out < minY)          { out = (uint8)minY; }              \
    else if (out > maxY)    { out = (uint8)maxY; };


namespace DataPainter
{
    // Нарисовать данные в обычном режиме работы
    void DrawInModeDirect();

    // Нарисовать данные в режиме ПАМЯТЬ-Последние
    void DrawInModeLatest();

    // Нарисовать данные режиме ПАМЯТЬ-ВНУТР ЗУ
    void DrawInModeInternal();

    void DrawDataNormal();

    void DrawDataInModeWorkLatestMemInt();

    void DrawDataMinMax();

    void DrawDataChannel(DataStruct &, Chan, int minY, int maxY);

    void DrawMarkersForMeasure(float scale, Chan);

    void DrawSignalLined(const uint8 *data, const DataSettings &, int startPoint, int endPoint, int minY, int maxY,
        float scaleY, float scaleX);

    void DrawSignalPointed(const uint8 *data, const DataSettings &, int startPoint, int endPoint, int minY, int maxY,
        float scaleY, float scaleX);

    void DrawBothChannels(DataStruct &);

    // modeLines - true - точками, false - точками
    void DrawSignal(const int x, const uint8 data[281], bool modeLines);

    namespace MemoryWindow
    {
        // shiftForPeakDet - если рисуем информацию с пикового детектора - то через shiftForPeakDet точек расположена
        // иниформация о максимумах.
        // указатель data передаётся на первую точку, startI, endI измеряются в точках для пикового детектора и нет
        void DrawChannel(int timeWindowRectWidth, int xVert0, int xVert1, int startI, int endI,
            const uint8 *data, int rightX, Chan, DataSettings &);

        // shiftForPeakDet - если рисуем информацию с пикового детектора - то через shiftForPeakDet точек расположена
        // иниформация о максимумах.
        void DrawDataInRect(int x, int width, const uint8 *data, int numElems, Chan ch, DataSettings &);
    }
}


void DataPainter::DrawData()
{
    static const pFuncVV functions[ModeWork::Count] =
    {
        DrawInModeDirect,
        DrawInModeLatest,
        DrawInModeInternal
    };

    functions[MODE_WORK]();

    Painter::DrawRectangle(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight(), COLOR_FILL);
}


void DataPainter::DrawInModeDirect()
{
    DataFrame frame;
    Processing::out.ds.valid = 0;

    Processing::SetDataForProcessing(false);

    if (TBase::InModeP2P())
    {
        int x = Processing::out.PrepareForNormalDrawP2P();

        DrawDataNormal();

        if (x != 0)
        {
            Painter::DrawVLine(Grid::Left() + x, GRID_TOP, Grid::ChannelBottom(), Color::GRID);
        }
    }
    else
    {
        DrawDataNormal();
    }
}


void DataPainter::DrawInModeLatest()
{

}


void DataPainter::DrawInModeInternal()
{

}


void DataPainter::DrawDataInModeWorkLatestMemInt()
{
    if (Processing::out.Valid())
    {
        DrawDataChannel(Processing::out, Chan::A, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Processing::out, Chan::B, GRID_TOP, Grid::ChannelBottom());
    }
}


void DataPainter::DrawDataChannel(DataStruct &data, Chan ch, int minY, int maxY)
{
    if (!SET_ENABLED(ch))
    {
        return;
    }

    float scaleY = (float)(maxY - minY) / (ValueFPGA::MAX - ValueFPGA::MIN);
    float scaleX = (float)Grid::Width() / 280.0f;

    if (SHOW_MEASURES)
    {
        DrawMarkersForMeasure(scaleY, ch);
    }

    BitSet32 points = SettingsDisplay::PointsOnDisplay();
    int first = points.half_iword[0];
    int last = points.half_iword[1];

    DataSettings ds = data.ds;

    Color::SetCurrent(ColorChannel(ch));

    if (MODE_DRAW_IS_SIGNAL_LINES)
    {
        DrawSignalLined(data.Data(ch).Data(), ds, first, last, minY, maxY, scaleY, scaleX);
    }
    else
    {
        DrawSignalPointed(data.Data(ch).Data(), ds, first, last, minY, maxY, scaleY, scaleX);
    }
}


void DataPainter::DrawMarkersForMeasure(float scale, Chan ch)
{
    if (ch == Chan::Math)
    {
        return;
    }

    Color::SetCurrent(ColorCursors(ch));

    for (int numMarker = 0; numMarker < 2; numMarker++)
    {
        int pos = Processing::GetMarkerHorizontal(ch, numMarker);

        if (pos != ERROR_VALUE_INT && pos > 0 && pos < 200)
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


void DataPainter::DrawSignalLined(const uint8 *in, const DataSettings &ds, int start, int end, int minY,
    int maxY, float scaleY, float scaleX)
{
    if (end < start)
    {
        return;
    }

    uint8 out[281];

    int gridLeft = Grid::Left();
    int gridRight = Grid::Right();

    if (ds.peak_det == 0)
    {
        for (int i = start; i < end; i++)
        {
            float x0 = gridLeft + (i - start) * scaleX;
            if (x0 >= gridLeft && x0 <= gridRight)
            {
                int index = i - start;
                CONVERT_DATA_TO_DISPLAY(out[index], in[i]);
            }
        }

        if (end - start < 281)
        {
            int _numPoints = 281 - (end - start);
            for (int i = 0; i < _numPoints; i++)
            {
                int index = end - start + i;
                CONVERT_DATA_TO_DISPLAY(out[index], ValueFPGA::MIN);
            }
        }

        CONVERT_DATA_TO_DISPLAY(out[280], in[end]);
        DrawSignal(Grid::Left(), out, true);
    }
    else
    {
        start *= 2;
        end *= 2;

        int yMinNext = -1;
        int yMaxNext = -1;

        for (int i = start; i < end; i += 2)
        {
            float x = gridLeft + (i - start) / 2.0f * scaleX;

            if (x >= gridLeft && x <= gridRight)
            {
                int yMin = yMinNext;

                if (yMin == -1)
                {
                    CONVERT_DATA_TO_DISPLAY(yMin, in[i + 1]);
                }

                int yMax = yMaxNext;

                if (yMax == -1)
                {
                    CONVERT_DATA_TO_DISPLAY(yMax, in[i]);
                }

                CONVERT_DATA_TO_DISPLAY(yMaxNext, in[i + 1]);

                if (yMaxNext < yMin)
                {
                    yMin = yMaxNext + 1;
                }

                CONVERT_DATA_TO_DISPLAY(yMinNext, in[i + 2]);

                if (yMinNext > yMax)
                {
                    yMax = yMinNext - 1;
                }

                Painter::DrawVLine((int)x, yMin, yMax);
            }
        }
    }
}


void DataPainter::DrawSignalPointed(const uint8 *in, const DataSettings &ds, int start, int end, int minY,
    int maxY, float scaleY, float scaleX)
{
    uint8 out[281];

    if (scaleX == 1.0f)
    {
        if (ds.peak_det == 0)
        {
            for (int i = start; i < end; i++)
            {
                int index = i - start;
                CONVERT_DATA_TO_DISPLAY(out[index], in[i]);
            }

            DrawSignal(Grid::Left(), out, false);
        }
        else
        {
            start *= 2;
            end *= 2;

            for (int i = start; i < end; i += 2)
            {
                int index = (i - start) / 2;
                CONVERT_DATA_TO_DISPLAY(out[index], in[i]);
            }

            DrawSignal(Grid::Left(), out, false);

            for (int i = start + 1; i < end + 1; i += 2)
            {
                int index = (i - start - 1) / 2;
                CONVERT_DATA_TO_DISPLAY(out[index], in[i]);
            }

            DrawSignal(Grid::Left(), out, false);
        }
    }
    else
    {
        for (int i = start; i < end; i++)
        {
            int index = i - start;
            int dat = 0;
            CONVERT_DATA_TO_DISPLAY(dat, in[i]);
            Painter::SetPoint((int)(Grid::Left() + index * scaleX), dat);
        }
    }
}


void DataPainter::DrawMath()
{
    if (DISABLED_DRAW_MATH || Storage::GetData(Chan::A, 0) == 0 || Storage::GetData(Chan::B, 0) == 0)
    {
        return;
    }

    float dataAbs0[FPGA::MAX_POINTS * 2];
    float dataAbs1[FPGA::MAX_POINTS * 2];

    BufferFPGA   &dataA = Processing::out.A;
    BufferFPGA   &dataB = Processing::out.B;
    DataSettings &ds = Processing::out.ds;

    ValueFPGA::ToVoltage(dataA.Data(), ds.BytesInChannel(), ds.range[Chan::A], (int16)ds.rShiftA, dataAbs0);
    ValueFPGA::ToVoltage(dataB.Data(), ds.BytesInChannel(), ds.range[Chan::B], (int16)ds.rShiftB, dataAbs1);

    Math::CalculateMathFunction(dataAbs0, dataAbs1, ds.BytesInChannel());

    DataStruct data;
    data.ds.Set(ds);
    data.Data(ChA).Realloc(ds.BytesInChannel());

    ValueFPGA::FromVoltage(dataAbs0, ds.BytesInChannel(), SET_RANGE_MATH, SET_RSHIFT_MATH, data.Data(ChA).Data());

    DrawDataChannel(data, Chan::Math, Grid::MathTop(), Grid::MathBottom());

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


void DataPainter::DrawBothChannels(DataStruct &data)
{
    if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawDataChannel(data, Chan::A, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(data, Chan::B, GRID_TOP, Grid::ChannelBottom());
    }
    else
    {
        DrawDataChannel(data, Chan::B, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(data, Chan::A, GRID_TOP, Grid::ChannelBottom());
    }
}


void DataPainter::DrawDataMinMax()
{
    ModeDrawSignal::E modeDrawSignalOld = MODE_DRAW_SIGNAL;
    MODE_DRAW_SIGNAL = ModeDrawSignal::Lines;

    DataStruct limit;

    if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawDataChannel(Limitator::GetLimitation(ChA, 0, limit), ChA, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Limitator::GetLimitation(ChA, 1, limit), ChA, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Limitator::GetLimitation(ChB, 0, limit), ChB, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Limitator::GetLimitation(ChB, 1, limit), ChB, GRID_TOP, Grid::ChannelBottom());
    }
    else
    {
        DrawDataChannel(Limitator::GetLimitation(ChB, 0, limit), ChB, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Limitator::GetLimitation(ChB, 1, limit), ChB, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Limitator::GetLimitation(ChA, 0, limit), ChA, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Limitator::GetLimitation(ChA, 1, limit), ChA, GRID_TOP, Grid::ChannelBottom());
    }

    MODE_DRAW_SIGNAL = modeDrawSignalOld;
}


void DataPainter::DrawDataNormal()
{
    if (Storage::NumFrames() == 0)
    {
        return;
    }

    int16 numSignals = (int16)Storage::NumFramesWithSameSettings();
    LIMITATION(numSignals, numSignals, 1, NUM_ACCUM);

    if (numSignals == 1 || ENUM_ACCUM_IS_INFINITY || MODE_ACCUM_IS_RESET || TBase::InModeRandomizer())
    {
        DrawBothChannels(Processing::out);

        Display::numDrawingSignals++;
    }
    else
    {
        for (int i = 0; i < numSignals; i++)
        {
            Processing::SetData(Storage::GetData(i));

            DrawBothChannels(Processing::out);
        }
    }
}


void DataPainter::MemoryWindow::Draw()
{
    DEBUG_POINT_0;
    Processing::SetDataForProcessing(true);
    DEBUG_POINT_0;

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
        DataStruct *dat = &Processing::out;

        if (dat->A.Size() || dat->B.Size())
        {
            Chan::E chanFirst = LAST_AFFECTED_CHANNEL_IS_A ? ChB : ChA;
            Chan::E chanSecond = LAST_AFFECTED_CHANNEL_IS_A ? ChA : ChB;

            const uint8 *dataFirst = LAST_AFFECTED_CHANNEL_IS_A ? dat->B.Data() : dat->A.Data();
            const uint8 *dataSecond = LAST_AFFECTED_CHANNEL_IS_A ? dat->A.Data() : dat->B.Data();

            if (SET_ENABLED(chanFirst))
            {
                DrawChannel(timeWindowRectWidth, xVert0, xVert1, startI, endI, dataFirst, rightX,
                    chanFirst, dat->ds);
            }

            if (SET_ENABLED(chanSecond))
            {
                DrawChannel(timeWindowRectWidth, xVert0, xVert1, startI, endI, dataSecond, rightX,
                    chanSecond, dat->ds);
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

    float xShift = 1 + (TPos::InPoints(SET_ENUM_POINTS, SET_TPOS) - SET_TSHIFT * 2) * scale;

    if (xShift < leftX - 2)
    {
        xShift = (float)(leftX - 2);
    }

    Painter::FillRegion((int)(xShift - 1), 3, 6, 6, COLOR_BACK);
    Painter::FillRegion((int)(xShift), 4, 4, 4, COLOR_FILL);
    Color::SetCurrent(COLOR_BACK);

    if (xShift == leftX - 2)
    {
        xShift = (float)(leftX - 2);
        Painter::DrawLine((int)(xShift + 3), 5, (int)(xShift + 3), 7);
        Painter::DrawLine((int)(xShift + 1), 6, (int)(xShift + 2), 6);
    }
    else if (xShift > rightX - 1)
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


void DataPainter::MemoryWindow::DrawChannel(int timeWindowRectWidth, int xVert0, int xVert1, int startI,
    int endI, const uint8 *data, int rightX, Chan ch, DataSettings &ds)
{
    int k = ds.peak_det ? 2 : 1;

    DrawDataInRect(1, xVert0 - 1, &(data[0 * k]), startI, ch, ds);

    DrawDataInRect(xVert0 + 2, timeWindowRectWidth - 2, &(data[startI * k]), 281, ch, ds);

    DrawDataInRect(xVert1 + 2, rightX - xVert1 + 2, &(data[endI * k + 1]), ds.PointsInChannel() - endI, ch, ds);
}


void DataPainter::MemoryWindow::DrawDataInRect(int x, int width, const uint8 *in, int numElems, Chan ch,
    DataSettings &ds)
{
    if (numElems == 0)
    {
        return;
    }

    if (ds.peak_det)
    {
        numElems *= 2;
    }

    width--;
    float elemsInColumn = (float)numElems / (float)width;

    uint8 min[300];
    uint8 max[300];

    std::memset(min, 255, 300);
    std::memset(max, 0, 300);

    if (ds.peak_det == 0)
    {
        uint8 *iMin = &min[0];
        uint8 *iMax = &max[0];

        for (int col = 0; col < width; col++, iMin++, iMax++)
        {
            int firstElem = (int)(col * elemsInColumn);
            int lastElem = (int)(firstElem + elemsInColumn - 1);

            *iMin = in[firstElem];
            *iMax = in[firstElem];

            for (int elem = firstElem + 1; elem <= lastElem; elem++)
            {
                SET_MIN_IF_LESS(in[elem], *iMin);
                SET_MAX_IF_LARGER(in[elem], *iMax);
            }
        }
    }
    else
    {
        for (int col = 0; col < width; col++)
        {
            float first = col * elemsInColumn;
            float last = first + elemsInColumn - 1;

            max[col] = in[(int)first];
            min[col] = in[(int)first + 1];

            for (int elem = (int)(first + 2); elem < last; elem += 2)
            {
                SET_MAX_IF_LARGER(in[elem], max[col]);
                SET_MIN_IF_LESS(in[elem + 1], max[col]);
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


void DataPainter::DrawSignal(const int _x, const uint8 data[281], bool modeLines)
{
    int x = _x;

    if (modeLines)
    {
        int y_prev = data[0];

        for (int i = 1; i < 280; i++)
        {
            int y = data[i];

            if (y == y_prev)
            {
                Painter::SetPoint(x, y);
            }
            else if (y > y_prev)
            {
                Painter::DrawVLine(x, y - 1, y_prev);
            }
            else
            {
                Painter::DrawVLine(x, y_prev, y + 1);
            }

            y_prev = y;
            x++;
        }
    }
    else
    {
        for (int i = 0; i < 281; i++)
        {
            Painter::SetPoint(x++, data[i]);
        }
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(284, (uint8)(modeLines ? DRAW_SIGNAL_LINES : DRAW_SIGNAL_POINTS));
        command.PushHalfWord(_x);

        for (int i = 0; i < 281; i++)
        {
            command.PushByte(data[i]);
        }

        command.Transmit(284);
    }
}
