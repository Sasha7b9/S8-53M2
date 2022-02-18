// 2022/02/11 17:44:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Grid.h"
#include "Settings/Settings.h"


namespace Grid
{
    int SignalWidth();
}


int Grid::Left()
{
    return (MenuIsMinimize() && MenuIsShown() ? 9 : 20) + Measure_GetDeltaGridLeft();
}


int Grid::Right()
{
    return ((MenuIsMinimize() && MenuIsShown()) ? 9 : 20) + 280;
}


int Grid::ChannelBottom()
{
    return (sDisplay_IsSeparate()) ? (GRID_TOP + GRID_HEIGHT / 2) : FullBottom();
}


int Grid::SignalWidth()
{
    return Width();
}


int Grid::ChannelHeight()
{
    return (sDisplay_IsSeparate()) ? FullHeight() / 2 : FullHeight();
}


int Grid::ChannelCenterHeight()
{
    return (GRID_TOP + ChannelBottom()) / 2;
}


int Grid::FullBottom()
{
    return GRID_BOTTOM - Measure_GetDeltaGridBottom();
}


int Grid::FullHeight()
{
    return FullBottom() - GRID_TOP;
}


int Grid::Width()
{
    return Right() - Left();
}


int Grid::FullCenterHeight()
{
    return (FullBottom() + GRID_TOP) / 2;
}


int Grid::WidthInCells()
{
    return MenuIsShown() ? 10 : 14;
}


float Grid::DeltaY()
{
    float delta = (FullBottom() - GRID_TOP) / 10.0f;
    return sDisplay_IsSeparate() ? (delta / 2.0f) : delta;
}


float Grid::DeltaX()
{
    float delta = (Right() - Left()) / 14.0f;
    return delta;
}


int Grid::MathTop()
{
    return MathBottom() - MathHeight();
}


int Grid::MathHeight()
{
    if (ENABLED_FFT || MODE_DRAW_MATH_IS_SEPARATE)
    {
        return FullHeight() / 2;
    }
    return FullHeight();
}


int Grid::MathBottom()
{
    return FullBottom();
}


int Grid::BottomMessages()
{
    int retValue = FullBottom();
    if (MODE_WORK_IS_MEMINT)
    {
        retValue -= 12;
    }
    else if (SHOW_MEASURES)
    {
        retValue = TOP_MEASURES;
        if (MEAS_NUM_IS_6_1 || MEAS_NUM_IS_6_2 || MEAS_NUM_IS_1)
        {
            retValue = FullBottom();
        }
    }
    return retValue - 12;;
}
