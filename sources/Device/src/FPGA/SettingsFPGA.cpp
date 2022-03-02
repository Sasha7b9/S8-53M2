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
#include "Utils/Strings.h"
#include <cstring>


StateWorkFPGA::E StateWorkFPGA::current = StateWorkFPGA::Stop;


const float Range::scale[Range::Count] = {2e-3f, 5e-3f, 10e-3f, 20e-3f, 50e-3f, 100e-3f, 200e-3f, 500e-3f, 1.0f, 2.0f, 5.0f, 10.0f, 20.0f};

const float TShift::absStep[TBase::Count] =
{
    2e-9f / 20, 5e-9f / 20, 10e-9f / 20, 20e-9f / 20, 50e-9f / 20, 100e-9f / 20, 200e-9f / 20, 500e-9f / 20,
    1e-6f / 20, 2e-6f / 20, 5e-6f / 20, 10e-6f / 20, 20e-6f / 20, 50e-6f / 20, 100e-6f / 20, 200e-6f / 20, 500e-6f / 20,
    1e-3f / 20, 2e-3f / 20, 5e-3f / 20, 10e-3f / 20, 20e-3f / 20, 50e-3f / 20, 100e-3f / 20, 200e-3f / 20, 500e-3f / 20,
    1.0f / 20, 2.0f / 20, 5.0f / 20, 10.0f / 20
};


const float Range::voltsInPoint[Range::Count] =
{
    2e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 2mV
    5e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 5mV
    10e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 10mV
    20e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 20mV
    50e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 50mV
    100e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 100mV
    200e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 200mV
    500e-3f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 500mV
    1.0f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 1V
    2.0f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 2V
    5.0f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 5V
    10.0f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN),  // 10V
    20.0f * 10.0f / (float)(ValueFPGA::MAX - ValueFPGA::MIN)   // 20V
};


const float RShift::absStep[Range::Count] =
{
    2e-3f / 20 / RShift::STEP,
    5e-3f / 20 / RShift::STEP,
    10e-3f / 20 / RShift::STEP,
    20e-3f / 20 / RShift::STEP,
    50e-3f / 20 / RShift::STEP,
    100e-3f / 20 / RShift::STEP,
    200e-3f / 20 / RShift::STEP,
    500e-3f / 20 / RShift::STEP,
    1.0f / 20 / RShift::STEP,
    2.0f / 20 / RShift::STEP,
    5.0f / 20 / RShift::STEP,
    10.0f / 20 / RShift::STEP,
    20.0f / 20 / RShift::STEP
};


namespace FPGA
{
    // Добавочные смещения по времени для разверёток режима рандомизатора.
    int deltaTShift[TBase::Count] = {505, 489, 464, 412, 258};

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
    TShift::Set(SET_TSHIFT);
    TBase::Load();
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
        float rShiftAbs = SET_RSHIFT(ch).ToAbs(SET_RANGE(ch));
        float trigLevAbs = TRIG_LEVEL(ch).ToAbs(SET_RANGE(ch));

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
        Display::ShowWarningBad(ch == Chan::A ? Warning::LimitChan1_Volts : Warning::LimitChan2_Volts);
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
        float tShiftAbsOld = TShift::ToAbs(SET_TSHIFT, SET_TBASE);
        SET_TBASE = tBase;
        Load();
        TShift::Set(TShift::ToRel(tShiftAbsOld, SET_TBASE));
        Display::Redraw();
    }
    else
    {
        Display::ShowWarningBad(Warning::LimitSweep_Time);
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
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 2ns
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 5ns
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 10ns
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 20ns
        {BINARY_U8(00000000), BINARY_U8(00000000)},     // 50ns
        {BINARY_U8(00100001), BINARY_U8(00100001)},     // 100ns
        {BINARY_U8(00100010), BINARY_U8(00100010)},     // 200ns
        {BINARY_U8(00100011), BINARY_U8(00100010)},     // 500ns
        {BINARY_U8(01000101), BINARY_U8(00100011)},     // 1us
        {BINARY_U8(01000110), BINARY_U8(01000101)},     // 2us
        {BINARY_U8(01000111), BINARY_U8(01000110)},     // 5us
        {BINARY_U8(01001001), BINARY_U8(01000111)},     // 10us
        {BINARY_U8(01001010), BINARY_U8(01001001)},     // 20us
        {BINARY_U8(01001011), BINARY_U8(01001010)},     // 50us
        {BINARY_U8(01001101), BINARY_U8(01001011)},     // 100us
        {BINARY_U8(01001110), BINARY_U8(01001101)},     // 200us
        {BINARY_U8(01001111), BINARY_U8(01001110)},     // 500us
        {BINARY_U8(01010001), BINARY_U8(01001111)},     // 1ms
        {BINARY_U8(01010010), BINARY_U8(01010001)},     // 2ms
        {BINARY_U8(01010011), BINARY_U8(01010010)},     // 5ms
        {BINARY_U8(01010101), BINARY_U8(01010011)},     // 10ms
        {BINARY_U8(01010110), BINARY_U8(01010101)},     // 20ms
        {BINARY_U8(01010111), BINARY_U8(01010110)},     // 50ms
        {BINARY_U8(01011001), BINARY_U8(01010111)},     // 100ms
        {BINARY_U8(01011010), BINARY_U8(01011001)},     // 200ms
        {BINARY_U8(01011011), BINARY_U8(01011010)},     // 500ms
        {BINARY_U8(01011101), BINARY_U8(01011011)},     // 1s
        {BINARY_U8(01011110), BINARY_U8(01011101)},     // 2s
        {BINARY_U8(01011111), BINARY_U8(01011110)},     // 5s
        {BINARY_U8(01111111), BINARY_U8(01011111)}      // 10s
    };

    TBase::E tBase = SET_TBASE;
    uint8 mask = SET_PEAKDET_IS_ENABLE ? masksTBase[tBase].maskPeackDet : masksTBase[tBase].maskNorm;

    char buffer[10];
    LOG_WRITE("%d = %s", tBase, Bin2String(mask, buffer));

    BUS_FPGA::Write(WR_RAZV, mask, true);
    ADD_SHIFT_T0 = FPGA::deltaTShift[tBase];
}


void TBase::Decrease()
{
    if (SET_PEAKDET_IS_ENABLE && (SET_TBASE <= TBase::MIN_PEC_DEAT))
    {
        Display::ShowWarningBad(Warning::LimitSweep_Time);
        Display::ShowWarningBad(Warning::EnabledPeakDet);
        return;
    }

    if ((int)SET_TBASE > 0)
    {
        if (SET_SELFRECORDER && SET_TBASE == TBase::MIN_P2P)
        {
            Display::ShowWarningBad(Warning::TooFastScanForSelfRecorder);
        }
        else
        {
            TBase::E base = (TBase::E)((int)SET_TBASE - 1);
            TBase::Set(base);
        }
    }
    else
    {
        Display::ShowWarningBad(Warning::LimitSweep_Time);
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
        Display::ShowWarningBad(Warning::LimitSweep_Time);
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
        Display::ShowWarningBad(ch == Chan::A ? Warning::LimitChan1_RShift : Warning::LimitChan2_RShift);
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
        Display::ShowWarningBad(Warning::LimitSweep_Level);
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
        Math::Limitation<int>(&tShift, TShift::Min(), TShift::MAX);

        Display::ShowWarningBad(Warning::LimitSweep_TShift);
    }

    SET_TSHIFT = (int16)tShift;

    FPGA::Launch::Load();

    Display::Redraw();
};


void TShift::SetDelta(int shift)
{
    FPGA::deltaTShift[SET_TBASE] = shift;

    FPGA::Launch::Load();
}


void PeackDetMode::Set(PeackDetMode::E peackDetMode)
{
    SET_PEAKDET = peackDetMode;
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

    if (SET_TBASE < TBase::MAX_RAND)
    {
        _SET_BIT(data, UPR_BIT_RAND);
    }

    if (!SET_PEAKDET_IS_DISABLE)
    {
        _SET_BIT(data, UPR_BIT_PEAKDET);
    }

    if (CALIBRATOR_IS_FREQ)
    {
        _SET_BIT(data, UPR_BIT_CALIBRATOR_AC_DC);
    }

    else if (CALIBRATOR_IS_DC)
    {
        _SET_BIT(data, UPR_BIT_CALIBRATOR_VOLTAGE);
    }

    BUS_FPGA::Write(WR_UPR, data, true);
}


void FPGA::LoadKoeffCalibration(Chan::E ch)
{
    BUS_FPGA::WriteToHardware(ch == Chan::A ? WR_CAL_A : WR_CAL_B, STRETCH_ADC(ch) * 0x80, false);
}


pchar TShift::ToString(int tShiftRel, char buffer[20])
{
    float tShiftVal = TShift::ToAbs(tShiftRel, SET_TBASE);
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
       Display::ShowWarningBad(ch == Chan::A ? Warning::LimitChan1_Volts : Warning::LimitChan2_Volts);
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
        Display::ShowWarningBad(ch == Chan::A ? Warning::LimitChan1_Volts : Warning::LimitChan2_Volts);
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
    BUS_FPGA::Write(WR_TRIG, TRIG_POLARITY_IS_FRONT ? 0x00U : 0x01U, true);
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


bool TBase::InModeRandomizer()
{
    return SET_TBASE <= MAX_RAND;
}


bool TBase::InModeP2P()
{
    return SET_TBASE >= MIN_P2P;
}


int TShift::Zero()
{
    return -Min();
}


int TShift::Min()
{
    static const int16 m[3][3] =
    {
        {-511, -441, -371},
        {-511, -383, -255},
        {-511, -255, 0}
    };

    if ((int)SET_ENUM_POINTS < 3 && (int)SET_ENUM_POINTS >= 0)
    {
        return m[SET_ENUM_POINTS][SET_TPOS];
    }

    LOG_ERROR("");
    return 0;
}


int TPos::InPoints(ENUM_POINTS_FPGA::E enum_points, TPos::E tPos)
{
    static const int m[3][3] =
    {
        {0, 140, 280},
        {0, 255, 511},
        {0, 512, 1022}
    };

    return m[enum_points][tPos];
}


int RShift::ToRel(float rShiftAbs, Range::E range)
{
    int retValue = ZERO + rShiftAbs / RShift::absStep[range];

    if (retValue < MIN)
    {
        retValue = MIN;
    }
    else if (retValue > MAX)
    {
        retValue = MAX;
    }

    return retValue;
};


void DataSettings::Init()
{
    rec_point = TBase::InModeP2P() ? 0 : -1;

    en_a = Chan::Enabled(Chan::A) ? 1U : 0U;
    en_b = Chan::Enabled(Chan::B) ? 1U : 0U;
    inv_a = SET_INVERSE_A ? 1U : 0U;
    inv_b = SET_INVERSE_B ? 1U : 0U;
    range[0] = SET_RANGE_A;
    range[1] = SET_RANGE_B;
    rShiftA = (uint)SET_RSHIFT_A;
    rShiftB = (uint)SET_RSHIFT_B;
    tBase = SET_TBASE;
    tShift = SET_TSHIFT;
    coupleA = SET_COUPLE_A;
    coupleB = SET_COUPLE_B;
    e_points_in_channel = SET_ENUM_POINTS;
    trigLevA = (uint)TRIG_LEVEL_A;
    trigLevB = (uint)TRIG_LEVEL_B;
    peakDet = (uint)SET_PEAKDET;
    div_a = SET_DIVIDER_A;
    div_b = SET_DIVIDER_B;
}


int TBase::StretchRand()
{
    static const int N = 100;

    //                                    2ns  5ns  10ns  20ns  50ns
    static const int Kr[TBase::Count] = { N/1, N/2, N/10, N/20, N/50,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    return Kr[SET_TBASE];
}


int DataSettings::BytesInChannel() const
{
    int result = PointsInChannel();

    if (peakDet != 0)
    {
        result *= 2;
    }

    return result;
}


int DataSettings::PointsInChannel() const
{
    static const int num_points[ENUM_POINTS_FPGA::Count] =
    {
        281, 512, 1024
    };

    return (int)num_points[e_points_in_channel];
}


int16 DataSettings::GetRShift(Chan::E ch) const
{
    return (int16)((ch == ChA) ? rShiftA : rShiftB);
}


bool DataSettings::InModeP2P() const
{
    return (tBase >= TBase::MIN_P2P) && (rec_point != -1);
}


void DataSettings::AppendPoints(uint8 *a, uint8 *b, BitSet16 pointsA, BitSet16 pointsB)
{
    if (InModeP2P())
    {
        int max_bytes = BytesInChannel();

        if (rec_point == max_bytes - 1)
        {
            std::memmove(a, a + 1, (uint)(max_bytes - 1));
            rec_point = max_bytes - 2;
        }
        else if (rec_point == max_bytes)
        {
            std::memmove(a, a + 2, (uint)(max_bytes - 2));
            rec_point = max_bytes - 2;
        }

        if (en_a)
        {
            a[rec_point] = pointsA.byte0;
            a[rec_point + 1] = pointsA.byte1;
        }
        if (en_b)
        {
            b[rec_point] = pointsB.byte0;
            b[rec_point + 1] = pointsB.byte1;
        }

        rec_point += 2;

//        if (rec_point == 10)
//        {
//            SU::LogBuffer(a, 10);
//        }
    }
}


float TShift::ToAbs(int shift, TBase::E base)
{
    return absStep[base] * shift * 2.0f;
}


int TShift::ToRel(float shift, TBase::E base)
{
    return shift / absStep[base] / 2.0f;
}


float Range::MaxOnScreen(Range::E range)
{
    return scale[range] * 5.0f;
}


bool DataSettings::Equal(const DataSettings &ds)
{
    return (en_a     == ds.en_a) &&
           (en_b     == ds.en_b) &&
           (inv_a    == ds.inv_a) &&
           (inv_b    == ds.inv_b) &&
           (range[0] == ds.range[0]) &&
           (range[1] == ds.range[1]) &&
           (rShiftA  == ds.rShiftA) &&
           (rShiftB  == ds.rShiftB) &&
           (tBase    == ds.tBase) &&
           (tShift   == ds.tShift) &&
           (coupleA  == ds.coupleA) &&
           (coupleB  == ds.coupleB) &&
           (trigLevA == ds.trigLevA) &&
           (trigLevB == ds.trigLevB) &&
           (div_a    == ds.div_a) &&
           (div_b    == ds.div_b) &&
           (peakDet  == ds.peakDet);
}
