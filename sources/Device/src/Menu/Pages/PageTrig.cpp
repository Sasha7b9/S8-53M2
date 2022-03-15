// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/Pages/Definition.h"


extern const Page pTrig;
extern const Page mpAutoFind;


static void OnPress_Mode(bool)
{
    StartMode::Set(START_MODE);
}


static const Choice mcMode =
{
    TypeItem::Choice, &pTrig, 0,
    {
        "�����", "Mode"
        ,
        "����� ����� �������:\n"
        "1. \"����\" - ������ ���������� �������������.\n"
        "2. \"������\" - ������ ���������� �� ������ �������������.\n"
        "3. \"�����������\" - ������ ���������� �� ���������� ������ ������������� ���� ���. ��� ���������� ��������� ����� ������ ������ ����/����."
        ,
        "Sets the trigger mode:\n"
        "1. \"Auto\" - start automatically.\n"
        "2. \"Standby\" - the launch takes place at the level of synchronization.\n"
        "3. \"Single\" - the launch takes place on reaching the trigger levelonce. For the next measurement is necessary to press the START/STOP."
    },
    {
        {"���� ",       "Auto"},
        {"������",      "Wait"},
        {"�����������", "Single"}
    },
    (int8*)&START_MODE, OnPress_Mode
};


static void OnChanged_Source(bool)
{
    TrigSource::Set(TRIG_SOURCE);
}


static const Choice mcSource =
{
    TypeItem::Choice, &pTrig, 0,
    {
        "��������", "Source",
        "����� ��������� ������� �������������.",
        "Synchronization signal source choice."
    },
    {
        {"����� 1", "Channel 1"},
        {"����� 2", "Channel 2"},
        {"�������", "External"}
    },
    (int8*)&TRIG_SOURCE, OnChanged_Source
};


static void OnChanged_Polarity(bool)
{
    TrigPolarity::Set(TRIG_POLARITY);
}


static const Choice mcPolarity =
{
    TypeItem::Choice, &pTrig, 0,
    {
        "����������", "Polarity"
        ,
        "1. \"�����\" - ������ ���������� �� ������ ��������������.\n"
        "2. \"����\" - ������ ���������� �� ����� ��������������."
        ,
        "1. \"Front\" - start happens on the front of clock pulse.\n"
        "2. \"Back\" - start happens on a clock pulse cut."
    },
    {
        {"�����",   "Front"},
        {"����",    "Back"}
    },
    (int8*)&TRIG_POLARITY, OnChanged_Polarity
};


static void OnChanged_Input(bool)
{
    TrigInput::Set(TRIG_INPUT);
}


static const Choice mcInput =
{
    TypeItem::Choice, &pTrig, 0,
    {
        "����", "Input"
        ,
        "����� ����� � ���������� �������������:\n"
        "1. \"��\" - ������ ������.\n"
        "2. \"��\" - �������� ����.\n"
        "3. \"���\" - ������ ������ ������.\n"
        "4. \"���\" - ������ ������� ������."
        ,
        "The choice of communication with the source of synchronization:\n"
        "1. \"SS\" - a full signal.\n"
        "2. \"AS\" - a gated entrance.\n"
        "3. \"LPF\" - low-pass filter.\n"
        "4. \"HPF\" - high-pass filter frequency."
    },
    {
        {"��",  "Full"},
        {"��",  "AC"},
        {"���", "LPF"},
        {"���", "HPF"}
    },
    (int8*)&TRIG_INPUT, OnChanged_Input
};


static const Choice mcAutoFind_Mode =
{
    TypeItem::Choice, &mpAutoFind, 0,
    {
        "�����", "Mode"
        ,
        "����� ������ ��������������� ������ �������������:\n"
        "1. \"������\" - ����� ������������ �� ������� ������ \"�����\" ��� �� ��������� � ������� 0.5� ������ �����, ���� ����������� \"������\x99��� ���� �����\x99�����������\".\n"
        "2. \"��������������\" - ����� ������������ �������������."
        ,
        "Selecting the automatic search of synchronization:\n"
#ifndef WIN32
#pragma push
#pragma diag_suppress 192
#endif
    "1. \"Hand\" - search is run on pressing of the button \"Find\" or on deduction during 0.5s the ����� button if it is established \"SERVICE\x99Mode long �����\x99\x41utolevel\".\n"
#ifndef WIN32
#pragma pop
#endif
    "2. \"Auto\" - the search is automatically."
    },
    {
        {"������",          "Hand"},
        {"��������������",  "Auto"}
    },
    (int8 *)&TRIG_MODE_FIND
};


static bool IsActive_AutoFind_Search()
{
    return TRIG_MODE_FIND_IS_HAND;
}


static void OnPress_AutoFind_Search()
{
    FPGA::FindAndSetTrigLevel();
}


static const Button mbAutoFind_Search
(
    &mpAutoFind, IsActive_AutoFind_Search,
    "�����", "Search",
    "���������� ����� ������ �������������.",
    "Runs for search synchronization level.",
    OnPress_AutoFind_Search
);


static const arrayItems itemsAutoFind =
{
    (void*)&mcAutoFind_Mode,     // ����� - ����� - �����
    (void*)&mbAutoFind_Search     // ����� - ����� - �����    
};


static const Page mpAutoFind
(
    &pTrig, 0,
    "�����", "SEARCH",
    "���������� �������������� ������� ������ �������������.",
    "Office of the automatic search the trigger level.",
    NamePage::TrigAuto, &itemsAutoFind
);


static const arrayItems itemsTrig =
{
    (void *)&mcMode,
    (void *)&mcSource,
    (void *)&mcPolarity,
    (void *)&mcInput,
    (void *)&mpAutoFind
};


static const Page pTrig
(
    PageMain::self, 0,
    "�����", "TRIG",
    "�������� ��������� �������������.",
    "Contains synchronization settings.",
    NamePage::Trig, &itemsTrig
);

const Page *PageTrig::self = &pTrig;
