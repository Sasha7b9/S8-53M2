// 2022/03/23 12:17:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Warnings.h"
#include "Display/Colors.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Display/Screen/Grid.h"
#include "Tables.h"
#include "Display/String.h"


namespace Warning
{
    static const int  NUM_WARNINGS = 10;
    static const char *warnings[NUM_WARNINGS] = {0};       // ����� ��������������� ���������.
    static uint       timeWarnings[NUM_WARNINGS] = {0};   // ����� �����, ����� ��������������� ��������� ��������� �� �����.
    static void OnTimerShowWarning();
    static void DrawStringInRectangle(int x, int y, char const *text);
    static void ShowWarn(pchar);
    static pchar GetWarning(Warning::E);
}


void Warning::ShowBad(Warning::E warning)
{
    Color::ResetFlash();
    ShowWarn(GetWarning(warning));
    Sound::WarnBeepBad();
}


void Warning::ShowWarn(pchar message)
{
    if (warnings[0] == 0)
    {
        Timer::Enable(TypeTimer::ShowMessages, 100, OnTimerShowWarning);
    }

    bool alreadyStored = false;

    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (warnings[i] == 0 && !alreadyStored)
        {
            warnings[i] = message;
            timeWarnings[i] = TIME_MS;
            alreadyStored = true;
        }
        else if (warnings[i] == message)
        {
            timeWarnings[i] = TIME_MS;
            return;
        }
    }
}


void Warning::OnTimerShowWarning()
{
    uint time = TIME_MS;

    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (time - timeWarnings[i] > (uint)TIME_MESSAGES * 1000)
        {
            timeWarnings[i] = 0;
            warnings[i] = 0;
        }
    }

    int pointer = 0;

    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (warnings[i] != 0)
        {
            warnings[pointer] = warnings[i];
            timeWarnings[pointer] = timeWarnings[i];
            if (pointer != i)
            {
                timeWarnings[i] = 0;
                warnings[i] = 0;
            }
            pointer++;
        }
    }

    if (pointer == 0)
    {
        Timer::Disable(TypeTimer::ShowMessages);
    }
}


void Warning::Clear()
{
    Timer::Disable(TypeTimer::ShowMessages);

    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        warnings[i] = 0;
        timeWarnings[i] = 0;
    }
}


void Warning::DrawWarnings()
{
    int delta = 12;
    int y = Grid::BottomMessages();

    for (int i = 0; i < 10; i++)
    {
        if (warnings[i] != 0)
        {
            DrawStringInRectangle(Grid::Left(), y, warnings[i]);
            y -= delta;
        }
    }
}


void Warning::DrawStringInRectangle(int, int y, char const *text)
{
    int width = Font::GetLengthText(text);
    int height = 8;
    Rectangle(width + 4, height + 4).Draw(Grid::Left(), y, COLOR_FILL);
    Rectangle(width + 2, height + 2).Draw(Grid::Left() + 1, y + 1, COLOR_BACK);
    Painter::FillRegion(Grid::Left() + 2, y + 2, width, height, Color::FLASH_10);
    String(text).Draw(Grid::Left() + 3, y + 2, Color::FLASH_01);
}


void Warning::ShowGood(Warning::E warning)
{
    Color::ResetFlash();
    ShowWarn(GetWarning(warning));
    Sound::WarnBeepGood();
}


pchar Warning::GetWarning(Warning::E warning)
{
    static pchar warns[Warning::Count][2] =
    {
        {"������ ����� 1 - �����/���",      "LIMIT CHANNEL 1 - VOLTS/DIV"},
        {"������ ����� 2 - �����/���",      "LIMIT CHANNEL 2 - VOLTS/DIV"},
        {"������ ��������� - �����/���",    "LIMIT SWEEP - TIME/DIV"},
        {"������� ������� ��������",        "SET_ENABLED PEAK. DET."},
        {"������ ����� 1 - \x0d",           "LIMIT CHANNEL 1 - \x0d"},
        {"������ ����� 2 - \x0d",           "LIMIT CHANNEL 2 - \x0d"},
        {"������ ��������� - �������",      "LIMIT SWEEP - LEVEL"},
        {"������ ��������� - \x97",         "LIMIT SWEEP - \x97"},
        {"���. ���. �� �������� �� ���������� ����� 0.5���/���", "PEAK. DET. NOT WORK ON SWEETS LESS THAN 0.5us/div"},
        {"��������� �� ����� �������� �� ������� ���¨�����", "SELF-RECORDER DOES NOT WORK FAST SCANS"},
        {"���� ��������",                   "FILE IS SAVED"},
        {"������ ��������",                 "SIGNAL IS SAVED"},
        {"������ ������",                   "SIGNAL IS DELETED"},
        {"���� ������� ��������",           "MENU DEBUG IS SET_ENABLED"},
        {"��������� ��������� ������, ���� ����������� ������", "WAIT A FEW SECONDS, GOES A MOVEMENT DATA"},
        {"����� �� �����������. ������ ���������� ��� ������", "TIME IS NOT SET. YOU CAN INSTALL IT NOW"},
        {"������ �� ������",                "SIGNAL IS NOT FOUND"},
        {"�� ���������� ��������� 10��/��� ���������� �������-", "AT SCANNER SLOWLY 10ms/div DESIRABLY SET \"SCAN - \x7b\" IN"},
        {"������ \"��������� - \x7b\" � ��������� \"����\" ��� ������-", "SWITCH \"Left\" FOR TO ACCELERATE THE OUTPUT SIGNAL"},
        {"��� ������ �������",              ""},
        {"����� ���� ���������",            "VERY SMALL MEASURES"},
        {"��� ���������� ��������� � ����", "FOR THE INTRODUCTION OF CHANGES"},
        {"��������� ������",                "IN FORCE SWITCH OFF THE DEVICE"},
        {"��������� ���������� ���",        "DISCONNECT CALCULATION OF FFT"},
        {"��������� �������������� �������", "DISCONNECT MATHEMATICAL FUNCTION"},
        {"�������� ���������",              "FIRMWARE SAVED"},
        {"������ OTP ��������� ��������",   "OPT memory fully completed"}
    };
    return warns[warning][LANG];
}
