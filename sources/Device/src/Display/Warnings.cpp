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
    static const char *warnings[NUM_WARNINGS] = {0};       // Çäåñü ïğåäóïğåæäàşùèå ñîîáùåíèÿ.
    static uint       timeWarnings[NUM_WARNINGS] = {0};   // Çäåñü âğåìÿ, êîãäà ïğåäóïğåæäàşùåå ñîîáùåíèå ïîñòóïèëî íà ıêğàí.
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
        {"ÏĞÅÄÅË ÊÀÍÀË 1 - ÂÎËÜÒ/ÄÅË",      "LIMIT CHANNEL 1 - VOLTS/DIV"},
        {"ÏĞÅÄÅË ÊÀÍÀË 2 - ÂÎËÜÒ/ÄÅË",      "LIMIT CHANNEL 2 - VOLTS/DIV"},
        {"ÏĞÅÄÅË ĞÀÇÂÅĞÒÊÀ - ÂĞÅÌß/ÄÅË",    "LIMIT SWEEP - TIME/DIV"},
        {"ÂÊËŞ×ÅÍ ÏÈÊÎÂÛÉ ÄÅÒÅÊÒÎĞ",        "SET_ENABLED PEAK. DET."},
        {"ÏĞÅÄÅË ÊÀÍÀË 1 - \x0d",           "LIMIT CHANNEL 1 - \x0d"},
        {"ÏĞÅÄÅË ÊÀÍÀË 2 - \x0d",           "LIMIT CHANNEL 2 - \x0d"},
        {"ÏĞÅÄÅË ĞÀÇÂÅĞÒÊÀ - ÓĞÎÂÅÍÜ",      "LIMIT SWEEP - LEVEL"},
        {"ÏĞÅÄÅË ĞÀÇÂÅĞÒÊÀ - \x97",         "LIMIT SWEEP - \x97"},
        {"ÏÈÊ. ÄÅÒ. ÍÅ ĞÀÁÎÒÀÅÒ ÍÀ ĞÀÇÂÅĞÒÊÀÕ ÌÅÍÅÅ 0.5ìêñ/äåë", "PEAK. DET. NOT WORK ON SWEETS LESS THAN 0.5us/div"},
        {"ÑÀÌÎÏÈÑÅÖ ÍÅ ÌÎÆÅÒ ĞÀÁÎÒÀÒÜ ÍÀ ÁÛÑÒĞÛÕ ĞÀÇÂ¨ĞÒÊÀÕ", "SELF-RECORDER DOES NOT WORK FAST SCANS"},
        {"ÔÀÉË ÑÎÕĞÀÍÅÍ",                   "FILE IS SAVED"},
        {"ÑÈÃÍÀË ÑÎÕĞÀÍÅÍ",                 "SIGNAL IS SAVED"},
        {"ÑÈÃÍËÀ ÓÄÀËÅÍ",                   "SIGNAL IS DELETED"},
        {"ÌÅÍŞ ÎÒËÀÄÊÀ ÂÊËŞ×ÅÍÎ",           "MENU DEBUG IS SET_ENABLED"},
        {"ÏÎÄÎÆÄÈÒÅ ÍÅÑÊÎËÜÊÎ ÑÅÊÓÍÄ, ÈÄÅÒ ÏÅĞÅÌÅÙÅÍÈÅ ÄÀÍÍÛÕ", "WAIT A FEW SECONDS, GOES A MOVEMENT DATA"},
        {"ÂĞÅÌß ÍÅ ÓÑÒÀÍÎÂËÅÍÎ. ÌÎÆÅÒÅ ÓÑÒÀÍÎÂÈÒÜ ÅÃÎ ÑÅÉ×ÀÑ", "TIME IS NOT SET. YOU CAN INSTALL IT NOW"},
        {"ÑÈÃÍÀË ÍÅ ÍÀÉÄÅÍ",                "SIGNAL IS NOT FOUND"},
        {"ÍÀ ĞÀÇÂÅĞÒÊÀÕ ÌÅÄËÅÍÍÅÅ 10ìñ/äåë ÆÅËÀÒÅËÜÍÎ ÓÑÒÀÍÀÂ-", "AT SCANNER SLOWLY 10ms/div DESIRABLY SET \"SCAN - \x7b\" IN"},
        {"ËÈÂÀÒÜ \"ĞÀÇÂÅĞÒÊÀ - \x7b\" Â ÏÎËÎÆÅÍÈÅ \"Ëåâî\" ÄËß ÓÑÊÎĞÅ-", "SWITCH \"Left\" FOR TO ACCELERATE THE OUTPUT SIGNAL"},
        {"ÍÈß ÂÛÂÎÄÀ ÑÈÃÍÀËÀ",              ""},
        {"Î×ÅÍÜ ÌÀËÎ ÈÇÌÅĞÅÍÈÉ",            "VERY SMALL MEASURES"},
        {"ÄËß ÂÑÒÓÏËÅÍÈß ÈÇÌÅÍÅÍÈÉ Â ÑÈËÓ", "FOR THE INTRODUCTION OF CHANGES"},
        {"ÂÛÊËŞ×ÈÒÅ ÏĞÈÁÎĞ",                "IN FORCE SWITCH OFF THE DEVICE"},
        {"ÎÒÊËŞ×ÈÒÅ ÂÛ×ÈÑËÅÍÈÅ ÁÏÔ",        "DISCONNECT CALCULATION OF FFT"},
        {"ÎÒÊËŞ×ÈÒÅ ÌÀÒÅÌÀÒÈ×ÅÑÊÓŞ ÔÓÍÊÖÈŞ", "DISCONNECT MATHEMATICAL FUNCTION"},
        {"ÏĞÎØÈÂÊÀ ÑÎÕĞÀÍÅÍÀ",              "FIRMWARE SAVED"},
        {"Ïàìÿòü OTP ïîëíîñòüş çàïîëåíà",   "OPT memory fully completed"}
    };
    return warns[warning][LANG];
}
