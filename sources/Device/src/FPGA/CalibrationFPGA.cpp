// 2022/02/18 15:30:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Panel/Panel.h"
#include "Hardware/Timer.h"
#include "Display/Text.h"
#include "Utils/Containers/String.h"
#include "Utils/Containers/Buffer.h"
#include "Menu/Pages/Definition.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Containers/Queue.h"
#include <stm32f4xx_hal.h>
#include <cstring>


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

            void Draw(int y)
            {
                value = ((TIME_MS - timeStart) / 10) % width;

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
                Finish
            };
        };

        static StateCalibration::E state = StateCalibration::WaitA;

        static Progress progress;

        static bool errorCalibration[Chan::Count];      // Если true - произошла ошибка в процессе калибровки
        static bool carriedOut[Chan::Count];            // Если true - калибровка выполнялась

        // Функция отрисовка
        static void FunctionDraw();

        static void CalibrateChannel(Chan);

        // Калибровать смещение канала
        static bool CalibrateRShift(Chan);

        // Калибровать растяжку канала
        static bool CalibrateStretch(Chan);

        // Изобразить результат калибровки
        static void DrawResultCalibration(int x, int y, Chan);

        // Читает 1024 точки и возвращает их среднее значение
        static float Read1024PointsAve(Chan);

        static void Read1024Points(uint8 buffer[1024], Chan);

        static int CalculateAddRShift(float ave);

        // Читает 1024 точки и возвращает минимальное и максимальное значения
        static void Read1024PointsMinMax(Chan, float *min, float *max);
    }

    // Принудительно запустить синхронизацию.
    void SwitchingTrig();
}


void FPGA::Calibrator::RunCalibrate()
{
    bool isRunning = FPGA::IsRunning();

    FPGA::Stop(false);

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

    state = StateCalibration::Finish;

    Panel::WaitPressingButton();

    Panel::Enable();

    Display::SetDrawMode(DrawMode::Auto);

    if (isRunning)
    {
        FPGA::Start();
    }
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
            String message(LANG_RU ? "Подключите ко входу канала %d выход калибратора и нажмите кнопку ПУСК/СТОП. Если вы не хотите калибровать первый канала, нажмите любую другую кнопку." :
                "Connect the output of the calibrator to channel %d input and press the START/STOP button. If you do not want to calibrate the first channel, press any other button.",
                (state == StateCalibration::WaitA) ? 1 : 2);

            PText::DrawInRect(50, 80, SCREEN_WIDTH - 100, SCREEN_HEIGHT, message.c_str(), 2);
        }
        break;

    case StateCalibration::RShiftA:
    case StateCalibration::RShiftB:
        {
            String message(LANG_RU ? "Калибрую параметр 1 канала %d" : "Calibrate parameter 1 channel %d", (state == StateCalibration::RShiftA) ? 1 : 2);

            PText::Draw(85, 50, message.c_str());

            progress.Draw(100);
        }
        break;

    case StateCalibration::StretchA:
    case StateCalibration::StretchB:
        {
            String message(LANG_RU ? "Калибрую параметр 2 канала %d" : "Calibrate parameter 2 channel %d", (state == StateCalibration::StretchA) ? 1 : 2);

            PText::Draw(85, 50, message.c_str());

            progress.Draw(100);
        }
        break;

    case StateCalibration::Finish:
        {
            int y1 = 70;
            int y2 = 100;

            PText::Draw(80, y1, LANG_RU ? "Канал 1 :" : "Channel 1 :");
            PText::Draw(80, y2, LANG_RU ? "Канал 2 :" : "Channel 2 :");

            int x = 125;

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
    state = ch.IsA() ? StateCalibration::RShiftA : StateCalibration::RShiftB;

    progress.Reset();

    PageDebug::PageADC::ResetCalRShift(ch);

    PageDebug::PageADC::ResetCalStretch(ch);

    RShift::Set(ch, RShift::ZERO);
    TBase::Set(TBase::_200us);
    TShift::Set(0);
    TrigSource::Set((TrigSource::E)ch.value);
    TrigPolarity::Set(TrigPolarity::Front);
    TrigLev::Set((TrigSource::E)ch.value, TrigLev::ZERO);
    PeackDetMode::Set(PeackDetMode::Disable);

    CalibratorMode::Set(CalibratorMode::GND);

    for (int range = 0; range < Range::Count; range++)
    {
        for (int couple = 0; couple < ModeCouple::Count; couple++)
        {
            Range::Set(ch, (Range::E)range);
            ModeCouple::Set(ch, (ModeCouple::E)couple);

            float ave = Read1024PointsAve(ch);

            int addShift = CalculateAddRShift(ave);

            if (addShift < -50 || addShift > 50)
            {
                return false;
            }

            CAL_RSHIFT(ch) = (int8)addShift;
        }
    }

    return true;
}


static bool FPGA::Calibrator::CalibrateStretch(Chan ch)
{
    state = ch.IsA() ? StateCalibration::StretchA : StateCalibration::StretchB;

    progress.Reset();

    ModeCouple::Set(ch, ModeCouple::AC);
    Range::Set(ch, Range::_500mV);
    RShift::Set(ch, RShift::ZERO);
    TBase::Set(TBase::_200us);
    TShift::Set(0);
    TrigSource::Set((TrigSource::E)ch.value);
    TrigPolarity::Set(TrigPolarity::Front);
    TrigLev::Set((TrigSource::E)ch.value, TrigLev::ZERO);
    PeackDetMode::Set(PeackDetMode::Disable);

    CalibratorMode::Set(CalibratorMode::Freq);

    float min = 0.0f;
    float max = 0.0f;

    Read1024PointsMinMax(ch, &min, &max);

    return true;
}


static void FPGA::Calibrator::DrawResultCalibration(int x, int y, Chan ch)
{
    Color::SetCurrent(COLOR_FILL);

    String message;

    if (!carriedOut[ch])
    {
        message.SetFormat(LANG_RU ? "Калибровка не выполнялась." : "Calibration not implemented.");
    }
    else
    {
        if (errorCalibration[ch])
        {
            Color::SetCurrent(Color::FLASH_01);

            message.SetFormat(LANG_RU ? "!!! Ошибка калибровки !!!" : "!!! Calibration error !!!");
        }
        else
        {
            message.SetFormat(LANG_RU ? "Калибровка прошла успешно." : "Calibration successful.");
        }
    }

    PText::Draw(x, y, message.c_str());
}


float FPGA::Calibrator::Read1024PointsAve(Chan ch)
{
    uint8 buffer[1024];

    Read1024Points(buffer, ch);

    return Buffer<uint8>::Sum(buffer, 1024) / 1024;
}


void FPGA::Calibrator::Read1024Points(uint8 buffer[1024], Chan ch)
{
    Timer::PauseOnTime((SET_RANGE(ch) < 2) ? 500U : 100U);

    FPGA::Start();

    std::memset(buffer, 255, 1024);

    Timer::PauseOnTime(8);

    uint16 fl = HAL_FMC::Read(RD_FL);

    while (_GET_BIT(fl, FL_PRED) == 0) { fl = HAL_FMC::Read(RD_FL); }

    FPGA::SwitchingTrig();

    while (_GET_BIT(fl, FL_TRIG) == 0) { fl = HAL_FMC::Read(RD_FL); }

    Timer::PauseOnTime(8);

    while (_GET_BIT(fl, FL_DATA) == 0) { fl = HAL_FMC::Read(RD_FL); }

    FPGA::Stop(false);

    FPGA::Reader::ReadPoints(ch, buffer, &buffer[0] + 1024);
}


int FPGA::Calibrator::CalculateAddRShift(float ave)
{
    return (int)((ValueFPGA::AVE - ave) * 2);
}


void FPGA::Calibrator::Read1024PointsMinMax(Chan ch, float *min, float *max)
{
    uint8 buffer[1024];

    Read1024Points(buffer, ch);

    Queue<float> mins;
    Queue<float> maxs;

    for (int i = 0; i < 1024; i++)
    {
        if (buffer[i] > 200)
        {
            maxs.Push(buffer[i]);
        }
        else if (buffer[i] < 50)
        {
            mins.Push(buffer[i]);
        }
    }

    *min = 0.0f;

    for (int i = 0; i < mins.Size(); i++)
    {
        *min += (float)mins[(int)i];
    }

    *min = *min / mins.Size();

    *max = 0.0f;

    for (int i = 0; i < maxs.Size(); i++)
    {
        *max += (float)maxs[(int)i];
    }

    *max = *max / maxs.Size();
}
