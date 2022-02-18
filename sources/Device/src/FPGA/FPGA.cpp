// 2022/02/18 15:30:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"



namespace FPGA
{
    void WriteToDAC(TypeWriteDAC::E, uint16);

    void WriteToAnalog(TypeWriteAnalog::E type, uint data);
}



float FPGA::FreqMeter::GetFreq()
{
    return 0.0f;
}


void FPGA::Init()
{

}


void FPGA::Update()
{

}


void FPGA::Start()
{

}


bool FPGA::IsRunning()
{
    return false;
}


void FPGA::Stop(bool)
{

}


void FPGA::OnPressStartStop()
{

}


void FPGA::SetAdditionShift(int)
{

}


void FPGA::SetNumSignalsInSec(int)
{

}


void FPGA::FindAndSetTrigLevel()
{

}


void FPGA::TemporaryPause()
{

}


void FPGA::StartAutoFind()
{

}


void FPGA::SetNumberMeasuresForGates(int)
{

}


void FPGA::WriteToDAC(TypeWriteDAC::E, uint16)
{

}


void FPGA::WriteToAnalog(TypeWriteAnalog::E, uint)
{

}


void FPGA::WriteToHardware(uint8 *, uint8, bool)
{

}


void FPGA::WriteToHardware(uint16 *, uint16, bool)
{

}
