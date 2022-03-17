#include "defines.h"
#include "GlobalEvents.h"
#include "FPGA/FPGA.h"
#include "Data/Storage.h"



namespace Event
{
    static void OnChangedSettingsRandomizer();
    static void OnChangedSettingsReal();
    static void OnChangedSettingsP2P();
}


void Event::OnChnagedSetting()
{
    bool isRunning = FPGA::IsRunning();

    if (isRunning)
    {
        FPGA::Stop(false);
    }

    if (TBase::InModeP2P())
    {
        OnChangedSettingsP2P();
    }
    else if (TBase::InModeRandomizer())
    {
        OnChangedSettingsRandomizer();
    }
    else
    {
        OnChangedSettingsReal();
    }

    if (isRunning)
    {
        FPGA::Start();
    }
}


void Event::OnChangedSettingsRandomizer()
{

}


void Event::OnChangedSettingsReal()
{

}


void Event::OnChangedSettingsP2P()
{
    DataSettings *ds = Storage::GetDataSettings(0);

    if (ds)
    {
        ds->ResetP2P();
    }
}
