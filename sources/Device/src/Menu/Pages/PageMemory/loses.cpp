#include "defines.h"
#include "Display/Font/Font.h"
#include "Settings/Settings.h"
#include "Utils/Text/Text.h"


// ����� "������" ������� - ��, ������� ���� � ���������, �� ������ �� ��� �������


void DrawSB_MemLastSelect(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 3, y + 2, set.memory.isActiveModeSelect ? '\x2a' : '\x28');
    Font::Set(TypeFont::_8);
}


void PressSB_MemLastSelect()
{
    set.memory.isActiveModeSelect = !set.memory.isActiveModeSelect;
}


void DrawSB_MemExtNewFolder(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 1, y, '\x46');
    Font::Set(TypeFont::_8);
}
