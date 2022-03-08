// 2022/02/18 15:30:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Panel/Panel.h"
#include "Hardware/Timer.h"
#include "Display/Text.h"
#include <stm32f4xx_hal.h>


namespace FPGA
{
    namespace Calibrator
    {
        struct StateCalibration
        {
            enum E
            {
                Start,
                ErrorChannelA,
                ErrorChannelB,
                Finish
            };
        };

        static StateCalibration::E state = StateCalibration::Start;

        static void FunctionDraw();
    }
}


void FPGA::Calibrator::RunCalibrate()
{
    Display::SetDrawMode(DrawMode::Hand, FunctionDraw);


    Display::SetDrawMode(DrawMode::Auto);
}


static void FPGA::Calibrator::FunctionDraw()
{

}
