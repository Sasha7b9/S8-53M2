// 2022/02/18 15:30:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Panel/Panel.h"
#include "Hardware/Timer.h"
#include "Menu/Pages/Definition.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Containers/Queue.h"
#include "Display/Screen/Console.h"
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
            float max;

            uint timeStart;

            Progress() : value(0.0f), max(0.0f), timeStart(0) {}

            void Reset(float _max) { max = _max; value = 0; }

            void SetValue(float _value)
            {
                value = _value;
            }

            void Draw(int y)
            {
                int x = (SCREEN_WIDTH - width) / 2;

                Rectangle(width, height).Draw(x, y, COLOR_FILL);
                Region(width * value / max, height).Fill(x, y);
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

        static bool errorCalibration[Chan::Count];      // ���� true - ��������� ������ � �������� ����������
        static bool carriedOut[Chan::Count];            // ���� true - ���������� �����������

        // ������� ���������
        static void FunctionDraw();

        // ������� ���������� � ��������� ������������� �������������
        void ShowCalibrationInfo(const int y, Chan);

        static bool CalibrateChannel(Chan);

        // ����������� �������� ������
        static bool CalibrateRShift(Chan);

        // ����������� �������� ������
        static bool CalibrateStretch(Chan);

        // ���������� ��������� ����������
        static void DrawResultCalibration(int x, int y, Chan);

        // ������ 1024 ����� � ���������� �� ������� ��������
        static float Read1024PointsAve(Chan);

        // ������ 1024 ����� � ���������� ����������� � ������������ ��������
        static void Read1024PointsMinMax(Chan, float *min, float *max);

        static int CalculateAddRShift(float ave);

        // ���������� ����������� �������� ������ �� ���������� min � max
        static float CalculateStretch(float min, float max);

        // ���������� ��������� ��������� ���������� �� src � dest
        static void CopyCalibrationSettings(Chan ch, Settings &dest, const Settings &src);
    }

    // ������������� ��������� �������������.
    void SwitchingTrig();
}


void FPGA::Calibrator::CopyCalibrationSettings(Chan ch, Settings &dest, const Settings &src)
{
    dest.chan[ch].cal_stretch = src.chan[ch].cal_stretch;

    std::memcpy(&dest.debug.cal_rshift[ch][0][0], &src.debug.cal_rshift[ch][0][0],
        sizeof(src.debug.cal_rshift[0][0][0]) * Chan::Count * Range::Count * ModeCouple::Count);
}


void FPGA::Calibrator::RunCalibrate()
{
    bool isRunning = FPGA::IsRunning();

    FPGA::Stop();

    Display::SetDrawMode(DrawMode::Timer, FunctionDraw);

    Settings old = set;

    Panel::Disable();

    {
        state = StateCalibration::WaitA;
        errorCalibration[ChA] = false;
        carriedOut[ChA] = false;

        Panel::WaitPressingButton();

        carriedOut[ChA] = true;
        errorCalibration[ChA] = !CalibrateChannel(ChA);
    }

    {
        state = StateCalibration::WaitB;
        errorCalibration[ChB] = false;
        carriedOut[ChB] = false;

        Panel::WaitPressingButton();

        carriedOut[ChB] = true;
        errorCalibration[ChB] = !CalibrateChannel(ChB);
    }

    Settings set_cal = set;                     // ��������� �������������� ���������

    set = old;                                  // ���������� ���������, ������� ���� �� ����������

    if (carriedOut[ChA] && !errorCalibration[ChA])
    {
        CopyCalibrationSettings(ChA, set, set_cal);
    }
    if (carriedOut[ChB] && !errorCalibration[ChB])
    {
        CopyCalibrationSettings(ChB, set, set_cal);
    }

    state = StateCalibration::Finish;

    Panel::WaitPressingButton();

    Panel::Enable();

    Display::SetDrawMode(DrawMode::Normal);

    FPGA::Init();

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
            String<>(LANG_RU ? "���������� �� ����� ������ %d ����� ����������� � ������� ������ ����/����." :
                "Connect the output of the calibrator to channel %d input and press the START/STOP button.",
                (state == StateCalibration::WaitA) ? 1 : 2)
                .DrawInRect(50, 80, SCREEN_WIDTH - 100, SCREEN_HEIGHT, 2);
        }
        break;

    case StateCalibration::RShiftA:
    case StateCalibration::RShiftB:
    case StateCalibration::StretchA:
    case StateCalibration::StretchB:
        {
            String<>(LANG_RU ? "��� ���������� ������ %d" : "Channel %d calibration in progress",
                (state == StateCalibration::RShiftA) ? 1 : 2).Draw(85, 50);

            progress.Draw(100);
        }
        break;

    case StateCalibration::Finish:
        {
            int y1 = 70;
            int y2 = 100;

            String<>(LANG_RU ? "����� 1 :" : "Channel 1 :").Draw(80, y1);
            String<>(LANG_RU ? "����� 2 :" : "Channel 2 :").Draw(80, y2);

            int x = 125;

            DrawResultCalibration(x, y1, ChA);

            DrawResultCalibration(x, y2, ChB);

//            ShowCalibrationInfo(130, ChA);

//            ShowCalibrationInfo(185, ChB);

            String<>(LANG_RU ? "������� ����� ������" : "Press any button").DrawInCenterRect(0, 190, 320, 10, COLOR_FILL);
        }
        break;
    }

    Console::Draw();

    Painter::EndScene();
}


static bool FPGA::Calibrator::CalibrateChannel(Chan ch)
{
    if (CalibrateRShift(ch))
    {
        if (CalibrateStretch(ch))
        {
            return true;;
        }
    }

    return false;
}


static bool FPGA::Calibrator::CalibrateRShift(Chan ch)
{
    state = ch.IsA() ? StateCalibration::RShiftA : StateCalibration::RShiftB;

    progress.Reset(Range::Count * ModeCouple::Count);

    PageDebug::ADC::ResetCalRShift(ch);

    PageDebug::ADC::ResetCalStretch(ch);

    RShift::Set(ch, RShift::ZERO);
    TBase::Set(TBase::_200us);
    TShift::Set(0);
    TrigSource::Set((TrigSource::E)ch.value);
    TrigPolarity::Set(TrigPolarity::Front);
    TrigLev::Set((TrigSource::E)ch.value, TrigLev::ZERO);
    PeackDetMode::Set(PeackDetMode::Disable);

    CalibratorMode::Set(CalibratorMode::GND);

    int counter = 0;

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

            progress.SetValue((float)++counter);
        }
    }

    return true;
}


static bool FPGA::Calibrator::CalibrateStretch(Chan ch)
{
    state = ch.IsA() ? StateCalibration::StretchA : StateCalibration::StretchB;

    progress.Reset(1);

    PageDebug::ADC::ResetCalStretch(ch);

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

    float stretch = CalculateStretch(min, max);

    if (stretch < 0.8f || stretch > 1.2f)
    {
        return false;
    }

    CAL_STRETCH(ch) = stretch;

    progress.SetValue(1);

    return true;
}


float FPGA::Calibrator::CalculateStretch(float min, float max)
{
    const float K = 200.0f;

    float delta = max - min;

    return K / delta;
}


static void FPGA::Calibrator::DrawResultCalibration(int x, int y, Chan ch)
{
    Color::SetCurrent(COLOR_FILL);

    String<> message;

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

    message.Draw(x, y);
}


float FPGA::Calibrator::Read1024PointsAve(Chan ch)
{
    uint8 buffer[1024];

    Reader::Read1024Points(buffer, ch);

    float sum = 0.0f;

    for (int i = 0; i < 1024; i++)
    {
        sum += (float)buffer[i];
    }

    return sum / 1024.0f;
}


int FPGA::Calibrator::CalculateAddRShift(float ave)
{
    return (int)((ValueFPGA::AVE - ave) * 2);
}


void FPGA::Calibrator::Read1024PointsMinMax(Chan ch, float *min, float *max)
{
    uint8 buffer[1024];

    Reader::Read1024Points(buffer, ch);

    Buffer<float, 1024> mins;
    int num_mins = 0;

    Buffer<float, 1024> maxs;
    int num_maxs = 0;

    for (int i = 0; i < 1024; i++)
    {
        if (buffer[i] > 200)
        {
            maxs[num_maxs++] = buffer[i];
        }
        else if (buffer[i] < 50)
        {
            mins[num_mins++] = buffer[i];
        }
    }

    *min = 0.0f;

    for (int i = 0; i < num_mins; i++)
    {
        *min += (float)mins[(int)i];
    }

    *min = *min / num_mins;

    *max = 0.0f;

    for (int i = 0; i < num_maxs; i++)
    {
        *max += (float)maxs[(int)i];
    }

    *max = *max / num_maxs;
}


void FPGA::Calibrator::ShowCalibrationInfo(const int y0, Chan ch)
{
    const int x0 = 30;
    const int dX = 20;
    const int dY = 12;

    Color::SetCurrent(COLOR_FILL);

    for (int couple = 0; couple < ModeCouple::Count; couple++)
    {
        for (int range = 0; range < Range::Count; range++)
        {
            String<>("%d", set.debug.cal_rshift[ch][range][couple]).Draw(x0 + range * dX, y0 + couple * dY);
        }
    }

    String<>("%f", CAL_STRETCH(ch)).Draw(x0, y0 + dY * ModeCouple::Count, COLOR_FILL);
}
