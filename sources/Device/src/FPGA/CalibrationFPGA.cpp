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

        static StateCalibration::E state = StateCalibration::WaitA;

        static bool errorCalibration[Chan::Count];

        static void FunctionDraw();

        static void CalibrateChannel(Chan::E);

        static bool CalibrateRShift(Chan::E);

        static bool CalibrateStretch(Chan::E);
    }
}


void FPGA::Calibrator::RunCalibrate()
{
    Display::SetDrawMode(DrawMode::Hand, FunctionDraw);

    Panel::Disable();

    CalibrateChannel(ChA);

    CalibrateChannel(ChB);

    if (errorCalibration[ChA] || errorCalibration[ChB])
    {
        state = StateCalibration::Error;

        Panel::WaitPressingButton();
    }

    Panel::Enable();

    Display::SetDrawMode(DrawMode::Auto);
}


static void FPGA::Calibrator::FunctionDraw()
{
    Painter::BeginScene(COLOR_BACK);

    switch (state)
    {
    case StateCalibration::WaitA:
        PText::DrawInRect(50, 25, SCREEN_WIDTH - 100, SCREEN_HEIGHT, "Подключите ко входу канала 1 выход калибратора и нажмите кнопку ПУСК/СТОП. \
                                                                         Если вы не хотите калибровать первый канала, нажмите любую другую кнопку.");
        break;

    case StateCalibration::WaitB:
        PText::DrawInRect(50, 25, SCREEN_WIDTH - 100, SCREEN_HEIGHT, "Подключите ко входу канала 2 выход калибратора и нажмите кнопку ПУСК/СТОП. \
                                                                         Если вы не хотите калибровать первый канала, нажмите любую другую кнопку.");
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


static void FPGA::Calibrator::CalibrateChannel(Chan::E ch)
{
    errorCalibration[ch] = false;

    if (CalibrateRShift(ch))
    {
        if (CalibrateStretch(ch))
        {
            return;
        }
    }

    errorCalibration[ch] = true;
}


static bool FPGA::Calibrator::CalibrateRShift(Chan::E)
{
    return false;
}


static bool FPGA::Calibrator::CalibrateStretch(Chan::E)
{
    return false;
}
