// 2022/2/11 19:33:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "Panel/Panel.h"
#include "Menu/Pages/Definition.h"


int16 PageMemory::Latest::current = 0;
int8  PageMemory::Internal::currentSignal = 0;
bool  PageMemory::Internal::showAlways = false;
bool  PageMemory::Internal::exitToLast = false;
uint8 PageMemory::SetName::exitTo = RETURN_TO_MAIN_MENU;


void CalculateConditions(int16 pos0, int16 pos1, CursCntrl cursCntrl, bool *cond0, bool *cond1)
{
    bool zeroLessFirst = pos0 < pos1;
    *cond0 = cursCntrl == CursCntrl_1_2 || (cursCntrl == CursCntrl_1 && zeroLessFirst) || (cursCntrl == CursCntrl_2 && !zeroLessFirst);
    *cond1 = cursCntrl == CursCntrl_1_2 || (cursCntrl == CursCntrl_1 && !zeroLessFirst) || (cursCntrl == CursCntrl_2 && zeroLessFirst);
}


int CalculateYforCurs(int y, bool top)
{
    return top ? y + MI_HEIGHT / 2 + 4 : y + MI_HEIGHT - 2;
}


int CalculateXforCurs(int x, bool left)
{
    return left ? x + MI_WIDTH - 20 : x + MI_WIDTH - 5;
}


void CalculateXY(int *x0, int *x1, int *y0, int *y1)
{
    *x0 = CalculateXforCurs(*x0, true);
    *x1 = CalculateXforCurs(*x1, false);
    *y0 = CalculateYforCurs(*y0, true);
    *y1 = CalculateYforCurs(*y1, false);
}


void DrawMenuCursTime(int x, int y, bool left, bool right)
{
    x -= 58;
    y -= 16;
    int x0 = x, x1 = x, y0 = y, y1 = y;
    CalculateXY(&x0, &x1, &y0, &y1);
    for(int i = 0; i < (left ? 3 : 1); i++)
    {
        Painter::DrawVLine(x0 + i, y0, y1);
    }
    for(int i = 0; i < (right ? 3 : 1); i++)
    {
        Painter::DrawVLine(x1 - i, y0, y1);
    }
}


void PageCursors::DrawMenuCursVoltage(int x, int y, bool top, bool bottom)
{
    x -= 65;
    y -= 21;
    int x0 = x, x1 = x, y0 = y, y1 = y;
    CalculateXY(&x0, &x1, &y0, &y1);
    for(int i = 0; i < (top ? 3 : 1); i++)
    {
        Painter::DrawHLine(y0 + i, x0, x1);
    }
    for(int i = 0; i < (bottom ? 3 : 1); i++)
    {
        Painter::DrawHLine(y1 - i, x0, x1);
    }
}


void DrawSB_Exit(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, '\x2e');
    Font::Set(TypeFont::_8);
}


extern const Page mainPage;

#include "PageHelp.cpp"

extern const Page pDebug;
extern const Page pTime;
extern const Page pTrig;


static const arrayItems itemsMainPage =
{
    PageDisplay::self,
    PageChannelA::self,
    PageChannelB::self,
    (void*)&pTrig,
    (void*)&pTime,
    PageCursors::GetPointer(),
    PageMemory::GetPointer(),
    PageMeasures::self,
    PageService::self,
    (void*)&mpHelp,
    (void*)&pDebug
};

const Page mainPage
(
    0, 0,
    "лемч", "MENU",
    "",
    "",
    NamePage::MainPage, &itemsMainPage
);


const Page *Page::ForButton(Key::E button)
{
    static const void *pages[] = {  
        0,                           // Key::Empty
        PageChannelA::self,             // Key::ChannelA
        PageService::self,   // Key::Service
        PageChannelB::self,             // Key::ChannelB
        PageDisplay::self,           // Key::Display
        (void *)&pTime,              // Key::Time
        PageMemory::GetPointer(),    // Key::Memory
        (void *)&pTrig,              // Key::Trig
        0,                           // Key::Start
        PageCursors::GetPointer(),   // Key::Cursors
        PageMeasures::self,          // Key::Measures
        0,                           // Key::Power
        0,                           // Key::Help
        0,                           // Key::Menu
        0,                           // Key::F1
        0,                           // Key::F2
        0,                           // Key::F3
        0,                           // Key::F4
        0,                           // Key::F5
    };

    return (const Page *)pages[button];
}


bool IsMainPage(void *item)
{
    return item == &mainPage;
}


bool PageService::Math::Enabled()
{
    return !DISABLED_DRAW_MATH || ENABLED_FFT;
}


Page *Page::FromName(NamePage::E name)
{
    if (name == NamePage::SB_MemExtSetName)
    {
        return PageMemory::SetName::self;
    }
    else if (name == NamePage::SB_MemInt)
    {
        return PageMemory::Internal::self;
    }

    return nullptr;
}

