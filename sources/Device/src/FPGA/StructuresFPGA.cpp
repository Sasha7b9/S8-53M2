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
        static int pred = 0;    // Пред- и после- запуски хранятся в точках.
        static int post = 0;    // Перед засылкой их нужно уменьшать в два раза, потому что две точки считываются за раз

        static const int8 d_pred[TBase::Count] =   // Дополнительное смещение для предзапуска
        {//  2    5   10   20   50  100  200
            10,  10,  10,  10,  10,   5,   3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        };

        static const int8 d_post[TBase::Count] =   // Дополнительное смещение для послезапуска
        {//  2    5   10   20   50  100  200
            10,  20,  10,  10,  10,   5,   3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        };

//        static const int8 d_read[TBase::Count] =   // Дополнительное смещение для чтения адреса
//        {// 2    5   10   20   50  100  200
//            0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//        };

        void Calculate();

        void CalculateReal();

        void CalculateRandomize();

        // Возвращают значение, готовое для записи в ПЛИС
        uint16 PostForWrite();
    }
}


void FPGA::Launch::Load()
{
    if (!FPGA::Reader::mutex_read.IsLocked())
    {
        Calculate();

        BUS_FPGA::Write(WR_POST, PostForWrite(), true);
        BUS_FPGA::Write(WR_PRED, PredForWrite(), true);
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
    int num_points = ENUM_POINTS_FPGA::ToNumBytes();

    if (TSHIFT <= 0)
    {
        pred = num_points / 2 + TSHIFT;
        post = num_points / 2 - TSHIFT;
    }
    else
    {
        pred = num_points / 2;
        post = num_points / 2;
    }
}


void FPGA::Launch::CalculateRandomize()
{
}
