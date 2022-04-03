// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "FPGA/FPGA.h"
#include "Log.h"
#include "Display/Display.h"
#include "Utils/Math.h"
#include "Display/Screen/Grid.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Text/Warnings.h"


extern const Page pTime;


static bool IsActive_Sample()
{
    return TBase::InModeRandomizer();
}


static const Choice mcSample =
{
    TypeItem::Choice, &pTime, IsActive_Sample,
    {
        "�������", "Sampling"
        ,
        "\"��������\" - \n"
        "\"�������������\" -"
        ,
        "\"Real\" - \n"
        "\"Equals\" - "
    },
    {
        {"�������� �����",  "Real"},
        {"�������������",   "Equals"}
    },
    (int8*)&SET_SAMPLE_TYPE
};


static bool IsActive_PeakDet()
{
    return (SET_TBASE >= TBase::MIN_PEC_DEAT);
}


void OnChanged_PeakDet(bool active)
{
    if (active)
    {
        bool running = FPGA::IsRunning();
        FPGA::Stop();

        PeackDetMode::Set(SET_PEAKDET);
        TBase::Set(SET_TBASE);

        if (SET_PEAKDET_IS_DISABLED)
        {
            int centerX = SHIFT_IN_MEMORY + Grid::Width() / 2;
            SHIFT_IN_MEMORY = centerX * 2 - Grid::Width() / 2;
            PageMemory::OnChanged_NumPoints(true);
        }
        else if (SET_PEAKDET_IS_ENABLED)
        {
            int centerX = SHIFT_IN_MEMORY + Grid::Width() / 2;
            LIMITATION(SHIFT_IN_MEMORY, centerX / 2 - Grid::Width() / 2, 0, ENUM_POINTS_FPGA::ToNumPoints() - Grid::Width());
            PageMemory::OnChanged_NumPoints(true);
        }

        if (running)
        {
            FPGA::Start();
        }
    }
    else
    {
        Warning::ShowBad(Warning::TooSmallSweepForPeakDet);
    }
}


static const Choice mcPeakDet =
{
    TypeItem::Choice, &pTime, IsActive_PeakDet,
    {
        "��� ���", "Pic deat",
        "��������/��������� ������� ��������.",
        "Turns on/off peak detector."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_PEAKDET, OnChanged_PeakDet
};


void PageTime::OnChanged_TPos(bool)
{
    TPos::Set(SET_TPOS);
}


static const Choice mcTPos =
{
    TypeItem::Choice, &pTime, 0,
    {
        "\x7b", "\x7b",
        "����� ����� �������� �������� �������� �� ������� � ������ - ����� ����, �����, ������ ����.",
        "Sets the anchor point nuleovgo time offset to the screen - the left edge, center, right edge."
    },
    {
        {"����",    "Left"},
        {"�����",   "Center"},
        {"�����",   "Right"}
    },
    (int8*)&SET_TPOS, PageTime::OnChanged_TPos
};


static bool IsActive_SelfRecorder()
{
    return TBase::InModeP2P();
}


static void OnChanged_SelfRecorder(bool active)
{
    if (active)
    {
        FPGA::LoadRegUPR();
    }
    else
    {
        Warning::ShowBad(Warning::TooFastScanForSelfRecorder);
    }
}


static const Choice mcSelfRecorder =
{
    TypeItem::Choice, &pTime, IsActive_SelfRecorder,
    {
        "���������", "Self-Recorder",
        "��������/��������� ����� ���������. ���� ����� �������� �� ��������� 20��/��� � ����� ���������.",
        "Turn on/off the recorder. This mode is available for scanning 20ms/div and slower."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_SELFRECORDER, OnChanged_SelfRecorder
};


static const Choice mcDivRole =
{
    TypeItem::Choice, &pTime, 0,
    {
        "�-��� ��/���", "Func Time/DIV"
        ,
        "����� ������� ��� ����� �����/���: � ������ ����� ���������� (����/���� � ��������� ����):\n"
        "1. \"�����\" - ��������� �������� �� �������.\n"
        "2. \"������\" - ����������� �� ������."
        ,
        "Sets the function to handle TIME/DIV: mode of collecting information (START/STOP to start position):\n"
        "1. \"Time\" - change the time shift.\n"
        "2. \"Memory\" - moving from memory."
    },
    {
        {"�����",   "Time"},
        {"������",  "Memory"}
    },
    (int8*)&SET_TIME_DIV_XPOS
};


static const arrayItems itemsTime =
{
    (void *)&mcSample,       // ��������� - �������
    (void *)&mcPeakDet,      // ��������� - ��� ���
    (void *)&mcTPos,         // ��������� - To
    (void *)&mcSelfRecorder, // ��������� - ���������
    (void *)&mcDivRole       // ��������� - �-��� ��/���    
};

static const Page pTime            // ���������
(
    PageMain::self, 0,
    "���������", "SCAN",
    "�������� ��������� ��������.",
    "Contains scan settings.",
    NamePage::Time, &itemsTime
);

const Page *PageTime::self = &pTime;
