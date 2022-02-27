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

        void Calculate();

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
    return (uint16)(~post);
}


uint16 FPGA::Launch::PredForWrite()
{
    return (uint16)(~pred);
}


void FPGA::Launch::Calculate()
{
    static const int8 d_pred[TBase::Count] =   // Дополнительное смещение для предзапуска
    {//  2    5   10   20   50  100  200
        10,  10,  10,  10,  10,   5,   3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    static const int8 d_post[TBase::Count] =   // Дополнительное смещение для послезапуска
    {//  2    5   10   20   50  100  200
        10,  20,  10,  10,  10,   5,   3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    int num_bytes = ENUM_POINTS_FPGA::ToNumBytes();

    int values[TPos::Count] = {num_bytes, num_bytes / 2, 0};

    pred = post = values[SET_TPOS];

    int tShift = SET_TSHIFT * (SET_PEAKDET_IS_ENABLE ? 2 : 1);

    pred = Math::Limitation(pred - tShift * 2, 0, 65535);

    post = Math::Limitation(post + tShift * 2, 0, 65535);

    if (pred + post < num_bytes)
    {
        post = num_bytes - pred;
    }

    if (TBase::InRandomizeMode())
    {
        int stretch = TBase::StretchRand();

        pred = pred / stretch + d_pred[SET_TBASE];
        post = post / stretch + d_post[SET_TBASE];
    }
}


int FPGA::Reader::ShiftRead()
{
    static const int shift[TBase::Count] =
    {// 2  5  10 20
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    return shift[SET_TBASE];
}
