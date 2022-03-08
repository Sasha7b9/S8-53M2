// 2022/02/18 15:30:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Panel/Panel.h"
#include "Hardware/Timer.h"
#include "Display/Text.h"
#include "Utils/Containers/String.h"
#include <stm32f4xx_hal.h>


namespace FPGA
{
    namespace Calibrator
    {
        struct Progress
        {
            static const int width = 200;
            static const int height = 20;

            float value;

            uint timeStart;

            Progress() : value(0.0f), timeStart(0) {}

            void Reset() { value = 0.0f; timeStart = TIME_MS; }

            void Update()
            {
                value = ((TIME_MS - timeStart) / 10) % width;
            }

            void Draw(int y)
            {
                int x = (SCREEN_WIDTH - width) / 2;

                Painter::DrawRectangle(x, y, width, height, COLOR_FILL);
                Painter::FillRegion(x, y, value, height);
            }
        };

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
                Error          // � ��� ��������� ��������� ��� ���� �� ����� ������ ����������
            };
        };

        static StateCalibration::E state = StateCalibration::WaitA;

        static Progress progress;

        static bool errorCalibration[Chan::Count];      // ���� true - ��������� ������ � �������� ����������
        static bool carriedOut[Chan::Count];            // ���� true - ���������� �����������

        // ������� ���������
        static void FunctionDraw();

        static void CalibrateChannel(Chan);

        // ����������� �������� ������
        static bool CalibrateRShift(Chan);

        // ����������� �������� ������
        static bool CalibrateStretch(Chan);

        // ���������� ��������� ����������
        static void DrawResultCalibration(int x, int y, Chan);
    }
}


void FPGA::Calibrator::RunCalibrate()
{
    Display::SetDrawMode(DrawMode::Hand, FunctionDraw);

    Panel::Disable();

    {
        state = StateCalibration::WaitA;
        errorCalibration[ChA] = false;
        carriedOut[ChA] = false;

        if (Panel::WaitPressingButton() == Key::Start)
        {
            carriedOut[ChA] = true;

            CalibrateChannel(ChA);
        }
    }

    {
        state = StateCalibration::WaitB;
        errorCalibration[ChB] = false;
        carriedOut[ChB] = false;

        if (Panel::WaitPressingButton() == Key::Start)
        {
            carriedOut[ChB] = true;

            CalibrateChannel(ChB);
        }
    }

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

    Color::SetCurrent(COLOR_FILL);

    switch (state)
    {
    case StateCalibration::WaitA:
    case StateCalibration::WaitB:
        {
            String message(LANG_RU ? "���������� �� ����� ������ %d ����� ����������� � ������� ������ ����/����. ���� �� �� ������ ����������� ������ ������, ������� ����� ������ ������." :
                "Connect the output of the calibrator to channel %d input and press the START/STOP button. If you do not want to calibrate the first channel, press any other button.",
                (state == StateCalibration::WaitA) ? 1 : 2);

            PText::DrawInRect(50, 80, SCREEN_WIDTH - 100, SCREEN_HEIGHT, message.c_str(), 2);
        }
        break;

    case StateCalibration::RShiftA:
    case StateCalibration::RShiftB:
        {
            String message(LANG_RU ? "�������� �������� 1 ������ %d" : "Calibrate parameter 1 channel %d", (state == StateCalibration::RShiftA) ? 1 : 2);

            PText::Draw(85, 50, message.c_str());

            progress.Draw(100);
        }
        break;

    case StateCalibration::StretchA:
    case StateCalibration::StretchB:
        {
            String message(LANG_RU ? "�������� �������� 2 ������ %d" : "Calibrate parameter 2 channel %d", (state == StateCalibration::StretchA) ? 1 : 2);

            PText::Draw(85, 50, message.c_str());

            progress.Draw(100);
        }
        break;

    case StateCalibration::Error:
        {
            int y1 = 100;
            int y2 = 130;

            PText::Draw(50, y1, LANG_RU ? "����� 1" : "Channel 1");
            PText::Draw(50, y2, LANG_RU ? "����� 2" : "Channel 2");

            int x = 200;

            DrawResultCalibration(x, y1, ChA);

            DrawResultCalibration(x, y2, ChB);
        }
        break;
    }

    Painter::EndScene();
}


static void FPGA::Calibrator::CalibrateChannel(Chan ch)
{
    if (CalibrateRShift(ch))
    {
        if (CalibrateStretch(ch))
        {
            return;
        }
    }

    errorCalibration[ch] = true;
}


static bool FPGA::Calibrator::CalibrateRShift(Chan ch)
{
    bool result = true;

    state = ch.IsA() ? StateCalibration::RShiftA : StateCalibration::RShiftB;

    progress.Reset();

    while (TIME_MS - progress.timeStart < 1000)
    {
        progress.Update();
    }

    return result;
}


static bool FPGA::Calibrator::CalibrateStretch(Chan ch)
{
    bool result = true;

    state = ch.IsA() ? StateCalibration::StretchA : StateCalibration::StretchB;

    progress.Reset();

    while (TIME_MS - progress.timeStart < 1000)
    {
        progress.Update();
    }

    return result;
}


static void FPGA::Calibrator::DrawResultCalibration(int x, int y, Chan ch)
{
    Color::SetCurrent(COLOR_FILL);

    String message;

    if (!carriedOut[ch])
    {
        message.SetFormat(LANG_RU ? "���������� �� �����������." : "Calibration not implemented.");
    }
    else
    {
        if (errorCalibration[ch])
        {
            Color::SetCurrent(Color::FLASH_01);

            message.SetFormat(LANG_RU ? "!!! ������ ���������� !!!" : "!!! Calibration error !!!");
        }
        else
        {
            message.SetFormat(LANG_RU ? "���������� ������ �������." : "Calibration successful.");
        }
    }

    PText::Draw(x, y, message.c_str());
}
