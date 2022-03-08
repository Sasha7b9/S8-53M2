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
                WaitA,
                RShiftA,
                StretchA,
                WaitB,
                RShiftB,
                StretchB,
                Error          // В это состояние переходим при хотя бы одной ошибке калибровки
            };
        };

        static StateCalibration::E state = StateCalibration::Start;

        static bool errorA = false;
        static bool errorB = false;

        static void FunctionDraw();
    }
}


void FPGA::Calibrator::RunCalibrate()
{
    Display::SetDrawMode(DrawMode::Hand, FunctionDraw);

    errorA = false;
    errorB = false;

    if (errorA || errorB)
    {
        state = StateCalibration::Error;
    }

    Display::SetDrawMode(DrawMode::Auto);
}


static void FPGA::Calibrator::FunctionDraw()
{
    Painter::BeginScene(COLOR_BACK);

    switch (state)
    {
    case StateCalibration::WaitA:
    case StateCalibration::WaitB:
        break;

    case StateCalibration::RShiftA:
    case StateCalibration::RShiftB:
        break;

    case StateCalibration::StretchA:
    case StateCalibration::StretchB:
        break;

    case StateCalibration::Error:
        break;
    }

    Painter::EndScene();
}
