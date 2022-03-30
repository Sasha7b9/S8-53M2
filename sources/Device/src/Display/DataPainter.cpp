// 2022/03/16 10:18:22 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/DataPainter.h"
#include "Data/Storage.h"
#include "Menu/Pages/Definition.h"
#include "Display/Screen/Grid.h"
#include "Data/Processing.h"
#include "Utils/Math.h"
#include "Display/Symbols.h"
#include "Data/DataExtensions.h"
#include "Hardware/InterCom.h"
#include "Menu/Menu.h"
#include "Utils/Strings.h"
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

    void DrawDataInternal();

    void DrawDataInModeWorkLatestMemInt();

    void DrawDataChannel(DataStruct &, Chan, int minY, int maxY);

    void DrawMarkersForMeasure(float scaleY, Chan);

    void DrawSignalLined(const uint8 *data, const DataSettings &, int startPoint, int endPoint, int minY, int maxY,
        float scaleY, float scaleX);

    void DrawSignalPointed(const uint8 *data, const DataSettings &, int startPoint, int endPoint, int minY, int maxY,
        float scaleY, float scaleX);

    // Если (min_max == true), то рисуем draw_min_max
    void DrawBothChannels(DataStruct &);

    // Нарисовать min/max
    void DrawMinMax();

    void DrawMinMax(Chan ch);

    // modeLines - true - точками, false - точками
    void DrawSignal(const int x, const uint8 data[281], bool modeLines);

    namespace Spectrum
    {
        void DRAW_SPECTRUM(const uint8 *data, int numPoints, Chan);

        void DrawChannel(const float *spectrum, Color::E);

        void WriteParameters(Chan, float freq0, float density0, float freq1, float density1);
    }

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

    Rectangle(Grid::Width(), Grid::FullHeight()).Draw(Grid::Left(), GRID_TOP, COLOR_FILL);
}


void DataPainter::DrawInModeDirect()
{
    Processing::SetDataForProcessing(ModeWork::Direct, false);

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
    Processing::SetDataForProcessing(ModeWork::Latest, false);

    DrawDataNormal();
}


void DataPainter::DrawInModeInternal()
{
    if (MODE_SHOW_MEMINT_IS_DIRECT || MODE_SHOW_MEMINT_IS_BOTH)
    {
        DrawInModeDirect();
    }

    if (MODE_SHOW_MEMINT_IS_SAVED || MODE_SHOW_MEMINT_IS_BOTH)
    {
        DrawDataInternal();
    }
}


void DataPainter::DrawDataInModeWorkLatestMemInt()
{
    if (Processing::out.ds.valid)
    {
        DrawDataChannel(Processing::out, Chan::A, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Processing::out, Chan::B, GRID_TOP, Grid::ChannelBottom());
    }
}


void DataPainter::DrawDataChannel(DataStruct &data, Chan ch, int minY, int maxY)
{
    if (!ch.Enabled())
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

    if (ch.IsMath())
    {
        ch = Chan::A;
    }

    if (MODE_DRAW_IS_SIGNAL_LINES)
    {
        DrawSignalLined(data.Data(ch).Data(), ds, first, last, minY, maxY, scaleY, scaleX);
    }
    else
    {
        DrawSignalPointed(data.Data(ch).Data(), ds, first, last, minY, maxY, scaleY, scaleX);
    }
}


void DataPainter::DrawMarkersForMeasure(float scaleY, Chan ch)
{
    if (ch == Chan::Math)
    {
        return;
    }

    Color::SetCurrent(ColorCursors(ch));

    for (int numMarker = 0; numMarker < 2; numMarker++)
    {
        int pos = Processing::GetMarkerVoltage(ch, numMarker);

        if (pos != ERROR_VALUE_INT && pos > 0 && pos < 200)
        {
            Painter::DrawDashedHLine((int)(Grid::FullBottom() - pos * scaleY), Grid::Left(), Grid::Right(), 3, 2, 0);
        }

        pos = Processing::GetMarkerTime(ch, numMarker);

        if (pos != ERROR_VALUE_INT && pos > 0 && pos < Grid::Width())
        {
            Painter::DrawDashedVLine((int)(Grid::Left() + pos), GRID_TOP, Grid::FullBottom(), 3, 2, 0);
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
            Point().Set((int)(Grid::Left() + index * scaleX), dat);
        }
    }
}


void DataPainter::DrawMath()
{
    if (DISABLED_DRAW_MATH)
    {
        return;
    }

    BufferFPGA   &dataA = Processing::out.A;
    BufferFPGA   &dataB = Processing::out.B;
    DataSettings &ds = Processing::out.ds;

    float absA[FPGA::MAX_BYTES];
    float absB[FPGA::MAX_BYTES];

    ValueFPGA::ToVoltageArray(dataA.Data(), ds.BytesInChanReal(), ds.range[Chan::A], (int16)ds.rshiftA, absA);
    ValueFPGA::ToVoltageArray(dataB.Data(), ds.BytesInChanReal(), ds.range[Chan::B], (int16)ds.rshiftB, absB);

    Math::CalculateMathFunction(absA, absB, ds.BytesInChanReal());

    DataStruct data;
    data.ds.Set(ds);
    data.A.Realloc(ds.BytesInChanReal());

    ValueFPGA::FromVoltageArray(absA, ds.BytesInChanReal(), SET_RANGE_MATH, SET_RSHIFT_MATH, data.A.Data());
    data.ds.range[ChA] = SET_RANGE_MATH;
    data.ds.rshiftA = (uint)SET_RSHIFT_MATH;

    DrawDataChannel(data, Chan::Math, Grid::MathTop(), Grid::MathBottom());

    static const int width = 71;
    static const int height = 10;
    int delta = (SHOW_STRING_NAVIGATION && MODE_DRAW_MATH_IS_TOGETHER) ? 10 : 0;
    Rectangle(width, height).Draw(Grid::Left(), Grid::MathTop() + delta, COLOR_FILL);
    Region(width - 2, height - 2).Fill(Grid::Left() + 1, Grid::MathTop() + 1 + delta, COLOR_BACK);
    Divider::E multiplier = MATH_MULTIPLIER;

    DString(Range::ToString(SET_RANGE_MATH, multiplier)).Draw(Grid::Left() + 2, Grid::MathTop() + 1 + delta, COLOR_FILL);

    DString(":").Draw(Grid::Left() + 25, Grid::MathTop() + 1 + delta);

    SET_RSHIFT_MATH.ToString(SET_RANGE_MATH, multiplier).Draw(Grid::Left() + 27, Grid::MathTop() + 1 + delta);
}


void DataPainter::DrawBothChannels(DataStruct &data)
{
    int min = GRID_TOP;
    int max = Grid::ChannelBottom();

    if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawDataChannel(data, ChA, min, max);
        DrawDataChannel(data, ChB, min, max);
    }
    else
    {
        DrawDataChannel(data, ChB, min, max);
        DrawDataChannel(data, ChA, min, max);
    }
}


void DataPainter::DrawMinMax(Chan ch)
{
    if (!ch.Enabled())
    {
        return;
    }

    int minY = GRID_TOP;
    int maxY = Grid::ChannelBottom();

    DataStruct data;

    Limitator::GetLimitation(ch, 0, data);
    Processing::SetData(data);
    DrawDataChannel(Processing::out, ch, minY, maxY);

    Limitator::GetLimitation(ch, 1, data);
    Processing::SetData(data);
    DrawDataChannel(Processing::out, ch, minY, maxY);
}


void DataPainter::DrawMinMax()
{
    ModeDrawSignal::E modeOld = MODE_DRAW_SIGNAL;
    MODE_DRAW_SIGNAL = ModeDrawSignal::Lines;

    if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawMinMax(ChA);
        DrawMinMax(ChB);
    }
    else
    {
        DrawMinMax(ChB);
        DrawMinMax(ChA);
    }

    MODE_DRAW_SIGNAL = modeOld;
}


void DataPainter::DrawDataNormal()
{
    if(!Processing::out.ds.valid)
    {
        return;
    }

    int num_signals = Math::Limitation(Storage::SameSettings::GetCount(), 1, NUM_ACCUM);

    if (ENUM_ACCUM > ENumAccumulation::_1)
    {
        static int last_id;     

        if (ENUM_ACCUM_IS_INFINITY)
        {
            DrawBothChannels(Processing::out);
        }
        else
        {
            if (MODE_ACCUM_IS_RESET)
            {
                if (ENumAccumulation::number_drawing == 0 || last_id != Processing::out.ds.id)
                {
                    last_id = Processing::out.ds.id;

                    DrawBothChannels(Processing::out);

                    ENumAccumulation::number_drawing++;
                }
            }
            else
            {
                for (int i = 0; i < num_signals; i++)
                {
                    Processing::SetData(Storage::GetData(i));

                    DrawBothChannels(Processing::out);

                    ENumAccumulation::number_drawing = NUM_ACCUM + 1;
                }
            }
        }
    }
    else
    {
        DrawBothChannels(Processing::out);
    }

    if (NUM_MIN_MAX > 1)
    {
        DrawMinMax();
    }
}


void DataPainter::DrawDataInternal()
{
    DataStruct data;

    if (HAL_ROM::Data::Get(PageMemory::Internal::currentSignal, data))
    {
        Processing::SetData(data);

        DrawDataNormal();
    }
}


void DataPainter::MemoryWindow::Draw()
{
    Processing::SetDataForProcessing(MODE_WORK, true);

    int leftX = 3;
    static const int rightXses[3] = {276, 285, 247};
    int rightX = rightXses[MODE_WORK];

    int top = 1;
    int height = GRID_TOP - 3;
    int bottom = top + height;

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

    Rectangle(xVert1 - xVert0, bottom - top - (showFull ? 0 : 2)).Draw(xVert0, top + (showFull ? 0 : 1), COLOR_FILL);

    if (Processing::out.ds.valid)
    {
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
    }

    int x[] = {leftX, (rightX - leftX) / 2 + leftX + 1, rightX};
    int x0 = x[SET_TPOS];

    // Маркер TPos
    Region(6, 6).Fill(x0 - 3, 9, COLOR_BACK);
    Char(SYMBOL_TPOS_1).Draw(x0 - 3, 9, COLOR_FILL);

    // Маркер tshift
    float scale = (float)(rightX - leftX + 1) / ((float)ENUM_POINTS_FPGA::ToNumPoints() -
        (ENUM_POINTS_FPGA::ToNumPoints() == 281 ? 1 : 0));

    float xShift = 1 + (TPos::InPoints(SET_ENUM_POINTS, SET_TPOS) - SET_TSHIFT * 2) * scale;

    if (xShift < leftX - 2)
    {
        xShift = (float)(leftX - 2);
    }

    Region(6, 6).Fill((int)(xShift - 1), 3, COLOR_BACK);
    Region(4, 4).Fill((int)(xShift), 4, COLOR_FILL);
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

    Buffer1024<uint8> min(width + 1, 255);
    Buffer1024<uint8> max(width + 1, 0);

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

    Buffer1024<uint8> points(min.Size() * 2);

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
        Painter::DrawVLineArray(x, width, points.Data(), ColorChannel(ch), transparency);
    }
    else
    {
        Painter::DrawVLineArray(x, 255, points.Data(), ColorChannel(ch), transparency);
        Painter::DrawVLineArray(x + 255, width - 255, points.Data() + 255 * 2, ColorChannel(ch), transparency);
    }
}


void DataPainter::DrawSignal(const int _x, const uint8 data[281], bool modeLines)
{
    int x = _x + 1;

    if (modeLines)
    {
        int y_prev = data[0];

        for (int i = 1; i < 281; i++)
        {
            int y = data[i];

            if (y == y_prev)
            {
                Point().Set(x, y);
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
            Point().Set(x++, data[i]);
        }
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer<284> command((uint8)(modeLines ? DRAW_SIGNAL_LINES : DRAW_SIGNAL_POINTS));
        command.PushHalfWord(_x);

        for (int i = 0; i < 281; i++)
        {
            command.PushByte(data[i]);
        }

        command.Transmit(284);
    }
}


void DataPainter::Spectrum::Draw()
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
            DRAW_SPECTRUM(Processing::out.A.Data(), numPoints, Chan::A);
        }
        else if (SOURCE_FFT_IS_B)
        {
            DRAW_SPECTRUM(Processing::out.B.Data(), numPoints, Chan::B);
        }
        else
        {
            if (LAST_AFFECTED_CHANNEL_IS_A)
            {
                DRAW_SPECTRUM(Processing::out.B.Data(), numPoints, Chan::B);
                DRAW_SPECTRUM(Processing::out.A.Data(), numPoints, Chan::A);
            }
            else
            {
                DRAW_SPECTRUM(Processing::out.A.Data(), numPoints, Chan::A);
                DRAW_SPECTRUM(Processing::out.B.Data(), numPoints, Chan::B);
            }
        }
    }

    Painter::DrawHLine(Grid::ChannelBottom(), Grid::Left(), Grid::Right(), COLOR_FILL);
    Painter::DrawHLine(Grid::MathBottom(), Grid::Left(), Grid::Right());
}


void DataPainter::Spectrum::DRAW_SPECTRUM(const uint8 *data, int numPoints, Chan ch)
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

    ValueFPGA::ToVoltageArray(data, numPoints, Processing::out.ds.range[ch], (ch == Chan::A) ?
        (int16)Processing::out.ds.rshiftA :
        (int16)Processing::out.ds.rshiftB, dataR);

    Math::CalculateFFT(dataR, numPoints, spectrum, &freq0, &density0, &freq1, &density1, &y0, &y1);
    DrawChannel(spectrum, ColorChannel(ch));

    if (!Menu::IsShown() || Menu::IsMinimize())
    {
        Color::E color = COLOR_FILL;
        WriteParameters(ch, freq0, density0, freq1, density1);
        Rectangle(s * 2, s * 2).Draw(FFT_POS_CURSOR_0 + Grid::Left() - s, y0 - s, color);
        Rectangle(s * 2, s * 2).Draw(FFT_POS_CURSOR_1 + Grid::Left() - s, y1 - s);

        Painter::DrawVLine(Grid::Left() + FFT_POS_CURSOR_0, Grid::MathBottom(), y0 + s);
        Painter::DrawVLine(Grid::Left() + FFT_POS_CURSOR_1, Grid::MathBottom(), y1 + s);
    }
}


void DataPainter::Spectrum::DrawChannel(const float *spectrum, Color::E color)
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


void DataPainter::Spectrum::WriteParameters(Chan ch, float freq0, float density0, float freq1, float density1)
{
    int x = Grid::Left() + 259;
    int y = Grid::ChannelBottom() + 5;
    int dY = 10;

    Color::SetCurrent(COLOR_FILL);
    SU::Freq2String(freq0, false).Draw(x, y);
    y += dY;
    SU::Freq2String(freq1, false).Draw(x, y);

    if (ch == Chan::A)
    {
        y += dY + 2;
    }
    else
    {
        y += dY * 3 + 4;
    }

    Color::SetCurrent(ColorChannel(ch));
    String<>(SCALE_FFT_IS_LOG ? SU::Float2Db(density0, 4).c_str() : SU::Float2String(density0, false, 7).c_str()).Draw(x, y);
    y += dY;
    DString(SCALE_FFT_IS_LOG ? SU::Float2Db(density1, 4).c_str() : SU::Float2String(density1, false, 7).c_str()).Draw(x, y);
}
