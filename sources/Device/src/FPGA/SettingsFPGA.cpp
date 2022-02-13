// 2022/02/11 17:48:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/TypesFPGA_old.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"



StateWorkFPGA::E StateWorkFPGA::current = StateWorkFPGA::Stop;


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


// Добавочные смещения по времени для разверёток режима рандомизатора.
static int16 deltaTShift[TBase::Count] = {505, 489, 464, 412, 258};

struct TBaseMaskStruct
{
    uint8 maskNorm;         // Маска. Требуется для записи в аппаратную часть при выключенном режиме пикового детектора.
    uint8 maskPeackDet;     // Маска. Требуется для записи в аппаратную часть при включенном режиме пикового детектора.
};

static const TBaseMaskStruct masksTBase[TBase::Count] =
{
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00100010), BINARY_U8(00000000)},
    {BINARY_U8(00100001), BINARY_U8(00100011)},
    {BINARY_U8(01000000), BINARY_U8(00100001)},
    {BINARY_U8(01000010), BINARY_U8(01000000)},
    {BINARY_U8(01000001), BINARY_U8(01000011)},
    {BINARY_U8(01000100), BINARY_U8(01000001)},
    {BINARY_U8(01000110), BINARY_U8(01000100)},
    {BINARY_U8(01000101), BINARY_U8(01000111)},
    {BINARY_U8(01001000), BINARY_U8(01000101)},
    {BINARY_U8(01001010), BINARY_U8(01001000)},
    {BINARY_U8(01001001), BINARY_U8(01001011)},
    {BINARY_U8(01001100), BINARY_U8(01001001)},
    {BINARY_U8(01001110), BINARY_U8(01001100)},
    {BINARY_U8(01001101), BINARY_U8(01001111)},
    {BINARY_U8(01010000), BINARY_U8(01001101)},
    {BINARY_U8(01010010), BINARY_U8(01010000)},
    {BINARY_U8(01010001), BINARY_U8(01010011)},
    {BINARY_U8(01010100), BINARY_U8(01010001)},
    {BINARY_U8(01010110), BINARY_U8(01010100)},
    {BINARY_U8(01010101), BINARY_U8(01010111)},
    {BINARY_U8(01011000), BINARY_U8(01010101)},
    {BINARY_U8(01011010), BINARY_U8(01011000)},
    {BINARY_U8(01011001), BINARY_U8(01011011)},
    {BINARY_U8(01011100), BINARY_U8(01011001)}
};


namespace FPGA
{
    // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
    void LoadSettings();

    // Загрузить все параметры напряжения каналов и синхронизации в аппаратную часть.
    void SetAttribChannelsAndTrig(TypeWriteAnalog::E type);

    // Загрузить регистр WR_UPR (пиковый детектор и калибратор).
    void LoadRegUPR();

    void WriteToAnalog(TypeWriteAnalog::E type, uint data);

    void WriteToDAC(TypeWriteDAC::E type, uint16 data);
}


void FPGA::LoadSettings()
{
    LoadKoeffCalibration(Chan::A);
    LoadKoeffCalibration(Chan::B);
    SetAttribChannelsAndTrig(TypeWriteAnalog::All);
    TBase::Load();
    TShift::Load();
    Range::Load(Chan::A);
    RShift::Load(Chan::A);
    Range::Load(Chan::B);
    RShift::Load(Chan::B);
    TrigLev::Load();
    TrigPolarity::Load();
    LoadRegUPR();

    switch(BALANCE_ADC_TYPE) 
    {
        case BalanceADC_Settings:
            WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)SET_BALANCE_ADC_A, false);
            WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)SET_BALANCE_ADC_B, false);
            break;
        case BalanceADC_Hand:
            PeackDetMode::Set(PEAKDET);
            TBase::Set(SET_TBASE);
            if (PEAKDET)
            {
                WriteToHardware(WR_ADD_RSHIFT_DAC1, 3, false);     // Почему-то при пиковом детекторе смещение появляется. Вот его и компенсируем.
                WriteToHardware(WR_ADD_RSHIFT_DAC2, 3, false);
            }
            else
            {
                WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)BALANCE_ADC_A, false);
                WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)BALANCE_ADC_B, false);
            }
            break;
        case BalanceADC_Disable:
            break;
    }
}


void FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::E type) 
{
    uint data = 0;

    /*
    b0...b7 - Channel1
    b8...b15 - Channel2
    b16...b23 - Trig
    Передаваться биты должны начиная с b0
    */

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

    WriteToAnalog(type, data);
}


void Range::Set(Chan::E ch, Range::E range)
{
    if (!sChannel_Enabled(ch))
    {
        return;
    }
    if (range < Range::Count && (int)range >= 0)
    {
        float rShiftAbs = RSHIFT_2_ABS(SET_RSHIFT(ch), SET_RANGE(ch));
        float trigLevAbs = RSHIFT_2_ABS(TRIG_LEVEL(ch), SET_RANGE(ch));
        sChannel_SetRange(ch, range);

        if (LINKING_RSHIFT_IS_VOLTAGE)
        {
            SET_RSHIFT(ch) = (int16)Math_RShift2Rel(rShiftAbs, range);
            TRIG_LEVEL(ch) = (int16)Math_RShift2Rel(trigLevAbs, range);
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
    FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog::Range0);
    RShift::Load(ch);

    if (ch == (Chan::E)TRIG_SOURCE)
    {
        TrigLev::Load();
    }
}


void TBase::Set(TBase::E tBase)
{
    if (!sChannel_Enabled(Chan::A) && !sChannel_Enabled(Chan::B))
    {
        return;
    }
    if (tBase < TBase::Count && (int)tBase >= 0)
    {
        float tShiftAbsOld = TSHIFT_2_ABS(TSHIFT, SET_TBASE);
        sTime_SetTBase(tBase);
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
    TBase::E tBase = SET_TBASE;
    uint8 mask = PEAKDET ? masksTBase[tBase].maskPeackDet : masksTBase[tBase].maskNorm;
    FPGA::WriteToHardware(WR_RAZVERTKA, mask, true);
    ADD_SHIFT_T0 = deltaTShift[tBase];
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
        if (SET_SELFRECORDER && SET_TBASE == MIN_TBASE_P2P)
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
    if (!sChannel_Enabled(ch))
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
    FPGA::WriteToDAC(ch == Chan::A ? TypeWriteDAC::RShiftA : TypeWriteDAC::RShiftB, (uint16)(mask[ch] | (rShift << 2)));
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
//    FPGA::WriteToHardware(WR_DAC_LOW, data.byte[0], true);
//    FPGA::WriteToHardware(WR_DAC_HI, data.byte[1], true);
    FPGA::WriteToDAC(TypeWriteDAC::TrigLev, data);
}


void TShift::Set(int tShift)
{
    if (!sChannel_Enabled(Chan::A) && !sChannel_Enabled(Chan::B))
    {
        return;
    }

    if (tShift < sTime_TShiftMin() || tShift > TShift::MAX)
    {
        LIMITATION(tShift, tShift, sTime_TShiftMin(), TShift::MAX);
        Display::ShowWarningBad(LimitSweep_TShift);
    }

    sTime_SetTShift((int16)tShift);
    Load();
    Display::Redraw();
};


void TShift::SetDelta(int16 shift)
{
    deltaTShift[SET_TBASE] = shift;
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
    if (sTime_RandomizeModeEnabled())
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

    FPGA::WriteToHardware(WR_UPR, data, true);
}


void FPGA::LoadKoeffCalibration(Chan::E ch)
{
    FPGA::WriteToHardware(ch == Chan::A ? WR_CAL_A : WR_CAL_B, STRETCH_ADC(ch) * 0x80, false);
}


void TShift::Load()
{
    static const int16 k[TBase::Count] = {50, 20, 10, 5, 2};
    int16 tShift = TSHIFT - sTime_TShiftMin() + 1;
    int16 tShiftOld = tShift;
    TBase::E tBase = SET_TBASE;
    if (tBase < TBase::_100ns)
    {
        tShift = tShift / k[tBase] + deltaTShift[tBase];
    }
    int additionShift = (tShiftOld % k[tBase]) * 2;
    FPGA::SetAdditionShift(additionShift);
    uint16 post = (uint16)tShift;
    post = (uint16)(~post);
    FPGA::WriteToHardware(WR_POST_LOW, (uint8)post, true);
    FPGA::WriteToHardware(WR_POST_HI, (uint8)(post >> 8), true);
    uint16 pred = (uint16)((tShift > 511) ? 1023 : (511 - post));
    pred = (uint16)((~(pred - 1)) & 0x1ff);
    FPGA::WriteToHardware(WR_PRED_LOW, (uint8)pred, true);
    FPGA::WriteToHardware(WR_PRED_HI, (uint8)(pred >> 8), true);
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
    FPGA::WriteToHardware(WR_TRIG_F, TRIG_POLARITY_IS_FRONT ? 0x01U : 0x00U, true);
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
