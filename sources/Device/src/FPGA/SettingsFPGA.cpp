// 2022/02/11 17:48:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"


StateWorkFPGA::E StateWorkFPGA::current = StateWorkFPGA::Stop;


namespace FPGA
{
    // Добавочные смещения по времени для разверёток режима рандомизатора.
    int16 deltaTShift[TBase::Count] = {505, 489, 464, 412, 258};

    extern int addition_shift;

    // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
    void LoadSettings();

    // Загрузить все параметры напряжения каналов и синхронизации в аппаратную часть.
    void SetAttribChannelsAndTrig(TypeWriteAnalog::E type);

    // Загрузить регистр WR_UPR (пиковый детектор и калибратор).
    void LoadRegUPR();
}


void FPGA::LoadSettings()
{
    TBase::Load();
    TShift::Load();
    Range::Load(ChA);
    RShift::Load(ChA);
    Range::Load(ChB);
    RShift::Load(ChB);
    TrigLev::Load();
    TrigPolarity::Load();
    LoadRegUPR();
    TrigLev::Load();
    TrigSource::Set(TRIG_SOURCE);

//    PrepareAndWriteDataToAnalogSPI(CS1);
//    PrepareAndWriteDataToAnalogSPI(CS2);
//    PrepareAndWriteDataToAnalogSPI(CS3);
//    PrepareAndWriteDataToAnalogSPI(CS4);
}


void FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::E type) 
{
    /*
    b0...b7 - Channel1
    b8...b15 - Channel2
    b16...b23 - Trig
    Передаваться биты должны начиная с b0
    */

    static const uint8 masksRange[Range::Count] =
    {
        BINARY_U8(0000000),
        BINARY_U8(1000000),
        BINARY_U8(1100000),
        BINARY_U8(1010000),
        BINARY_U8(1000100),
        BINARY_U8(1100100),
        BINARY_U8(1010100),
        BINARY_U8(1000010),
        BINARY_U8(1100010),
        BINARY_U8(1010010),
        BINARY_U8(1000110),
        BINARY_U8(1100110),
        BINARY_U8(1010110)
    };

    uint data = 0;

    // Range0, Range1
    data |= masksRange[SET_RANGE_A];
    data |= (masksRange[SET_RANGE_B] << 8);

    // Параметры каналов
    static const uint maskCouple[2][3] = 
    {
        {0x0008, 0x0000, 0x0030},
        {0x0800, 0x0000, 0x3000}
    };

    data |= maskCouple[Chan::A][SET_COUPLE_A];
    data |= maskCouple[Chan::B][SET_COUPLE_B];

    static const uint maskFiltr[2][2] = 
    {
        {0x0000, 0x0080},
        {0x0000, 0x0100}
    };

    data |= maskFiltr[Chan::A][SET_FILTR_A];
    data |= maskFiltr[Chan::B][SET_FILTR_B];


    // Параметры синхронизации
    static const uint maskSource[3] = {0x000000, 0x800000, 0x400000};
    data |= maskSource[TRIG_SOURCE];

    static const uint maskInput[4] = {0x000000, 0x030000, 0x020000, 0x010000};
    data |= maskInput[TRIG_INPUT];

    BUS_FPGA::WriteAnalog(type, data);
}


void Range::Set(Chan::E ch, Range::E range)
{
    if (!Chan::Enabled(ch))
    {
        return;
    }
    if (range < Range::Count && (int)range >= 0)
    {
        float rShiftAbs = RSHIFT_2_ABS(SET_RSHIFT(ch), SET_RANGE(ch));
        float trigLevAbs = RSHIFT_2_ABS(TRIG_LEVEL(ch), SET_RANGE(ch));

        SET_RANGE(ch) = range;

        if (LINKING_RSHIFT_IS_VOLTAGE)
        {
            SET_RSHIFT(ch) = (int16)RShift::ToRel(rShiftAbs, range);
            TRIG_LEVEL(ch) = (int16)RShift::ToRel(trigLevAbs, range);
        }

        Load(ch);
    }
    else
    {
        Display::ShowWarningBad(ch == Chan::A ? LimitChan1_Volts : LimitChan2_Volts);
    }
};


void Range::Load(Chan::E ch)
{
    FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::RangeA);
    RShift::Load(ch);

    if (ch == (Chan::E)TRIG_SOURCE)
    {
        TrigLev::Load();
    }
}


void TBase::Set(TBase::E tBase)
{
    if (!Chan::Enabled(Chan::A) && !Chan::Enabled(Chan::B))
    {
        return;
    }
    if (tBase < TBase::Count && (int)tBase >= 0)
    {
        float tShiftAbsOld = TSHIFT_2_ABS(TSHIFT, SET_TBASE);
        SET_TBASE = tBase;
        Load();
        TShift::Set(TSHIFT_2_REL(tShiftAbsOld, SET_TBASE));
        Display::Redraw();
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
};


void TBase::Load()
{
    struct TBaseMaskStruct
    {
        uint8 maskNorm;         // Маска. Требуется для записи в аппаратную часть при выключенном режиме пикового детектора.
        uint8 maskPeackDet;     // Маска. Требуется для записи в аппаратную часть при включенном режиме пикового детектора.
    };
    
    static const TBaseMaskStruct masksTBase[TBase::Count] =
    {
        {BINARY_U8(00000000), BINARY_U8(00000000)}, // 2ns
        {BINARY_U8(00000000), BINARY_U8(00000000)}, // 5ns
        {BINARY_U8(00000000), BINARY_U8(00000000)}, // 10ns
        {BINARY_U8(00000000), BINARY_U8(00000000)}, // 20ns
        {BINARY_U8(00000000), BINARY_U8(00000000)}, // 50ns
        {BINARY_U8(00100001), BINARY_U8(00100001)}, // 100ns
        {BINARY_U8(00100010), BINARY_U8(00100010)}, // 200ns
        {BINARY_U8(00100011), BINARY_U8(00100010)}, // 500ns
        {BINARY_U8(01000101), BINARY_U8(00100011)}, // 1us
        {BINARY_U8(01000110), BINARY_U8(01000101)}, // 2us
        {BINARY_U8(01000111), BINARY_U8(01000110)}, // 5us
        {BINARY_U8(01001001), BINARY_U8(01000111)}, // 10us
        {BINARY_U8(01001010), BINARY_U8(01001001)}, // 20us
        {BINARY_U8(01001011), BINARY_U8(01001010)}, // 50us
        {BINARY_U8(01001101), BINARY_U8(01001011)}, // 100us
        {BINARY_U8(01001110), BINARY_U8(01001101)}, // 200us
        {BINARY_U8(01001111), BINARY_U8(01001110)}, // 500us
        {BINARY_U8(01010001), BINARY_U8(01001111)}, // 1ms
        {BINARY_U8(01010010), BINARY_U8(01010001)}, // 2ms
        {BINARY_U8(01010011), BINARY_U8(01010010)}, // 5ms
        {BINARY_U8(01010101), BINARY_U8(01010011)}, // 10ms
        {BINARY_U8(01010110), BINARY_U8(01010101)}, // 20ms
        {BINARY_U8(01010111), BINARY_U8(01010110)}, // 50ms
        {BINARY_U8(01011001), BINARY_U8(01010111)}, // 100ms
        {BINARY_U8(01011010), BINARY_U8(01011001)}, // 200ms
        {BINARY_U8(01011011), BINARY_U8(01011010)}, // 500ms
        {BINARY_U8(01011101), BINARY_U8(01011011)}, // 1s
        {BINARY_U8(01011110), BINARY_U8(01011101)}, // 2s
        {BINARY_U8(01011111), BINARY_U8(01011110)}, // 5s
        {BINARY_U8(01111111), BINARY_U8(01011111)}  // 10s
    };

    TBase::E tBase = SET_TBASE;
    uint8 mask = PEAKDET ? masksTBase[tBase].maskPeackDet : masksTBase[tBase].maskNorm;
    BUS_FPGA::Write(WR_RAZV, mask, true);
    ADD_SHIFT_T0 = FPGA::deltaTShift[tBase];
}


void TBase::Decrease()
{
    if (PEAKDET && SET_TBASE <= TBase::MIN_PEC_DEAT)
    {
        Display::ShowWarningBad(LimitSweep_Time);
        Display::ShowWarningBad(EnabledPeakDet);
        return;
    }

    if ((int)SET_TBASE > 0)
    {
        if (SET_SELFRECORDER && SET_TBASE == TBase::MIN_P2P)
        {
            Display::ShowWarningBad(TooFastScanForSelfRecorder);
        }
        else
        {
            TBase::E base = (TBase::E)((int)SET_TBASE - 1);
            TBase::Set(base);
        }
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
}


void TBase::Increase()
{
    if (SET_TBASE < (TBase::Count - 1))
    {
        TBase::E base = (TBase::E)(SET_TBASE + 1);
        TBase::Set(base);
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
}


void RShift::Set(Chan::E ch, int16 rShift)
{
    if (!Chan::Enabled(ch))
    {
        return;
    }
    Display::ChangedRShiftMarkers();

    if (rShift > RShift::MAX || rShift < RShift::MIN)
    {
        Display::ShowWarningBad(ch == Chan::A ? LimitChan1_RShift : LimitChan2_RShift);
    }

    LIMITATION(rShift, rShift, RShift::MIN, RShift::MAX);
    if (rShift > RShift::ZERO)
    {
        rShift &= 0xfffe;                                            // Делаем кратным двум, т.к. у нас 800 значений на 400 точек
    }
    else if (rShift < RShift::ZERO)
    {
        rShift = (rShift + 1) & 0xfffe;
    }
    SET_RSHIFT(ch) = rShift;
    Load(ch);
    Display::RotateRShift(ch);
};


void RShift::Load(Chan::E ch)
{
    static const uint16 mask[2] = {0x2000, 0x6000};

    Range::E range = SET_RANGE(ch);
    ModeCouple::E mode = SET_COUPLE(ch);
    static const int index[3] = {0, 1, 1};
    int16 rShiftAdd = RSHIFT_ADD(ch, range, index[mode]);

    uint16 rShift = (uint16)(SET_RSHIFT(ch) + (SET_INVERSE(ch) ? -1 : 1) * rShiftAdd);

    int16 delta = -(rShift - RShift::ZERO);
    if (SET_INVERSE(ch))
    {
        delta = -delta;
    }
    rShift = (uint16)(delta + RShift::ZERO);

    rShift = (uint16)(RShift::MAX + RShift::MIN - rShift);
    BUS_FPGA::WriteDAC(ch == Chan::A ? TypeWriteDAC::RShiftA : TypeWriteDAC::RShiftB, (uint16)(mask[ch] | (rShift << 2)));
}


void TrigLev::Set(TrigSource::E ch, int16 trigLev)
{
    Display::ChangedRShiftMarkers();
    if (trigLev < TrigLev::MIN || trigLev > TrigLev::MAX)
    {
        Display::ShowWarningBad(LimitSweep_Level);
    }
    LIMITATION(trigLev, trigLev, TrigLev::MIN, TrigLev::MAX);

    if (trigLev > TrigLev::ZERO)
    {
        trigLev &= 0xfffe;
    }
    else if (trigLev < TrigLev::ZERO)
    {
        trigLev = (trigLev + 1) & 0xfffe;
    }

    if (TRIG_LEVEL(ch) != trigLev)
    {
        TRIG_LEVEL(ch) = trigLev;
        Load();
        Display::RotateTrigLev();
    }
};


void TrigLev::Load()
{
    uint16 data = 0xa000;
    uint16 trigLev = (uint16)TRIG_LEVEL_SOURCE;
    trigLev = (uint16)(TrigLev::MAX + TrigLev::MIN - trigLev);
    data |= trigLev << 2;
    BUS_FPGA::WriteDAC(TypeWriteDAC::TrigLev, data);
}


void TShift::Set(int tShift)
{
    if (!Chan::Enabled(Chan::A) && !Chan::Enabled(Chan::B))
    {
        return;
    }

    if (tShift < TShift::Min() || tShift > TShift::MAX)
    {
        LIMITATION(tShift, tShift, TShift::Min(), TShift::MAX);
        Display::ShowWarningBad(LimitSweep_TShift);
    }

    TSHIFT = (int16)tShift;
    Load();
    Display::Redraw();
};


void TShift::SetDelta(int16 shift)
{
    FPGA::deltaTShift[SET_TBASE] = shift;
    Load();
}


void PeackDetMode::Set(PeackDetMode::E peackDetMode)
{
    PEAKDET = peackDetMode;
    FPGA::LoadRegUPR();
}


void CalibratorMode::Set(CalibratorMode::E calibratorMode)
{
    CALIBRATOR = calibratorMode;
    FPGA::LoadRegUPR();
}


void FPGA::LoadRegUPR()
{
    uint8 data = 0;

    if (TBase::InRandomizeMode())
    {
        _SET_BIT(data, 0);
    }

    if (!PEAKDET_IS_DISABLE)
    {
        _SET_BIT(data, 1);
    }

    if (CALIBRATOR_IS_FREQ)
    {
        _SET_BIT(data, 2);
    }

    else if (CALIBRATOR_IS_DC)
    {
        _SET_BIT(data, 3);
    }

    BUS_FPGA::Write(WR_UPR, data, true);
}


void FPGA::LoadKoeffCalibration(Chan::E ch)
{
    BUS_FPGA::WriteToHardware(ch == Chan::A ? WR_CAL_A : WR_CAL_B, STRETCH_ADC(ch) * 0x80, false);
}


void TShift::Load()
{
    static const int16 k[TBase::Count] = {50, 20, 10, 5, 2};
    int16 tShift = TSHIFT - TShift::Min() + 1;
    TBase::E tBase = SET_TBASE;

    FPGA::addition_shift = (tShift % k[tBase]) * 2;

    if ((tBase <= TBase::MAX_RAND) && 
        ((int)tBase >= 0))
    {
        tShift = tShift / k[tBase] + FPGA::deltaTShift[tBase];
    }

    uint16 post = (uint16)tShift;
    uint16 pred = (uint16)((tShift > 511) ? 1023 : (511 - post));

    post = (uint16)(~post);
    pred = (uint16)((~(pred - 1)) & 0x1ff);

    BUS_FPGA::Write(WR_POST, post, true);
    BUS_FPGA::Write(WR_PRED, pred, true);
}


pchar TShift::ToString(int16 tShiftRel, char buffer[20])
{
    float tShiftVal = TSHIFT_2_ABS(tShiftRel, SET_TBASE);
    return Time2String(tShiftVal, true, buffer);
}


bool Range::Increase(Chan::E ch)
{
    bool retValue = false;
    if (SET_RANGE(ch) < Range::Count - 1)
    {
        Range::Set(ch, (Range::E)(SET_RANGE(ch) + 1));
        retValue = true;
    }
    else
    {
       Display::ShowWarningBad(ch == Chan::A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    Display::Redraw();
    return retValue;
};


bool Range::Decrease(Chan::E ch)
{
    bool retValue = false;
    if (SET_RANGE(ch) > 0)
    {
        Range::Set(ch, (Range::E)(SET_RANGE(ch) - 1));
        retValue = true;
    }
    else
    {
        Display::ShowWarningBad(ch == Chan::A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    Display::Redraw();
    return retValue;
};


void TrigSource::Set(TrigSource::E trigSource)
{
    TRIG_SOURCE = trigSource;
    FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::TrigParam);
    if (!TRIG_SOURCE_IS_EXT)
    {
        TrigLev::Set(TRIG_SOURCE, TRIG_LEVEL_SOURCE);
    }
}


void TrigPolarity::Set(TrigPolarity::E polarity)
{
    TRIG_POLARITY = polarity;
    Load();
}


void TrigPolarity::Load()
{
    BUS_FPGA::Write(WR_TRIG, TRIG_POLARITY_IS_FRONT ? 0x01U : 0x00U, true);
}


void TrigInput::Set(TrigInput::E trigInput)
{
    TRIG_INPUT = trigInput;
    FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::TrigParam);
}


void ModeCouple::Set(Chan::E ch, ModeCouple::E modeCoupe)
{
    SET_COUPLE(ch) = modeCoupe;
    FPGA::SetAttribChannelsAndTrig(ch == Chan::A ? TypeWriteAnalog::ChanParam0 : TypeWriteAnalog::ChanParam1);
    RShift::Set(ch, SET_RSHIFT(ch));
}


void Filtr::Enable(Chan::E ch, bool enable)
{
    SET_FILTR(ch) = enable;
    FPGA::SetAttribChannelsAndTrig(ch == Chan::A ? TypeWriteAnalog::ChanParam0 : TypeWriteAnalog::ChanParam1);
}


bool TBase::InRandomizeMode()
{
    return SET_TBASE <= TBase::MAX_RAND;
}


int16 TShift::Zero()
{
    return -TShift::Min();
}


int16 TShift::Min()
{
    static const int16 m[3][3] =
    {
        {-511, -441, -371},
        {-511, -383, -255},
        {-511, -255, 0}
    };

    if ((int)ENUM_POINTS < 3 && (int)ENUM_POINTS >= 0)
    {
        return m[ENUM_POINTS][SET_TPOS];
    }

    LOG_ERROR("");
    return 0;
}


int TShift::InPoints(PeackDetMode::E peakDet)
{
    return TSHIFT * (peakDet == PeackDetMode::Disable ? 2 : 1);
}


int TPos::InPoints(PeackDetMode::E peakDet, int numPoints, TPos::E tPos)
{
    if (peakDet == PeackDetMode::Disable)
    {
        static const int m[3][3] =
        {
            {0, 140, 280},
            {0, 255, 511},
            {0, 512, 1022}
        };
        return m[ENUM_POINTS_FPGA::FromNumPoints(numPoints)][tPos];
    }
    else
    {
        static const int m[3][3] =
        {
            {0, 140, 280},
            {0, 256, 510},
            {0, 256, 510}
        };
        return m[ENUM_POINTS_FPGA::FromNumPoints(numPoints)][tPos];
    }
}


int RShift::ToRel(float rShiftAbs, Range::E range)
{
    int retValue = RShift::ZERO + rShiftAbs / absStepRShift[range];
    if (retValue < RShift::MIN)
    {
        retValue = RShift::MIN;
    }
    else if (retValue > RShift::MAX)
    {
        retValue = RShift::MAX;
    }
    return retValue;
};


void DataSettings::FillDataPointer()
{
    enableCh0 = Chan::Enabled(Chan::A) ? 1U : 0U;
    enableCh1 = Chan::Enabled(Chan::B) ? 1U : 0U;
    inverseCh0 = SET_INVERSE_A ? 1U : 0U;
    inverseCh1 = SET_INVERSE_B ? 1U : 0U;
    range[0] = SET_RANGE_A;
    range[1] = SET_RANGE_B;
    rShiftCh0 = (uint)SET_RSHIFT_A;
    rShiftCh1 = (uint)SET_RSHIFT_B;
    tBase = SET_TBASE;
    tShift = TSHIFT;
    modeCouple0 = SET_COUPLE_A;
    modeCouple1 = SET_COUPLE_B;
    length1channel = (uint)ENUM_POINTS_FPGA::ToNumPoints(false);
    trigLevCh0 = (uint)TRIG_LEVEL_A;
    trigLevCh1 = (uint)TRIG_LEVEL_B;
    peakDet = (uint)PEAKDET;
    multiplier0 = SET_DIVIDER_A;
    multiplier1 = SET_DIVIDER_B;
}


int TBase::StepRand()
{
    static const int num_Kr = 200;
    static const int Kr[] = {num_Kr / 2, num_Kr / 5, num_Kr / 10, num_Kr / 20, num_Kr / 50};

    return TBase::InRandomizeMode() ? Kr[SET_TBASE] : 1;
}

int TShift::ForLaunchFPGA()
{
    return TSHIFT - TShift::Min();
}
