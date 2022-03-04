// 2022/2/11 19:49:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Utils/GlobalFunctions.h"
#include "Globals.h"
#include "Menu/MenuItems.h"
#include "Definition.h"
#include "HelpContent.h"


void DrawSB_Help_ParagraphEnter(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    Font::Set(TypeFont::_8);
}

void DrawSB_Help_ParagraphLeave(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    Font::Set(TypeFont::_8);
}

void DrawSB_Help_ParagraphPrev(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 5, '\x4c');
    Font::Set(TypeFont::_8);
}

void DrawSB_Help_ParagraphNext(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    PText::Draw4SymbolsInRect(x + 2, y + 5, '\x4e');
    Font::Set(TypeFont::_8);
}

void OnHelpRegSet(int) 
{

}

extern const Page mpHelp;

const SmallButton sbHelpParagraphEnter
(
    &mpHelp, HelpContent::EnterParagraphIsActive,
    "�������", "Open",
    "��������� ������ �������",
    "Opens the section of the reference",
    HelpContent::EnterParagraph,
    DrawSB_Help_ParagraphEnter
);

const SmallButton sbHelpParagraphLeave
(
    &mpHelp, HelpContent::LeaveParagraphIsActive,
    "�������", "Close",
    "��������� ������ �������",
    "Closes the section of the reference",
    HelpContent::LeaveParagraph,
    DrawSB_Help_ParagraphLeave
);

const SmallButton sbHelpParagraphPrev
(
    &mpHelp, 0,
    "���������� ������", "Previous section",
    "������� ���������� ������ �������",
    "To choose the previous section of the reference",
    HelpContent::PrevParagraph,
    DrawSB_Help_ParagraphPrev
);

const SmallButton sbHelpParagraphNext
(
    &mpHelp, 0,
    "", "",
    "",
    "",
    HelpContent::NextParagraph,
    DrawSB_Help_ParagraphNext
);

static void PressSB_Help_Exit()
{
    Display::RemoveAddDrawFunction();
}

const SmallButton sbExitHelp
(
    &mpHelp,
    COMMON_BEGIN_SB_EXIT,
    PressSB_Help_Exit,
    DrawSB_Exit
);

static const arrayItems itemsHelp =
{
    (void*)&sbExitHelp,
    (void*)&sbHelpParagraphEnter,
    (void*)&sbHelpParagraphLeave,
    (void*)0,
    (void*)&sbHelpParagraphPrev,
    (void*)&sbHelpParagraphNext    
};

static const Page mpHelp            // ������
(
    &mainPage, 0,
    "������", "HELP",
    "������� ������� ������",
    "To open sections of the help",
    NamePage::SB_Help, &itemsHelp, EmptyFuncVV, HelpContent::Draw, OnHelpRegSet
);



void *PageHelp::GetPointer()
{
    return (void *)&mpHelp;
}
