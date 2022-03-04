// 2022/2/11 19:47:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Settings/SettingsTypes.h"
#include "Menu/MenuItems.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/Pages/Definition.h"


extern const Page pChanA;
extern const Page pChanB;


extern const Choice mcInputA;                   // ����� 1 - ����
extern const Choice mcCoupleA;                  // ����� 1 - �����
void OnChanged_CoupleA(bool active);
extern const Choice mcFiltrA;                   // ����� 1 - ������
void OnChanged_FiltrA(bool active);
extern const Choice mcInverseA;                 // ����� 1 - ��������
static void OnChanged_InverseA(bool active);
extern const Choice mcMultiplierA;              // ����� 1 - ���������

extern const Choice mcInputB;                   // ����� 2 - ����
extern const Choice mcCoupleB;                  // ����� 2 - �����
void OnChanged_CoupleB(bool active);
extern const Choice mcFiltrB;                   // ����� 2 - ������
void OnChanged_FiltrB(bool active);
extern const Choice mcInverseB;                 // ����� 2 - ��������
static void OnChanged_InverseB(bool active);
extern const Choice mcMultiplierB;              // ����� 2 - ���������



extern const char chanInputRu[] =   "1. \"���\" - �������� ������ �� �����.\n"
                                    "2. \"����\" - �� �������� ������ �� �����.";
extern const char chanInputEn[] =   "1. \"Enable\" - signal output to the screen.\n"
                                    "2. \"Disable\" - no output to the screen.";

extern const char chanCoupleRu[] =  "����� ��� ����� � ���������� �������.\n"
                                    "1. \"����\" - �������� ����.\n"
                                    "2. \"�����\" - �������� ����.\n"
                                    "3. \"�����\" - ���� ������� � �����.";
extern const char chanCoupleEn[] =  "Sets a type of communication with a signal source.\n"
                                    "1. \"AC\" - open input.\n"
                                    "2. \"DC\" - closed input.\n"
                                    "3. \"Ground\" - input is connected to the ground.";

extern const char chanFiltrRu[] = "��������/��������� ������ ��� ����������� ������ �����������.";
extern const char chanFiltrEn[] = "Includes/switches-off the filter for restriction of a pass-band.";

extern const char chanInverseRu[] = "��� \"���\" ������ �� ������ ����� ����������� ������ ������������ U = 0�.";
extern const char chanInverseEn[] = "When \"Enable\" signal on the screen will be reflected symmetrically with respect to U = 0V.";

extern const char chanMultiplierRu[] = "���������� �������:\n\"x1\" - ������ �� �����������.\n\"x10\" - ������ ����������� � 10 ���";
extern const char chanMultiplierEn[] = "Attenuation: \n\"x1\" - the signal is not attenuated.\n\"x10\" - the signal is attenuated by 10 times";



extern const Page mainPage;


// ����� 1 /////////////////////////
static const arrayItems itemsChanA =
{
    (void*)&mcInputA,       // ����� 1 - ����
    (void*)&mcCoupleA,      // ����� 1 - �����
    (void*)&mcFiltrA,       // ����� 1 - ������
    (void*)&mcInverseA,     // ����� 1 - ��������
    (void*)&mcMultiplierA   // ����� 1 - ���������
};

const Page pChanA           // ����� 1
(
    &mainPage, 0,
    "����� 1", "CHANNEL 1",
    "�������� ��������� ������ 1.",
    "Contains settings of the channel 1.",
    NamePage::Channel0, &itemsChanA
);


// ����� 1 - ���� -----------
static const Choice mcInputA =
{
    TypeItem::Choice, &pChanA, 0,
    {
        "����", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_ENABLED_A, PageChannelA::OnChanged_Input
};

void PageChannelA::OnChanged_Input(bool)
{
    Panel::EnableLEDChannelA(Chan::Enabled(Chan::A));
}


// ����� 1 - ����� ----------
static const Choice mcCoupleA =
{
    TypeItem::Choice, &pChanA, 0,
    {
        "�����",   "Couple",
        chanCoupleRu,
        chanCoupleEn
    },
    {
        {"����",    "AC"},
        {"�����",   "DC"},
        {"�����",   "Ground"}
    },
    (int8*)&SET_COUPLE_A, OnChanged_CoupleA
};

void OnChanged_CoupleA(bool)
{
    ModeCouple::Set(Chan::A, SET_COUPLE_A);
}


// ����� 1 - ������ ---------
static const Choice mcFiltrA =
{
    TypeItem::Choice, &pChanA, 0,
    {
        "������", "Filtr",
        chanFiltrRu,
        chanFiltrEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_FILTR_A, OnChanged_FiltrA
};

void OnChanged_FiltrA(bool)
{
    Filtr::Enable(Chan::A, SET_FILTR_A);
}


// ����� 1 - �������� -------
static const Choice mcInverseA =
{
    TypeItem::Choice, &pChanA, 0,
    {
        "��������",    "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_INVERSE_A, OnChanged_InverseA
};

static void OnChanged_InverseA(bool)
{
    RShift::Set(Chan::A, SET_RSHIFT_A);
}


// ����� 1 - ��������� ------
static const Choice mcMultiplierA =
{
    TypeItem::Choice, &pChanA, 0,
    {
        "���������", "Divider",
        chanMultiplierRu,
        chanMultiplierEn
    },
    {
        {"�1",  "x1"},
        {"x10", "x10"}
    },
    (int8*)&SET_DIVIDER(Chan::A)
};



// ����� 2 /////////////////////////
static const arrayItems itemsChanB =
{
    (void*)&mcInputB,       // ����� 2 - ����
    (void*)&mcCoupleB,      // ����� 2 - �����
    (void*)&mcFiltrB,       // ����� 2 - ������
    (void*)&mcInverseB,     // ����� 2 - ��������
    (void*)&mcMultiplierB   // ����� 2 - ���������
};

const Page pChanB
(
    &mainPage, 0,
    "����� 2", "CHANNEL 2",
    "�������� ��������� ������ 2.",
    "Contains settings of the channel 2.",
    NamePage::Channel1, &itemsChanB
);


// ����� 2 - ���� -----------
static const Choice mcInputB =  // ����� 2
{
    TypeItem::Choice, &pChanB, 0,
    {
        "����", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_ENABLED_B, PageChannelB::OnChanged_Input
};

void PageChannelB::OnChanged_Input(bool)
{
    Panel::EnableLEDChannelB(Chan::Enabled(Chan::B));
}


// ����� 2 - ����� ----------
static const Choice mcCoupleB =
{
    TypeItem::Choice, &pChanB, 0,
    {
        "�����", "Couple",
        chanCoupleRu,
        chanCoupleEn
    },
    {
        {"����",    "AC"},
        {"�����",   "DC"},
        {"�����",   "Ground"}
    },
    (int8*)&SET_COUPLE_B, OnChanged_CoupleB
};

void OnChanged_CoupleB(bool)
{
    ModeCouple::Set(Chan::B, SET_COUPLE_B);
}

// ����� 2 - ������ ---------
static const Choice mcFiltrB =
{
    TypeItem::Choice, &pChanB, 0,
    {
        "������", "Filtr",
        chanFiltrRu,
        chanFiltrEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_FILTR_B, OnChanged_FiltrB
};

void OnChanged_FiltrB(bool)
{
    Filtr::Enable(Chan::B, SET_FILTR_B);
}


// ����� 2 - �������� -------
static const Choice mcInverseB =
{
    TypeItem::Choice, &pChanB, 0,
    {
        "��������", "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_INVERSE_B, OnChanged_InverseB
};

static void OnChanged_InverseB(bool)
{
    RShift::Set(Chan::B, SET_RSHIFT_B);
}


// ����� 2 - ��������� ------
static const Choice mcMultiplierB =
{
    TypeItem::Choice, &pChanB, 0,
    {
        "���������", "Divider",
        chanMultiplierRu,
        chanMultiplierEn
    },
    {
        {"�1",  "x1"},
        {"x10", "x10"}
    },
    (int8*)&SET_DIVIDER(Chan::B)
};
