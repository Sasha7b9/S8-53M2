// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Settings.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Display/Display.h"
#include "Menu/Menu.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Menu/Pages/Definition.h"
#include <cstring>


bool Settings::isLoaded = false;


static const Settings defaultSettings =
{
    0,                              // size
    // Display
    {
        5,                          // timeShowLevels
        512 - 140,                  // shiftInMemory
        5,                          // timeMessages
        100,                        // brightness
        48,                         // brightnessGrid
        {
            Color::Make(0x00, 0x00, 0x00),   // Color::BLACK = 0x00,
            Color::Make(0xFF, 0xFF, 0xFF),   // Color::WHITE = 0x01,
            Color::Make(0x80, 0x80, 0x80),   // Color::GRID = 0x02,
            Color::Make(0x01, 0xCA, 0xFF),   // Color::DATA_A = 0x03,
            Color::Make(0x00, 0xFF, 0x00),   // COLOR_DATA_B = 0x04,
            Color::Make(0xD5, 0xDA, 0xD5),   // Color::MENU_FIELD = 0x05,
            Color::Make(0xFF, 0xB2, 0x00),   // COLOR_MENU_TITLE = 0x06,
            Color::Make(0x7B, 0x59, 0x00),   // Color::MENU_TITLE_DARK = 0x07,
            Color::Make(0xFF, 0xFF, 0x00),   // Color::MENU_TITLE_BRIGHT = 0x08,
            Color::Make(0x69, 0x89, 0x00),   // COLOR_MENU_ITEM = 0x09,
            Color::Make(0x6B, 0x45, 0x00),   // Color::MENU_ITEM_DARK = 0x0a,
            Color::Make(0xFF, 0xCE, 0x00),   // Color::MENU_ITEM_BRIGHT = 0x0b,
            Color::Make(0x00, 0x00, 0x00),   // Color::MENU_SHADOW = 0x0c,
            Color::Make(0x00, 0x00, 0x00),   // COLOR_EMPTY = 0x0d,
            Color::Make(0x08, 0xA2, 0xCE),   // COLOR_EMPTY_A = 0x0e,
            Color::Make(0x00, 0xCE, 0x00)    // COLOR_EMPTY_B = 0x0f,
        },
        ModeDrawSignal::Lines,      // modeDrawSignal
        TypeGrid::_1,               // typeGrid
        ENumAccumulation::_1,       // numAccumulation
        ENumAveraging::_1,          // numAveraging
        ModeAveraging::Accurately,  // modeAveraging
        ENumMinMax::_1,             // enumMinMax
        Smoothing::Disable,         // smoothing
        ENumSignalsInSec::_25,      // num signals in one second
        Chan::A,                    // lastAffectedChannel
        ModeAccumulation::NoReset,  // modeAccumulation
        AltMarkers::Show,           // altMarkers
        MenuAutoHide::None,         // menuAutoHide
        true,                       // showFullMemoryWindow
        false,                      // showStringNavigation
        LinkingRShift::Position     // linkingRShift
    },
    // channels
    {
        {
            RShift::ZERO,
            ModeCouple::AC,         // ModCouple
            Divider::_1,            // Divider
            Range::_500mV,          // range
            true,                   // enable
            false,                  // inverse
            false,                  // filtr
            1.0f,                   // cal_stretch
            {                       // cal_rshift
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0}
            }
        },
        {
            RShift::ZERO,
            ModeCouple::AC,         // ModCouple
            Divider::_1,            // Divider
            Range::_500mV,          // range
            true,                   // enable
            false,                  // inverse
            false,                  // filtr
            1.0f,                   // cal_stretch
            {                       // cal_rshift
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0}
            }
        }
    },
    // trig
    {
        StartMode::Auto,
        TrigSource::ChannelA,
        TrigPolarity::Front,
        TrigInput::Full,
        {TrigLev::ZERO, TrigLev::ZERO, TrigLev::ZERO},
        ModeLongPressTrig::Auto,
        TrigModeFind::Hand
    },
    // time
    {
        TBase::_200us,
        0,                          // set.time.tShiftRel
        FunctionTime::Time,
        TPos::Center,
        SampleType::Equal,
        PeackDetMode::Disable,
        false
    },
    // cursors
    {
        { CursCntrl::Disable, CursCntrl::Disable },     // CursCntrl U
        { CursCntrl::Disable, CursCntrl::Disable },     // CursCntrl T
        Chan::A,                                        // source
        { 60.0f,  140.0f, 60.0f, 140.0f },              // posCur U
        { 80.0f,  200.0f, 80.0f, 200.0f },              // posCur T
        { 80.0f,  80.0f },                              // ���������� ����� ��������� ���������� ��� 100%
        { 120.0f, 120.0f },                             // ���������� ����� ��������� ������� ��� 100%
        CursMovement::Points,                           // CursMovement
        CursActive::None,                               // CursActive
        { CursLookMode::None, CursLookMode::None },       // ����� �������� ��������.
        false,                                          // showFreq
        false                                           // showCursors
    },
    // memory
    {
        ENUM_POINTS_FPGA::_1024,
        ModeWork::Direct,
        FileNamingMode::Mask,
        "Signal_\x07\x03",      // \x07 - ��������, ��� ����� ������ ���� ��������� �����, \x03 - ������� �� ��� ����������
        "Signal01",
        0,
        false,
        ModeShowIntMem::Both,    // modeShowIntMem
        false,                  // flashAutoConnect
        ModeBtnMemory::Menu,
        ModeSaveSignal::BMP
    },
    // measures
    {
        MeasuresNumber::_1,                       // measures number
        Chan::A_B,                  // source
        ModeViewSignals::AsIs,       // mode view signal
        {
            Measure::VoltageMax,     Measure::VoltageMin,  Measure::VoltagePic,      Measure::VoltageMaxSteady,  Measure::VoltageMinSteady,
            Measure::VoltageAverage, Measure::VoltageAmpl, Measure::VoltageRMS,      Measure::VoltageVybrosPlus, Measure::VoltageVybrosMinus,
            Measure::Period,         Measure::Freq,        Measure::TimeNarastaniya, Measure::TimeSpada,         Measure::DurationPlus
        },
        false,                      // show
        MeasuresField::Screen,      // ����� ��� ��������� ��������, ������� �� ������
        //{0, 200},                 // ��������� �������� �������� ���������� ��� ���� ���������
        //{372, 652},               // ��������� �������� �������� ������� ��� ���� ���������
        {50, 150},                  // ��������� �������� �������� ���������� ��� ���� ���������
        {422, 602},                 // ��������� �������� �������� ������� ��� ���� ���������
        CursCntrl::_1,              // ��������� ������ �������� ����������
        CursCntrl::_1,              // ��������� ������ �������� �������
        CursActive::T,              // ������� ������� �������.
        Measure::None               // marked Measure
    },
    // math
    {
        ScaleFFT::Log,
        SourceFFT::A,
        WindowFFT::Rectangle,
        FFTmaxDB::_60,
        Function::Sum,
        0,
        {100, 256 - 100},
        1,
        1,
        1,
        1,
        false,
        ModeDrawMath::Disable,          // modeDrawMath
        ModeRegSet::Range,              // modeRegSet
        Range::_50mV,
        Divider::_1,
        RShift::ZERO                    // rshift
    },
    // service
    {
        true,                           // screenWelcomeEnable
        true,                          // soundEnable
        CalibratorMode::Freq,           // calibratorEnable
        0,                              // IP-����� (��������)  WARN
        ColorScheme::WhiteLetters       // colorScheme
    },
    {
        0x8b, 0x2e, 0xaf, 0x8f, 0x13, 0x00, // mac
        192, 168, 1, 200,                   // ip
        7,
        255, 255, 255, 0,                   // netmask
        192, 168, 1, 1                      // gateway
    },
    // menu
    {
        {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        Lang::Ru,
        0
    }
};


Settings set = defaultSettings;


void Settings::Load()
{
    if (!HAL_ROM::Settings::Load(this))
    {
        Reset();
    }

    RunAfterLoad();
}


void Settings::Reset()
{
    Settings old = set;

    set = defaultSettings;

    set.chan[ChA].cal_stretch = old.chan[ChA].cal_stretch;
    set.chan[ChB].cal_stretch = old.chan[ChB].cal_stretch;

    std::memcpy(&set.chan[ChA].cal_rshift[0][0], &old.chan[ChA].cal_rshift[0][0], sizeof(set.chan[ChA].cal_rshift[0][0]) * Range::Count * ModeCouple::Count);
    std::memcpy(&set.chan[ChB].cal_rshift[0][0], &old.chan[ChB].cal_rshift[0][0], sizeof(set.chan[ChB].cal_rshift[0][0]) * Range::Count * ModeCouple::Count);

    RunAfterLoad();
}


void Settings::RunAfterLoad()
{
    HAL_LTDC::LoadPalette();

    LED::ChanA.Switch(SET_ENABLED_A);
    LED::ChanB.Switch(SET_ENABLED_B);
    Menu::SetAutoHide(true);
    RShift::ChangedMarkers();

    PageMemory::OnChanged_NumPoints();

    isLoaded = true;
}

bool Settings::Save()
{
    return HAL_ROM::Settings::Save(this);
}


int Page::NumCurrentSubPage() const
{
    return set.menu.currentSubPage[name];
}


bool Menu::IsShown()
{
    return set.menu.isShown;
}


void Menu::Show(bool show)
{
    set.menu.isShown = show ? 1U : 0U;
    Menu::SetAutoHide(true);
}

bool Menu::IsMinimize()
{
    return Item::TypeOpened() == TypeItem::Page && ((const Page *)Item::Opened())->GetName() >= NamePage::SB_Curs;
}


void Page::RotateRegSetSB(int angle)
{
    Page *page = (Page *)Item::Opened();

    if (page->funcRegSetSB)
    {
        page->funcRegSetSB(angle);
    }
}

Range::E &operator++(Range::E &range)
{
    range = (Range::E)((int)range + 1);
    return range;

}


Range::E &operator--(Range::E &range)
{
    range = (Range::E)((int)range - 1);
    return range;
}


float FFTmaxDB::Abs()
{
    static const float db[] = {-40.0f, -60.0f, -80.0f};
    return db[FFT_MAX_DB];
}


int ENUM_POINTS_FPGA::ToNumPoints()
{
    static const int num_points[Count] =
    {
        281, 512, 1024
    };

    return num_points[SET_ENUM_POINTS];
}


int ENUM_POINTS_FPGA::ToNumBytes()
{
    int mul = SET_PEAKDET_IS_ENABLED ? 2 : 1;

    return ToNumPoints() * mul;
}
