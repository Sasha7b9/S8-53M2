#include "defines.h"
#include "Settings/SettingsNRST.h"
#include "Hardware/HAL/HAL.h"


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
