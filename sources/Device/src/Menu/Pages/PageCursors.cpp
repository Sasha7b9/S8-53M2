// 2022/2/11 19:47:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Definition.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Colors.h"
#include "Display/Screen/Grid.h"
#include "Utils/Math.h"
#include "Data/Processing.h"
#include "Hardware/Sound.h"
#include "Log.h"
#include "Display/Painter.h"
#include <cmath>
#include <cstring>


extern const SmallButton sbSetU;              // ������� - ���������� - ������� U . ����� ������� ���������� - ������ 1, ������ 2, ��� ������� ��� ���������.
extern const SmallButton sbSetT;              // ������� - ���������� - ������� T . ����� ������� ������� - ������ 1, ������ 2, ��� ������� ��� ���������.
extern const SmallButton sbSet100;            // ������� - ���������� - 100% . ��������� 100 ��������� � ������� ����� ��������.
extern const SmallButton sbSetPointsPercents; // ������� - ���������� - ����������� . ������������ ���� ����������� �������� - �� �������� ��� �� ���������.


static const int MAX_POS_U = 200;
static const int MAX_POS_T = 280;


void SetCursSource(Chan::E ch)
{
    CURS_SOURCE = ch;
}


void IncCursCntrlU(Chan::E ch)
{
    Math::CircleIncrease<int8>((int8*)&CURsU_CNTRL_CH(ch), 0, 3);
}


void IncCursCntrlT(Chan::E ch)
{
    Math::CircleIncrease<int8>((int8*)&CURS_CNTRL_T(ch), 0, 3);
}


// ���������� ������� ������� ����������.
void SetCursPosU(Chan::E ch, int numCur, float pos)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURS_POS_U(ch, numCur) = Math::Limitation<float>(pos, 0, MAX_POS_U);
    }
    else
    { //-V523
        CURS_POS_U(ch, numCur) = Math::Limitation<float>(pos, 0, MAX_POS_U);
    }
}


// ���������� �������� ������� �� �������.
void SetCursPosT(Chan::E ch, int numCur, float pos)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURS_POS_T(ch, numCur) = Math::Limitation<float>(pos, 0, MAX_POS_T);
    }
    else
    { //-V523
        CURS_POS_T(ch, numCur) = Math::Limitation<float>(pos, 0, MAX_POS_T);
    }
}


void PageCursors::Cursors_Update()
{
    Chan::E source = CURS_SOURCE;
    CursLookMode::E lookMode0 = CURS_LOOKMODE_0;
    CursLookMode::E lookMode1 = CURS_LOOKMODE_1;

    int posT0 = 0, posT1 = 0;

    if((lookMode0 == CursLookMode::Voltage || lookMode0 == CursLookMode::Both) && CURS_ACTIVE_IS_T)
    {
        int posU0 = Processing::Cursor::GetU(source, CURS_POS_T0(source));
        SetCursPosU(source, 0, posU0);
    }
    if((lookMode1 == CursLookMode::Voltage || lookMode1 == CursLookMode::Both)  && CURS_ACTIVE_IS_T)
    {
        int posU1 = Processing::Cursor::GetU(source, CURS_POS_T1(source));
        SetCursPosU(source, 1, posU1);
    }
    if((lookMode0 == CursLookMode::Time || lookMode0 == CursLookMode::Both) && CURS_ACTIVE_IS_U)
    {
        int posU0 = CURS_POS_U0(source);
        posT0 = Processing::Cursor::GetT(source, posU0, 0);
        SetCursPosT(source, 0, posT0);
    }
    if((lookMode1 == CursLookMode::Time || lookMode1 == CursLookMode::Both) && CURS_ACTIVE_IS_U)
    {
        int posU1 = CURS_POS_U1(source);
        posT1 = Processing::Cursor::GetT(source, posU1, 1);
        SetCursPosT(source, 1, posT1);
    }
}


static const Choice mcShow =
{
    TypeItem::Choice, PageCursors::self, 0,
    {
        "����������", "Shown",
        "��������/��������� �������.",
        "Enable/disable cursors."
    },
    {
        {"���", "No"},
        {"��",  "Yes"}
    },
    (int8*)&CURS_SHOW
};


static const Choice mcTrackingT1U1 =
{
    TypeItem::Choice, PageCursors::self, 0,
    {
        "�������� \x8e, \x9e",  "Tracking \x8e, \x9e"
        ,
        "����� ����� �������� �� ������ �������� ������� � ����������:\n"
        "1. \"����\" - ������ ������� � ������ ���������� ��������������� �������.\n"
        "2. \"����������\" - ��� ������ ��������� ��������� ������� ������� ������ ���������� ������������� ����������� ��������� �������.\n"
        "3. \"�����\" - ��� ������ ��������� ��������� ������� ���������� ������ ������� ������������� ����������� ��������� �������.\n"
        "4. \"������ � �����\" - ��������� ��� ���� �� ���������� �������, � ����������� �� ����, �� ����� ������ ������������� ��������� �����������."
        ,
        "Sets the mode tracking for second cursor:\n"                   // WARN �������
        "1. \"Disable\" - all cursors are set manually.\n"
        "2. \"Voltage\" - when manually changing the position of the cursor time cursors voltage automatically track changes in the signal.\n"
        "3. \"Time\" - when manually changing the position of the cursor voltage cursors time automatically track changes in the signal.\n"
        "4. \"Volt and time\" - acts as one of the previous modes, depending on which was carried out last effect cursors."
    },
    {
        {DISABLE_RU,        DISABLE_EN},
        {"����������",      "Voltage"},
        {"�����",           "Time"},
        {"������ � �����",  "Volt and time"}
    },
    (int8*)&CURS_LOOKMODE_0
};


static const Choice mcTrackingT2U2 =
{
    TypeItem::Choice, PageCursors::self, 0,
    {
        "�������� \x8f, \x9f", "Tracking \x8f, \x9f"
        ,
        "����� ����� �������� �� ������ �������� ������� � ����������:\n"
        "1. \"����\" - ������ ������� � ������ ���������� ��������������� �������.\n"
        "2. \"����������\" - ��� ������ ��������� ��������� ������� ������� ������ ���������� ������������� ����������� ��������� �������.\n"
        "3. \"�����\" - ��� ������ ��������� ��������� ������� ���������� ������ ������� ������������� ����������� ��������� �������.\n"
        "4. \"������ � �����\" - ��������� ��� ���� �� ���������� �������, � ����������� �� ����, �� ����� ������ ������������� ��������� �����������."
        ,
        "Sets the mode tracking cursors channel 2:\n"
        "1. \"Disable\" - all cursors are set manually.\n"
        "2. \"Voltage\" - when manually changing the position of the cursor time cursors voltage automatically track changes in the signal.\n"
        "3. \"Time\" - when manually changing the position of the cursor voltage cursors time automatically track changes in the signal.\n"
        "4. \"Volt and time\" - acts as one of the previous modes, depending on which was carried out last effect cursors."
    },
    {
        {DISABLE_RU,        DISABLE_EN},
        {"����������",      "Voltage"},
        {"�����",           "Time"},
        {"������. � �����", "Volt. and time"}
    },
    (int8*)&CURS_LOOKMODE_1
};


static const Choice mcShowFreq =
{
    TypeItem::Choice, PageCursors::self, 0,
    {
        "1/dT", "1/dT"
        ,
        "���� ������� \"���\", � ������ ������� ���� ��������� ��������, �������� ���������� ����� ��������� ������� - ������� �������, ���� ������ "
        "�������� ����� ���������� ����� ���������� ���������."
        ,
        "If you select \"Enable\" in the upper right corner displays the inverse of the distance between cursors time - frequency signal, a period "
        "equal to the distance between the time cursors."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&CURSORS_SHOW_FREQ
};


static void PressSB_Cursors_Exit()
{
}


static const SmallButton sbSetExit
(
    PageCursors::PageSet::self,
    COMMON_BEGIN_SB_EXIT,
    PressSB_Cursors_Exit,
    DrawSB_Exit
);


static void PressSB_Cursors_Source()
{
    Chan::E source = CURS_SOURCE_A ? Chan::B : Chan::A;
    SetCursSource(source);
}


static void DrawSB_Cursors_SourceA(int x, int y)
{
    String<>("1").Draw(x + 7, y + 5);
}


static void DrawSB_Cursors_SourceB(int x, int y)
{
    String<>("2").Draw(x + 7, y + 5);
}


static void DrawSB_Cursors_Source(int x, int y)
{
    CURS_SOURCE_A ? DrawSB_Cursors_SourceA(x, y) : DrawSB_Cursors_SourceB(x, y);
}


static const arrayHints hintsSetSource =
{
    { DrawSB_Cursors_SourceA, { "����� 1", "channel 1" } }, { DrawSB_Cursors_SourceB, { "����� 2", "channel 2" } }
};


static const SmallButton sbSetSource
(
    PageCursors::PageSet::self, 0,
    "��������", "Source",
    "����� ������ ��� ��������� ���������",
    "Channel choice for measurements",
    PressSB_Cursors_Source,
    DrawSB_Cursors_Source,
    &hintsSetSource
);


static const arrayItems itemsSet =
{
    (void*)&sbSetExit,
    (void*)&sbSetSource,
    (void*)&sbSetU,
    (void*)&sbSetT,
    (void*)&sbSet100,
    (void*)&sbSetPointsPercents
};


// �������� �������� ������� ������� ���������� �� delta �����
static void SetShiftCursPosU(Chan::E ch, int numCur, float delta)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURS_POS_U(ch, numCur) = Math::Limitation<float>(CURS_POS_U(ch, numCur) - delta, 0, MAX_POS_U);
    }
    else
    { //-V523
        CURS_POS_U(ch, numCur) = Math::Limitation<float>(CURS_POS_U(ch, numCur) - delta, 0, MAX_POS_U);
    }
}


static void MoveCursUonPercentsOrPoints(int delta)
{
    CursCntrl::E cursCntrl = CURsU_CNTRL;

    float value = delta;

    Chan::E source = CURS_SOURCE;

    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        value *= DELTA_U100(source) / 100.0f;
    }

    if (cursCntrl == CursCntrl::_1 || cursCntrl == CursCntrl::_1_2)
    {
        SetShiftCursPosU(source, 0, value);
    }
    if (cursCntrl == CursCntrl::_2 || cursCntrl == CursCntrl::_1_2)
    {
        SetShiftCursPosU(source, 1, value);
    }

    PageCursors::Cursors_Update();
}


// �������� �������� ������� ������� ������� �� delta �����
static void SetShiftCursPosT(Chan::E ch, int numCur, float delta)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURS_POS_T(ch, numCur) = Math::Limitation<float>(CURS_POS_T(ch, numCur) + delta, 0, MAX_POS_T);
    }
    else
    { //-V523
        CURS_POS_T(ch, numCur) = Math::Limitation<float>(CURS_POS_T(ch, numCur) + delta, 0, MAX_POS_T);
    }
}


static void MoveCursTonPercentsOrPoints(int delta)
{
    Chan::E source = CURS_SOURCE;
    CursCntrl::E cursCntrl = CURS_CNTRL_T(source);

    float value = delta;

    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        value *= DELTA_T100(source) / 100.0f;
    }

    if (cursCntrl == CursCntrl::_1 || cursCntrl == CursCntrl::_1_2)
    {
        SetShiftCursPosT(source, 0, value);
    }
    if (cursCntrl == CursCntrl::_2 || cursCntrl == CursCntrl::_1_2)
    {
        SetShiftCursPosT(source, 1, value);
    }

    PageCursors::Cursors_Update();
}


static void OnRotate_RegSet_Set(int angle)
{
    if (CURS_ACTIVE_IS_U)
    {
        MoveCursUonPercentsOrPoints(angle);
    }
    else
    {
        MoveCursTonPercentsOrPoints(angle);
    }
}


static const Page mspSet
(
    PageCursors::self, 0,
    "����������", "SET",
    "������� � ����� ��������� ���������",
    "Switch to cursor measures",
    NamePage::SB_Curs, &itemsSet, EmptyFuncVV, EmptyFuncVV, OnRotate_RegSet_Set
);


static void DrawSB_Cursors_U_Disable(int x, int y)
{
    String<>("U").Draw(x + 7, y + 5);
}


static void DrawSB_Cursors_U_Both_Disable(int x, int y)
{
    PageCursors::DrawMenuCursVoltage(x + 7, y + 5, false, false);
}


static void DrawSB_Cursors_U_Upper(int x, int y)
{
    PageCursors::DrawMenuCursVoltage(x + 7, y + 5, true, false);
}


static void DrawSB_Cursors_U_Lower(int x, int y)
{
    PageCursors::DrawMenuCursVoltage(x + 7, y + 5, false, true);
}


static void DrawSB_Cursors_U_Both_Enable(int x, int y)
{
    PageCursors::DrawMenuCursVoltage(x + 7, y + 5, true, true);
}


static const arrayHints hintsSetU =
{
    { DrawSB_Cursors_U_Disable,     { "������� ���������� ���������",
                                      "cursors of tension are switched off" } },
    { DrawSB_Cursors_U_Both_Disable,{ "������� ���������� ��������",
                                      "cursors of tension are switched on" } },
    { DrawSB_Cursors_U_Upper,       { "������� ���������� ��������, ���������� ������� ��������",
                                      "cursors of tension are switched on, control of the top cursor" } },
    { DrawSB_Cursors_U_Lower,       { "������� ���������� ��������, ���������� ������ ��������",
                                      "cursors of tension are switched on, control of the lower cursor" } },
    { DrawSB_Cursors_U_Both_Enable, { "������� ���������� ��������, ���������� ������ ���������",
                                      "cursors of tension are switched on, control of both cursors" } }
};


static void PressSB_Cursors_U()
{
    if (CURS_ACTIVE_IS_U || CURS_CNTRL_U_IS_DISABLE(CURS_SOURCE))
    {
        IncCursCntrlU(CURS_SOURCE);
    }
    CURS_ACTIVE = CursActive::U;
}


static void DrawSB_Cursors_U(int x, int y)
{
    CursCntrl::E cursCntrl = CURsU_CNTRL;

    if (cursCntrl == CursCntrl::Disable)
    {
        DrawSB_Cursors_U_Disable(x, y);
    }
    else
    {
        if (!CURS_ACTIVE_IS_U)
        {
            DrawSB_Cursors_U_Both_Disable(x, y);
        }
        else
        {
            Chan::E source = CURS_SOURCE;
            bool condTop = false, condDown = false;
            CalculateConditions(PageCursors::GetCursPosU(source, 0), PageCursors::GetCursPosU(source, 1), cursCntrl, &condTop, &condDown);
            if (condTop && condDown)
            {
                DrawSB_Cursors_U_Both_Enable(x, y);
            }
            else if (condTop)
            {
                DrawSB_Cursors_U_Upper(x, y);
            }
            else
            {
                DrawSB_Cursors_U_Lower(x, y);
            }
        }
        /*
        if (set.cursors.lookMode[0] == CursLookMode_Voltage || gset.cursors.lookMode[0] == CursLookMode_Both ||
            gset.cursors.lookMode[1] == CursLookMode_Voltage || gset.cursors.lookMode[1] == CursLookMode_Both)
        {
            Font::Set(TypeFont::_5);
            Char().Draw(x + 10, y, 'c');
            Font::Set(TypeFont::_8);
        }
        */
    }
}


static const SmallButton sbSetU
(
    &mspSet, 0,
    "������� U", "Cursors U",
    "����� �������� ���������� ��� ��������� � ����������",
    "Choice of cursors of voltage for indication and management",
    PressSB_Cursors_U,
    DrawSB_Cursors_U,
    &hintsSetU
);


static void DrawSB_Cursors_T_Disable(int x, int y)
{
    String<>("T").Draw(x + 7, y + 5);
}

static void DrawSB_Cursors_T_Both_Disable(int x, int y)
{
    DrawMenuCursTime(x, y, false, false);
}

static void DrawSB_Cursors_T_Left(int x, int y)
{
    DrawMenuCursTime(x, y, true, false);
}

static void DrawSB_Cursors_T_Right(int x, int y)
{
    DrawMenuCursTime(x, y, false, true);
}

static void DrawSB_Cursors_T_Both_Enable(int x, int y)
{
    DrawMenuCursTime(x, y, true, true);
}


static const arrayHints hintsSetT =
{
    { DrawSB_Cursors_T_Disable,       { "������� ������� ���������",
                                        "cursors of time are switched off" } },
    { DrawSB_Cursors_T_Both_Disable,  { "������� ������� ��������",
                                        "cursors of time are switched on" } },
    { DrawSB_Cursors_T_Left,          { "������� ������� ��������, ���������� ����� ��������",
                                        "cursors of time are switched on, control of the left cursor" } },
    { DrawSB_Cursors_T_Right,         { "������� ������� ��������, ���������� ������ ��������",
                                        "cursors of time are switched on, control of the right cursor" } },
    { DrawSB_Cursors_T_Both_Enable,   { "������� ������� ��������, ���������� ������ ���������",
                                        "cursors of time are switched on, control of both cursors" } }
};


static void PressSB_Cursors_T()
{
    if (CURS_ACTIVE_IS_T || CURS_CNTRL_T_IS_DISABLE(CURS_SOURCE))
    {
        IncCursCntrlT(CURS_SOURCE);
    }
    CURS_ACTIVE = CursActive::T;
}


static void DrawSB_Cursors_T(int x, int y)
{
    Chan::E source = CURS_SOURCE;
    CursCntrl::E cursCntrl = CURS_CNTRL_T(source);

    if (cursCntrl == CursCntrl::Disable)
    {
        DrawSB_Cursors_T_Disable(x, y);
    }
    else
    {
        if (!CURS_ACTIVE_IS_T)
        {
            DrawSB_Cursors_T_Both_Disable(x, y);
        }
        else
        {
            bool condLeft = false, condDown = false;
            CalculateConditions(CURS_POS_T0(source), CURS_POS_T1(source), cursCntrl, &condLeft, &condDown);
            if (condLeft && condDown)
            {
                DrawSB_Cursors_T_Both_Enable(x, y);
            }
            else if (condLeft)
            {
                DrawSB_Cursors_T_Left(x, y);
            }
            else
            {
                DrawSB_Cursors_T_Right(x, y);
            }
        }
    }
}


static const SmallButton sbSetT
(
    &mspSet, 0,
    "������� T", "Cursors T",
    "����� �������� ������� ��� ��������� � ����������",
    "Choice of cursors of time for indication and management",
    PressSB_Cursors_T,
    DrawSB_Cursors_T,
    &hintsSetT
);


// ��������� ������� ��������, ��������������� 100%.
static void SetCursPos100(Chan::E ch)
{
    DELTA_U100(ch) = std::fabs(CURS_POS_U0(ch) - CURS_POS_U1(ch));
    DELTA_T100(ch) = std::fabs(CURS_POS_T0(ch) - CURS_POS_T1(ch));
}


static void PressSB_Cursors_100()
{
    SetCursPos100(CURS_SOURCE);
}


static void DrawSB_Cursors_100(int x, int y)
{
    Font::Set(TypeFont::_5);
    String<>("100%%").Draw(x + 2, y - 3);
    Font::Set(TypeFont::_8);
}


static const SmallButton sbSet100
(
    &mspSet, 0,
    "100%%", "100%%",
    "������������ ��� ���������� ���������. ������� �������� ���������� ����� ��������� ��������� ��� 100%",
    "It is used for percentage measurements. Pressing marks distance between active cursors as 100%",
    PressSB_Cursors_100,
    DrawSB_Cursors_100,
    nullptr
);


static void DrawSB_Cursors_PointsPercents_Percents(int x, int y)
{
    String<>("\x83").Draw(x + 6, y + 5);
}


static void DrawSB_Cursors_PointsPercents_Points(int x, int y)
{
    Font::Set(TypeFont::_5);
    String<>(LANG_RU ? "���" : "pnt").Draw(x + 4, y - 3);
    Font::Set(TypeFont::_8);
}


static const arrayHints hintsSetPointsPercents =
{
    { DrawSB_Cursors_PointsPercents_Percents, { "��� ����������� �������� ������ ������ ��������",
                                                "the step of movement of cursors is multiple to one percent" } },
    { DrawSB_Cursors_PointsPercents_Points,   { "��� ����������� ������� ������ ������ �������",
                                                "the step of movement of the cursor is multiple to one pixel" } }
};


static void PressSB_Cursors_PointsPercents()
{
    Math::CircleIncrease<int8>((int8 *)&CURS_MOVEMENT, 0, 1);
}


static void DrawSB_Cursors_PointsPercents(int x, int y)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        DrawSB_Cursors_PointsPercents_Percents(x, y);
    }
    else
    {
        DrawSB_Cursors_PointsPercents_Points(x, y);
    }
}


static const SmallButton sbSetPointsPercents
(
    &mspSet, 0,
    "�����������", "Movement",
    "����� ���� ����������� �������� - �������� ��� �����",
    "Choice of a step of movement of cursors - percent or points",
    PressSB_Cursors_PointsPercents,
    DrawSB_Cursors_PointsPercents,
    &hintsSetPointsPercents
);


static const arrayItems itemsCursors =
{
    (void *)&mcShow,             // ������� - ����������
    (void *)&mcTrackingT1U1,     // ������� - ������� T1,U1
    (void *)&mcTrackingT2U2,     // ������� - ������� T2,U2
    (void *)&mcShowFreq,         // ������� - 1/dT
    (void *)&mspSet              // ������� - ����������
};


static const Page pCursors                // �������
(
    PageMain::self, 0,
    "�������", "CURSORS",
    "��������� ���������.",
    "Cursor measurements.",
    NamePage::Cursors, &itemsCursors
);


const Page *PageCursors::self = &pCursors;
const Page *PageCursors::PageSet::self = &mspSet;
