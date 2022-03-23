// 2022/03/23 12:17:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Warnings.h"
#include "Display/Colors.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Display/Grid.h"
#include "Tables.h"


namespace Warning
{
    static const int  NUM_WARNINGS = 10;
    static const char *warnings[NUM_WARNINGS] = {0};       // Здесь предупреждающие сообщения.
    static uint       timeWarnings[NUM_WARNINGS] = {0};   // Здесь время, когда предупреждающее сообщение поступило на экран.
    static void OnTimerShowWarning();
    static void DrawStringInRectangle(int x, int y, char const *text);
    static void ShowWarn(pchar);
}


void Warning::ShowBad(Warning::E warning)
{
    Color::ResetFlash();
    ShowWarn(Tables::GetWarning(warning));
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
    Painter::DrawRectangle(Grid::Left(), y, width + 4, height + 4, COLOR_FILL);
    Painter::DrawRectangle(Grid::Left() + 1, y + 1, width + 2, height + 2, COLOR_BACK);
    Painter::FillRegion(Grid::Left() + 2, y + 2, width, height, Color::FLASH_10);
    PText::Draw(Grid::Left() + 3, y + 2, text, Color::FLASH_01);
}


void Warning::ShowGood(Warning::E warning)
{
    Color::ResetFlash();
    ShowWarn(Tables::GetWarning(warning));
    Sound::WarnBeepGood();
}
