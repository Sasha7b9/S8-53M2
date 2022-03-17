// 2022/03/16 10:18:22 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/DataPainter.h"
#include "Data/Storage.h"
#include "Menu/Pages/Definition.h"
#include "Display/Grid.h"
#include "Data/Processing.h"
#include "Utils/Math.h"
#include "Display/Symbols.h"
#include <climits>


#define CONVERT_DATA_TO_DISPLAY(out, in)                        \
    out = (uint8)(maxY - ((in) - ValueFPGA::MIN) * scaleY);     \
    if(out < minY)          { out = (uint8)minY; }              \
    else if (out > maxY)    { out = (uint8)maxY; };


namespace DataPainter
{
    void DrawDataMemInt();

    void DrawDataNormal();

    void DrawDataInModeWorkLatest();

    void DrawDataMinMax();

    void DrawDataChannel(DataStruct &, Chan, int minY, int maxY);

    void DrawMarkersForMeasure(float scale, Chan);

    void DrawSignalLined(const uint8 *data, const DataSettings *ds, int startPoint, int endPoint, int minY, int maxY,
        float scaleY, float scaleX, bool calculateFiltr);

    void DrawSignalPointed(const uint8 *data, const DataSettings *ds, int startPoint, int endPoint, int minY, int maxY,
        float scaleY, float scaleX);

    void DrawBothChannels(DataStruct &);

    // shiftForPeakDet - если рисуем информацию с пикового детектора - то через shiftForPeakDet точек расположена
    // иниформация о максимумах.
    void DrawChannelInWindowMemory(int timeWindowRectWidth, int xVert0, int xVert1, int startI, int endI,
        const uint8 *data, int rightX, Chan::E ch, int shiftForPeakDet);

    // shiftForPeakDet - если рисуем информацию с пикового детектора - то через shiftForPeakDet точек расположена
    // иниформация о максимумах.
    void DrawDataInRect(int x, int width, const uint8 *data, int numElems, Chan::E ch, int shiftForPeakDet);
}


void DataPainter::DrawData()
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


void DataPainter::DrawDataMemInt()
{
    if (Data::ins.Valid())
    {
        DrawDataChannel(Data::ins, ChA, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Data::ins, ChB, GRID_TOP, Grid::ChannelBottom());
    }
}


// Если data == 0, то данные брать из GetData
void DataPainter::DrawDataChannel(DataStruct &data, Chan ch, int minY, int maxY)
{
    bool calculateFiltr = true;

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

    if (MODE_DRAW_IS_SIGNAL_LINES)
    {
        DrawSignalLined(data.Data(ch).Data(), &ds, first, last, minY, maxY, scaleY, scaleX, calculateFiltr);
    }
    else
    {
        DrawSignalPointed(data.Data(ch).Data(), &ds, first, last, minY, maxY, scaleY, scaleX);
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


void DataPainter::DrawSignalLined(const uint8 *data, const DataSettings *ds, int startPoint, int endPoint, int minY,
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

    if (endPoint - startPoint < 281)
    {
        int _numPoints = 281 - (endPoint - startPoint);
        for (int i = 0; i < _numPoints; i++)
        {
            int index = endPoint - startPoint + i;
            CONVERT_DATA_TO_DISPLAY(dataCD[index], ValueFPGA::MIN);
        }
    }

    if (ds->peakDet == PeackDetMode::Disable)
    {
        CONVERT_DATA_TO_DISPLAY(dataCD[280], data[endPoint]);
        Painter::DrawSignal(Grid::Left(), dataCD, true);
    }
}


void DataPainter::DrawSignalPointed(const uint8 *data, const DataSettings *ds, int startPoint, int endPoint, int minY,
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

    Math_CalculateMathFunction(dataAbs0, dataAbs1, ds.BytesInChannel());

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


void DataPainter::DrawDataInModeWorkLatest()
{
    if (Data::last.Valid())
    {
        DrawDataChannel(Data::last, Chan::A, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Data::last, Chan::B, GRID_TOP, Grid::ChannelBottom());
    }
}


void DataPainter::DrawDataMinMax()
{
    ModeDrawSignal modeDrawSignalOld = MODE_DRAW_SIGNAL;
    MODE_DRAW_SIGNAL = ModeDrawSignal_Lines;

//    if (LAST_AFFECTED_CHANNEL_IS_B)
//    {
//        DrawDataChannel(Storage::GetLimitation(Chan::A, 0), Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
//        DrawDataChannel(Storage::GetLimitation(Chan::A, 1), Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
//        DrawDataChannel(Storage::GetLimitation(Chan::B, 0), Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
//        DrawDataChannel(Storage::GetLimitation(Chan::B, 1), Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
//    }
//    else
//    {
//        DrawDataChannel(Storage::GetLimitation(Chan::B, 0), Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
//        DrawDataChannel(Storage::GetLimitation(Chan::B, 1), Chan::B, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
//        DrawDataChannel(Storage::GetLimitation(Chan::A, 0), Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
//        DrawDataChannel(Storage::GetLimitation(Chan::A, 1), Chan::A, Data::dir.ds, GRID_TOP, Grid::ChannelBottom());
//    }

    MODE_DRAW_SIGNAL = modeDrawSignalOld;
}


void DataPainter::DrawDataNormal()
{
    static void *prevAddr = 0;

    int16 numSignals = (int16)Storage::NumElementsWithSameSettings();
    LIMITATION(numSignals, numSignals, 1, NUM_ACCUM);

    if (numSignals == 1 || ENUM_ACCUM_IS_INFINITY || MODE_ACCUM_IS_RESET || TBase::InModeRandomizer())
    {
        DrawBothChannels(Data::dir);

        if (prevAddr == 0 || prevAddr != Processing::out.ds.prev)
        {
            Display::numDrawingSignals++;
            prevAddr = Processing::out.ds.prev;
        }
    }
    else
    {
        for (int i = 0; i < numSignals; i++)
        {
            DataStruct data;
            Storage::GetData(i, data);

            DrawBothChannels(data);
        }
    }
}


void DataPainter::DrawMemoryWindow()
{
    DataStruct *dat = &Data::ins;

    if (MODE_WORK_IS_DIRECT || MODE_WORK_IS_LATEST)
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
            Chan::E chanFirst = LAST_AFFECTED_CHANNEL_IS_A ? ChB : ChA;
            Chan::E chanSecond = LAST_AFFECTED_CHANNEL_IS_A ? ChA : ChB;

            const uint8 *dataFirst = LAST_AFFECTED_CHANNEL_IS_A ? dat->B.Data() : dat->A.Data();
            const uint8 *dataSecond = LAST_AFFECTED_CHANNEL_IS_A ? dat->A.Data() : dat->B.Data();

            int shiftForPeakDet = (dat->ds.peakDet == PeackDetMode::Disable) ? 0 : dat->ds.PointsInChannel();

            if (SET_ENABLED(chanFirst))
            {
                DrawChannelInWindowMemory(timeWindowRectWidth, xVert0, xVert1, startI, endI, dataFirst, rightX,
                    chanFirst, shiftForPeakDet);
            }

            if (SET_ENABLED(chanSecond))
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


void DataPainter::DrawChannelInWindowMemory(int timeWindowRectWidth, int xVert0, int xVert1, int startI, int endI,
    const uint8 *data, int rightX, Chan::E ch, int shiftForPeakDet)
{
    DrawDataInRect(1, xVert0 - 1, &(data[0]), startI, ch, shiftForPeakDet);

    DrawDataInRect(xVert0 + 2, timeWindowRectWidth - 2, &(data[startI]), 281, ch, shiftForPeakDet);
    DrawDataInRect(xVert1 + 2, rightX - xVert1 + 2, &(data[endI + 1]), ENUM_POINTS_FPGA::ToNumPoints() - endI, ch,
        shiftForPeakDet);
}


void DataPainter::DrawDataInRect(int x, int width, const uint8 *data, int numElems, Chan::E ch, int shiftForPeakDet)
{
    if (numElems == 0)
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
