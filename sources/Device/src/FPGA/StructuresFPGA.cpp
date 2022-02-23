// 2022/02/23 08:43:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include <climits>


namespace FPGA
{
    namespace Launch
    {
        static int pred = 0;
        static int post = 0;

        static const int8 d_pred[TBase::Count] =   // Дополнительное смещение для предзапуска
        {//  2    5   10   20   50  100  200
            10,  10,  10,  10,  10,   5,   3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        };

        static const int8 d_post[TBase::Count] =   // Дополнительное смещение для послезапуска
        {//  2    5   10   20   50  100  200
            10,  20,  10,  10,  10,   5,   3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        };

        static const int8 d_read[TBase::Count] =   // Дополнительное смещение для чтения адреса
        {// 2    5   10   20   50  100  200
            0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

        void Calculate();

        void CalculateReal();

        void CalculateRandomize();

        // Возвращают значения, готовые для записи в ПЛИС
        uint16 PredForWrite();
        uint16 PostForWrite();
    }
}


void FPGA::Launch::Load()
{
    if (!FPGA::Reader::mutex_read.IsLocked())
    {
        Calculate();

        BUS_FPGA::Write(WR_POST, PostForWrite(), false);
        BUS_FPGA::Write(WR_PRED, PredForWrite(), false);
    }
}


uint16 FPGA::Launch::PostForWrite()
{
    int result = post + d_post[SET_TBASE];

    if (PEAKDET_IS_ENABLE)
    {
        result *= 2;
    }

    return (uint16)(~result);
}


uint16 FPGA::Launch::PredForWrite()
{
    int result = pred + d_pred[SET_TBASE];

    if (PEAKDET_IS_ENABLE)
    {
        result *= 2;
    }

    return (uint16)(~result);
}


int FPGA::Launch::DeltaReadAddress()
{
    int result = 0;

    if (TShift::ForLaunchFPGA() < 0)
    {
        result += TShift::ForLaunchFPGA();
    }

    return result - d_read[SET_TBASE];
}


void FPGA::Launch::Calculate()
{
    if (TBase::InRandomizeMode())
    {
        CalculateRandomize();
    }
    else
    {
        CalculateReal();
    }
}


void FPGA::Launch::CalculateReal()
{
    const int shift = TShift::ForLaunchFPGA();

    post = shift / 2;

    int pointsInChannelHalf = ENUM_POINTS_FPGA::ToNumPoints() / 2;

    pred = Math::Limitation(pointsInChannelHalf - post, 0, INT_MAX);

    if (post + pred < pointsInChannelHalf)
    {
        pred = pointsInChannelHalf - post;
    }

    if (shift < 0)
    {
        post = 0;
    }
}


void FPGA::Launch::CalculateRandomize()
{
    int k = TBase::StepRand();
    int shift = TShift::ForLaunchFPGA();

    if (SET_TBASE == TBase::_50ns)      shift += 2;
    else if (SET_TBASE == TBase::_20ns) shift += 5;
    else if (SET_TBASE == TBase::_10ns) shift += 20;
    else if (SET_TBASE == TBase::_5ns)  shift -= 140;
    else if (SET_TBASE == TBase::_2ns)  shift += 100;

    int num_points = ENUM_POINTS_FPGA::ToNumPoints();
    int equal_points = num_points / k;      // Эквивалентное количество точек, которые нужно считать. Оно в коэффициент
                                            // растяжки меньше реального количесва точек, очевидно.
    post = shift / k;

    pred = equal_points - post;

    if (pred + post < equal_points)
    {
        pred = equal_points - post;
    }
}
