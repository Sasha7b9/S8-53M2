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
        struct ProgressBar
        {
            int x;
            int y;
            int width;
            int height;
            float fullTime;
            float passedTime;

            void Draw() const;
        };


        struct CalibrationStruct
        {
            float deltaADC[2];
            float deltaADCPercents[2];
            float avrADC1[2];
            float avrADC2[2];

            float deltaADCold[2];
            float avrADC1old[2];
            float avrADC2old[2];

            bool isCalculateStretch[2];

            int8 shiftADCA;
            int8 shiftADCB;

            ProgressBar barA;       // Прогресс-бар для калибровки первого канала.
            ProgressBar barB;       // Прогресс-бар для калибровки второго канала.

            uint startTimeChanA;    // Время начала калибровки первого канала.
            uint startTimeChanB;    // Время начала калибровки второго канала.
        };

        static CalibrationStruct cal_struct;

        // Функция обновления экрана в режиме калибровки.
        static void FuncAttScreen();

        static void AlignmentADC();

        static float CalculateDeltaADC(Chan::E, float *avgADC1, float *avgADC2, float *delta);
    }
}


void FPGA::Calibrator::RunCalibrate()
{
    Settings storedSettings;

//    bool chanAenable = SET_ENABLED_A;
//    bool chanBenable = SET_ENABLED_B;

    SET_ENABLED_A = SET_ENABLED_B = true;

    Display::SetDrawMode(DrawMode_Hand, FuncAttScreen);

    cal_struct.barA.fullTime = cal_struct.barA.passedTime = cal_struct.barB.fullTime = cal_struct.barB.passedTime = 0;

    state.stateWorkBeforeCalibration = StateWorkFPGA::GetCurrent();
    storedSettings = set;

    Panel::Disable();

    TBase::Set(TBase::_500us);
    TShift::Set(0);
    Range::Set(ChA, Range::_500mV);
    Range::Set(ChB, Range::_500mV);
    RShift::Set(ChA, RShift::ZERO);
    RShift::Set(ChB, RShift::ZERO);
    ModeCouple::Set(ChA, ModeCouple::GND);
    ModeCouple::Set(ChB, ModeCouple::GND);

    AlignmentADC();

    cal_struct.deltaADCPercents[0] = CalculateDeltaADC(ChA, &cal_struct.avrADC1[ChA], &cal_struct.avrADC2[ChA], &cal_struct.deltaADC[ChA]);
    cal_struct.deltaADCPercents[1] = CalculateDeltaADC(ChB, &cal_struct.avrADC1[ChB], &cal_struct.avrADC2[ChB], &cal_struct.deltaADC[ChB]);

//    // Рассчитываем дополнительное смещение и растяжку первого канала
//        gStateFPGA.stateCalibration = StateCalibration_RShiftAstart;
//
//        CalibrateChannel(A);
//
//        // Рассчитываем дополнительное смещение и растяжку второго канала
//        gStateFPGA.stateCalibration = StateCalibration_RShiftBstart;
//
//        CalibrateChannel(B);
//
//        break;

//    RestoreSettingsForCalibration(&storedSettings);
//
//    SET_BALANCE_ADC_A = cal->shiftADCA;
//    SET_BALANCE_ADC_B = cal->shiftADCB;
//
//    SetRShift(A, SET_RSHIFT_A);
//    SetRShift(B, SET_RSHIFT_B);
//
//    gStateFPGA.stateCalibration = StateCalibration_None;
//
//    Panel::WaitPressingButton();
//
//    WriteAdditionRShifts(A);
//    WriteAdditionRShifts(B);
//
//    Panel::Enable();
//    Display::SetDrawMode(DrawMode_Auto, 0);
//
//    SET_ENABLED_A = chanAenable;
//    SET_ENABLED_B = chanBenable;
//
//    DeleteCalibrationStruct();
//
//    OnPressStartStop();
}


static void FPGA::Calibrator::FuncAttScreen()
{
    Painter::BeginScene(Color::BLACK);

    static bool first = true;
    static uint startTime = 0;

    if (first)
    {
        first = false;
        startTime = TIME_MS;
    }

    int16 y = 10;
    Display::Clear();

    Color::SetCurrent(COLOR_FILL);

#define dX 20
#define dY -15

    switch (FPGA::state.stateCalibration)
    {
    case StateCalibration::None:
    {
        PText::DrawTextInRect(40 + dX, y + 25 + dY, SCREEN_WIDTH - 100, "Калибровка завершена. Нажмите любую кнопку, чтобы выйти из режима калибровки.");

        bool drawA = IsCalibrateChannel(A);
        bool drawB = IsCalibrateChannel(B);

        if (drawA) { Painter::DrawText(10 + dX, 55 + dY, "Поправка нуля 1к :"); }
        if (drawB) { Painter::DrawText(10 + dX, 80 + dY, "Поправка нуля 2к :"); }

        int x = 95 + dX;
        for (int i = 0; i < RangeSize; i++)
        {
            if (drawA)
            {
                FuncDrawAdditionRShift(x, 55 + dY, &NRST_RSHIFT_ADD_A(i, ModeCouple_DC));
                FuncDrawAdditionRShift(x, 65 + dY, &NRST_RSHIFT_ADD_A(i, ModeCouple_AC));
            }
            if (drawB)
            {
                FuncDrawAdditionRShift(x, 80 + dY, &NRST_RSHIFT_ADD_B(i, ModeCouple_DC));
                FuncDrawAdditionRShift(x, 90 + dY, &NRST_RSHIFT_ADD_B(i, ModeCouple_AC));
            }
            x += 16;
        }

        if (drawA)
        {
            WriteStretch(A, 10 + dX, 110 + dY);
        }
        if (drawB)
        {
            WriteStretch(B, 10 + dX, 130 + dY);
        }

        if (drawA) { DrawParametersChannel(A, 10 + dX, 150 + dY, false); }
        if (drawB) { DrawParametersChannel(B, 10 + dX, 200 + dY, false); }
    }
    break;

    case StateCalibration_ADCinProgress:
        if (IsCalibrateChannel(A)) { DrawParametersChannel(A, 5, 25, true); }
        if (IsCalibrateChannel(B)) { DrawParametersChannel(B, 5, 75, true); }
        break;

    case StateCalibration_RShiftAstart:
        Painter::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, "Подключите ко входу канала 1 выход калибратора и нажмите кнопку ПУСК/СТОП. \
                                                                        Если вы не хотите калибровать первый канала, нажмите любую другую кнопку.");
        break;

    case StateCalibration_RShiftAinProgress:
        break;

    case StateCalibration_RShiftBstart:
        Painter::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, "Подключите ко входу канала 2 выход калибратора и нажмите кнопку ПУСК/СТОП. \
                                                                        Если вы не хотите калибровать второй канал, нажмите любую другую кнопку.");
        break;

    case StateCalibration_RShiftBinProgress:
        break;

    case StateCalibration_ErrorCalibrationA:
        DrawMessageErrorCalibrate(A);
        break;

    case StateCalibration_ErrorCalibrationB:
        DrawMessageErrorCalibrate(B);
        break;
    }

    /*
    if(stateFPGA.stateCalibration == kNone || stateFPGA.stateCalibration == kRShift0start || stateFPGA.stateCalibration == kRShift1start) {
    x = 230;
    y = 187;
    int delta = 32;
    width = 80;
    height = 25;
    DrawRectangle(x, y, width, height, COLOR_BLACK);
    DrawStringInCenterRect(x, y, width, height, "ПРОДОЛЖИТЬ", COLOR_BLACK, false);
    DrawRectangle(x, y - delta, width, height, COLOR_BLACK);
    DrawStringInCenterRect(x, y - delta, width, height, "ОТМЕНИТЬ", COLOR_BLACK, false);
    }
    */
    const int SIZE = 100;
    char buffer[SIZE];
    snprintf(buffer, SIZE, "%.1f", (TIME_MS - startTime) / 1000.0f);
    Painter::DrawText(0, 0, buffer, Color::BLACK);

    Painter::EndScene();
}
