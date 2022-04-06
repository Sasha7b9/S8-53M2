#include "defines.h"
#include "Settings/SettingsNRST.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"


static SettingsNRST defaultNRST =
{
    0,                  // crc32
    15,                 // numStrings
    0,                  // размер шрифта - 5
    1000,               // numMeasuresForGates
    false,              // showStats
    6,                  // numAveForRand
    false,              // fpga_compact
    0,                  // fpga_gates_min
    0,                  // fpga_gates_max
    0,                  // first_byte
    {0,    0},          // balanceADC
    {                   // hand_rshift
        {0, 0, 0},
        {0, 0, 0}
    },
    0                   // not_used
};

int16 SettingsNRST::stored_rshift[Chan::Count][3];


SettingsNRST nrst = defaultNRST;


void SettingsNRST::Save()
{
    HAL_ROM::Settings::SaveNRST(&nrst);
}


void SettingsNRST::Load()
{
    if (!HAL_ROM::Settings::LoadNRST(&nrst))
    {
        LOG_WRITE("Не загружены отладочные настройки");
    }
}


int CAL::RShift(Chan ch, Range::E range)
{
    int result = set.chan[ch].cal_rshift[range][SET_COUPLE(ch)];

    if (range <= Range::_10mV && SET_COUPLE(ch) == ModeCouple::DC)
    {
        result += RSHIFT_HAND(ch, range);
    }

    return result;
}


void SettingsNRST::ClearHandRShift(Chan ch)
{
    std::memset(&hand_rshift[ch][0], 0, sizeof(hand_rshift[0][0]) * 3);
}


void SettingsNRST::StoreAndClearHandRShift(Chan ch, Range::E range)
{
    if ((ch.IsA() || ch.IsB()) && range <= Range::_10mV)
    {
        stored_rshift[ch][range] = hand_rshift[ch][range];
        hand_rshift[ch][range] = 0;
    }
    else
    {
        LOG_ERROR_TRACE("ошибка диапазона");
    }
}


void SettingsNRST::RestoreHandRShift(Chan ch, Range::E range)
{
    if ((ch.IsA() || ch.IsB()) && range <= Range::_10mV)
    {
        hand_rshift[ch][range] = stored_rshift[ch][range];
    }
    else
    {
        LOG_ERROR_TRACE("ошибка диапазона");
    }
}
