// 2022/2/11 19:47:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Settings/SettingsTypes.h"
#include "Menu/MenuItems.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Menu/Pages/Definition.h"


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


void PageChannelB::OnChanged_Input(bool)
{
    Panel::LED::EnableChannelB(SET_ENABLED_B);
}


static const Choice mcInputA =
{
    TypeItem::Choice, PageChannelA::self, 0,
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
    Panel::LED::EnableChannelA(SET_ENABLED_A);
}


void PageChannelA::OnChanged_Couple(bool)
{
    ModeCouple::Set(Chan::A, SET_COUPLE_A);
}


static const Choice mcCoupleA =
{
    TypeItem::Choice, PageChannelA::self, 0,
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
    (int8*)&SET_COUPLE_A, PageChannelA::OnChanged_Couple
};


void PageChannelA::OnChanged_Filtr(bool)
{
    Filtr::Enable(Chan::A, SET_FILTR_A);
}


static const Choice mcFiltrA =
{
    TypeItem::Choice, PageChannelA::self, 0,
    {
        "������", "Filtr",
        chanFiltrRu,
        chanFiltrEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_FILTR_A, PageChannelA::OnChanged_Filtr
};


static void OnChanged_InverseA(bool)
{
    RShift::Set(Chan::A, SET_RSHIFT_A);
}


static const Choice mcInverseA =
{
    TypeItem::Choice, PageChannelA::self, 0,
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


static const Choice mcMultiplierA =
{
    TypeItem::Choice, PageChannelA::self, 0,
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


static const Choice mcInputB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "����", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&SET_ENABLED_B, PageChannelB::OnChanged_Input
};


void PageChannelB::OnChanged_Couple(bool)
{
    ModeCouple::Set(Chan::B, SET_COUPLE_B);
}


static const Choice mcCoupleB =
{
    TypeItem::Choice, PageChannelB::self, 0,
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
    (int8 *)&SET_COUPLE_B, PageChannelB::OnChanged_Couple
};


void PageChannelB::OnChanged_Filtr(bool)
{
    Filtr::Enable(Chan::B, SET_FILTR_B);
}


static const Choice mcFiltrB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "������", "Filtr",
        chanFiltrRu,
        chanFiltrEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&SET_FILTR_B, PageChannelB::OnChanged_Filtr
};


static void OnChanged_InverseB(bool)
{
    RShift::Set(Chan::B, SET_RSHIFT_B);
}


static const Choice mcInverseB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "��������", "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&SET_INVERSE_B, OnChanged_InverseB
};


static const Choice mcMultiplierB =
{
    TypeItem::Choice, PageChannelB::self, 0,
    {
        "���������", "Divider",
        chanMultiplierRu,
        chanMultiplierEn
    },
    {
        {"�1",  "x1"},
        {"x10", "x10"}
    },
    (int8 *)&SET_DIVIDER(Chan::B)
};


static const arrayItems itemsChanB =
{
    (void *)&mcInputB,
    (void *)&mcCoupleB,
    (void *)&mcFiltrB,
    (void *)&mcInverseB,
    (void *)&mcMultiplierB
};


static const Page pChanB
(
    PageMain::self, 0,
    "����� 2", "CHANNEL 2",
    "�������� ��������� ������ 2.",
    "Contains settings of the channel 2.",
    NamePage::Channel1, &itemsChanB
);

const Page *PageChannelB::self = &pChanB;


static const arrayItems itemsChanA =
{
    (void *)&mcInputA,
    (void *)&mcCoupleA,
    (void *)&mcFiltrA,
    (void *)&mcInverseA,
    (void *)&mcMultiplierA
};


static const Page pChanA
(
    PageMain::self, 0,
    "����� 1", "CHANNEL 1",
    "�������� ��������� ������ 1.",
    "Contains settings of the channel 1.",
    NamePage::Channel0, &itemsChanA
);

const Page *PageChannelA::self = &pChanA;
