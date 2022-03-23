// 2022/2/11 19:46:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "HelpContentPages.h"
#include "HelpContent.h"
#include "Display/Painter.h"
#include "Display/Screen/Grid.h"
#include "Display/font/Font.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Display/String.h"


int currentParagraph = 0;   // ���� TypePage(currentPage) == TypePage_Content, �� ��������� �� ������� ������ ����������
const PageHelp *currentPage = &helpMain;


static const int WIDTH = 295;


static void DrawPageContent()
{
    int y = 50;
    String(TITLE(currentPage)).DrawInCenterRect(0, y, WIDTH, 10);

    int numPage = 0;

    y += 40;

    while(currentPage->pages[numPage])
    {
        Page *page = (Page*)currentPage->pages[numPage];
        pchar title = TITLE(page);
        if(currentParagraph == numPage)
        {
            PText::DrawStringInCenterRectOnBackground(0, y, WIDTH, 10, title, COLOR_BACK, 2, COLOR_FILL);
        }
        else
        {
            String(title).DrawInCenterRect(0, y, WIDTH, 10, COLOR_FILL);
        }
        y += 16;
        numPage++;
    }
}

static void DrawPageDescription()
{
    String(TITLE(currentPage)).DrawInCenterRect(0, 3, WIDTH, 10);
    PText::DrawInRectWithTransfers(2, 15, WIDTH - 5, 240, HINT(currentPage));
}


void HelpContent::Draw()
{
    uint startTime = TIME_MS;
    Painter::FillRegion(Grid::Right(), 0, 319 - Grid::Right(), 20, COLOR_BACK);
    Painter::FillRegion(Grid::Right(), 219, 319 - Grid::Right(), 21);
    Painter::FillRegion(1, 1, WIDTH, 237);
    Painter::DrawRectangle(0, 0, WIDTH + 2, 239, COLOR_FILL);

    /*
    uint16 *addr1 = (uint16*)(0x08000000 + (rand() % 65535));
    uint8 *addr2 = (uint8*)(0x08000000 + (rand() % 65535));

    Color::SetCurrent(COLOR_FILL);
    for (int i = 0; i < 10000; i++)
    {
        Painter::SetPoint((*addr1) % WIDTH, Math::LimitationInt(*addr2, 0, 239));
        addr1++;
        addr2++;
    }
    */

    if(currentPage->type == TypePage_Content)
    {
        DrawPageContent();
    }
    else if(currentPage->type == TypePage_Description)
    {
        DrawPageDescription();
    }

    String("%d", TIME_MS - startTime).Draw(2, 230, COLOR_FILL);
}

static int NumParagraphs(const PageHelp *page)
{
    int retValue = 0;
    while(page->pages[retValue])
    {
        retValue++;
    }
    return retValue;
}

void HelpContent::NextParagraph()
{
    if(currentPage->type == TypePage_Content)
    {
        CircleIncreaseInt(&currentParagraph, 0, NumParagraphs(currentPage) - 1);
    }
}

void HelpContent::PrevParagraph()
{
    if(currentPage->type == TypePage_Content)
    {
        CircleDecreaseInt(&currentParagraph, 0, NumParagraphs(currentPage) - 1);
    }
}

void HelpContent::EnterParagraph()
{
    if(currentPage->type == TypePage_Content)
    {
        currentPage = (const PageHelp *)currentPage->pages[currentParagraph];
    }
    currentParagraph = 0;
}

void HelpContent::LeaveParagraph()
{
    currentParagraph = 0;
    if(currentPage->parent)
    {
        currentPage = (const PageHelp *)currentPage->parent;
    }
}

bool HelpContent::LeaveParagraphIsActive()
{
    return currentPage->parent != 0;
}

bool HelpContent::EnterParagraphIsActive()
{
    return currentPage->type == TypePage_Content;
}
