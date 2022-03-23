// 2022/02/11 17:45:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Painter.h"
#include "Font/font.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "Hardware/InterCom.h"
#include "Display/String.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>


namespace PText
{
    // ��������� ��������������� �������
    int DrawCharHard(int x, int y, char symbol);

    int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight);

    // ���� draw == false, �� �������� ������ �� ����, ������ ������������ ������ ��� ����������
    int DrawPartWord(char *word, int x, int y, int xRight, bool draw);

    // ���������� ������ ������, ������� ����� ����� text, ��� ������ �� left �� right � ���������� height. ����
    // bool == false, �� ����� �� ������ �� ����� 
    bool GetHeightTextWithTransfers(int left, int top, int right, pchar text, int *height);

    bool ByteFontNotEmpty(int eChar, int byte);

    bool BitInFontIsExist(int eChar, int numByte, int bit);

    void DrawCharInColorDisplay(int eX, int eY, uchar symbol);

    int DrawBigChar(int eX, int eY, int size, char symbol);

    bool IsLetter(char symbol);

    String GetWord(pchar firstSymbol, int *length);

    bool IsConsonant(char symbol);

    // ������� ��������� �������. C letters ���������� ����� �����, ��� ����� ����� �������, � lettersInSyllable �����
    // �������� ����� ���� � ��������� �����. ���� ����� �����������, ������� ���������� false
    bool FindNextTransfer(char *letters, int8 *lettersInSyllable);

    int8 *BreakWord(char *word);

    bool CompareArrays(const bool *array1, const bool *array2, int numElems);

    // ���������� ����� ����� �� ����� numSyllable(�����������) ������ �� ������ ��������
    char *PartWordForTransfer(char *word, int8 *lengthSyllables, int numSyllables, int numSyllable, char buffer[30]);
}


void Font::Set(TypeFont::E typeFont)
{
    if (typeFont == current)
    {
        return;
    }

    Font::font = Font::fonts[typeFont];

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(4, SET_FONT);
        command.PushByte(typeFont);
        command.Transmit(2);
    }
}


void Font::Load(TypeFont::E typeFont)
{
    if (InterCom::TransmitGUIinProcess())
    {
        uint8 *pFont = (uint8 *)Font::fonts[typeFont];

        CommandBuffer command(2 + 4, LOAD_FONT);
        command.PushByte(typeFont);
        command.PushWord(Font::fonts[typeFont]->height);

        command.Transmit(2 + 4);

        pFont += 4;

        for (int i = 0; i < 256; i++)
        {
            InterCom::Send(pFont, sizeof(Symbol));
            pFont += sizeof(Symbol);
            Timer::PauseOnTicks(10000);
        }
    }
}


bool PText::ByteFontNotEmpty(int eChar, int byte)
{
    static const uint8 *bytes = 0;
    static int prevChar = -1;

    if (eChar != prevChar)
    {
        prevChar = eChar;
        bytes = Font::font->symbol[prevChar].bytes;
    }

    return bytes[byte] != 0;
}


bool PText::BitInFontIsExist(int eChar, int numByte, int bit)
{
    static uint8 prevByte = 0;      // WARN ����� ����� ������� �����?
    static int prevChar = -1;
    static int prevNumByte = -1;
    if (prevNumByte != numByte || prevChar != eChar)
    {
        prevByte = Font::font->symbol[eChar].bytes[numByte];
        prevChar = eChar;
        prevNumByte = numByte;
    }
    return prevByte & (1 << bit);
}


void PText::DrawCharInColorDisplay(int eX, int eY, uchar symbol)
{
    int8 width = (int8)Font::font->symbol[symbol].width;
    int8 height = (int8)Font::font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    Point().Set(x, y);
                }
                x++;
            }
        }
    }
}


int PText::DrawBigChar(int eX, int eY, int size, char symbol)
{
    int8 width = (int8)Font::font->symbol[symbol].width;
    int8 height = (int8)Font::font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b * size + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    for (int i = 0; i < size; i++)
                    {
                        for (int j = 0; j < size; j++)
                        {
                            Point().Set(x + i, y + j);
                        }
                    }
                }
                x += size;
            }
        }
    }

    return eX + width * size;
}


int PText::DrawCharHard(int eX, int eY, char symbol)
{
    int8 width = (int8)Font::font->symbol[(uint8)symbol].width;
    int8 height = (int8)Font::font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    Point().Set(x, y);
                }
                x++;
            }
        }
    }

    return eX + width;
}


int Char::Draw(int x, int y, Color::E color)
{
    Color::SetCurrent(color);

    if (Font::GetSize() == 5)
    {
        PText::DrawCharHard(x, y + 3, symbol);
    }
    else if (Font::GetSize() == 8)
    {
        PText::DrawCharHard(x, y, symbol);
    }
    else
    {
        PText::DrawCharInColorDisplay(x, y, (uint8)symbol);
    }

    return x + Font::GetLengthSymbol((uint8)symbol);
}


int PText::DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight)
{
    int8 width = (int8)Font::font->symbol[symbol].width;
    int8 height = (int8)Font::font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    if ((x >= limitX) && (x <= (limitX + limitWidth)) && (y >= limitY) && (y <= limitY + limitHeight))
                    {
                        Point().Set(x, y);
                    }
                }
                x++;
            }
        }
    }

    return eX + width + 1;
}


int String::DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight)
{
    Color::SetCurrent(color);
    int retValue = x;

    char *text = buffer;

    while (*text)
    {
        x = PText::DrawCharWithLimitation(x, y, (uint8)*text, limitX, limitY, limitWidth, limitHeight);
        retValue += Font::GetLengthSymbol((uint8)*text);
        text++;
    }

    return retValue + 1;
}


bool PText::IsLetter(char symbol)
{
    static const bool isLetter[256] =
    {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true
    };

    return isLetter[(uint8)symbol];
}


String PText::GetWord(pchar firstSymbol, int *length)
{
    *length = 0;

    String result;

    while (IsLetter(*firstSymbol))
    {
        result.Append(*firstSymbol);
        firstSymbol++;
        (*length)++;
    }

    return result;
}


bool PText::IsConsonant(char symbol)
{
    static const bool isConsonat[256] =
    {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, true, true, true, true, false, true, true, false, false, true, true, true, true, false, true,
        true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false,
        false, true, true, true, true, false, true, true, false, false, true, true, true, true, false, true,
        true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false
    };

    return isConsonat[(uint8)symbol];
}


bool PText::CompareArrays(const bool *array1, const bool *array2, int numElems)
{
    for (int i = 0; i < numElems; i++)
    {
        if (array1[i] != array2[i])
        {
            return false;
        }
    }
    return true;
}


bool PText::FindNextTransfer(char *letters, int8 *lettersInSyllable)
{

#define VOWEL       0   // �������
#define CONSONANT   1   // ���������

    *lettersInSyllable = (int8)std::strlen(letters);

    if (std::strlen(letters) <= 3) //-V1051
    {
        return false;
    }

    static const bool template1[3] = {false, true, true};               //     011     2   // ����� ������� ������� �������
    static const bool template2[4] = {true, false, true, false};        //     1010    2
    static const bool template3[4] = {false, true, false, true};        //     0101    3
    static const bool template4[4] = {true, false, true, true};         //     1011    3
    static const bool template5[4] = {false, true, false, false};       //     0100    3
    static const bool template6[4] = {true, false, true, true};         //     1011    3
    static const bool template7[5] = {true, true, false, true, false};  //     11010   3
    static const bool template8[6] = {true, true, false, true, true};   //     11011   4

    bool consonant[20];

    int size = (int)std::strlen(letters);

    for (int i = 0; i < size; i++)
    {
        consonant[i] = IsConsonant(letters[i]);
    }

    if (CompareArrays(template1, consonant, 3))
    {
        *lettersInSyllable = 2;
        return true;
    }
    if (CompareArrays(template2, consonant, 4))
    {
        *lettersInSyllable = 2;
        return true;
    }
    if (std::strlen(letters) < 5)
    {
        return false;
    }
    if (CompareArrays(template3, consonant, 4) || CompareArrays(template4, consonant, 4) || CompareArrays(template5, consonant, 4) || CompareArrays(template6, consonant, 4))
    {
        *lettersInSyllable = 3;
        return true;
    }
    if (std::strlen(letters) < 6)
    {
        return false;
    }
    if (CompareArrays(template7, consonant, 5))
    {
        *lettersInSyllable = 3;
        return true;
    }
    if (CompareArrays(template8, consonant, 5))
    {
        *lettersInSyllable = 4;
        return true;
    }
    return false;
}


int8* PText::BreakWord(char *word)
{
    int num = 0;
    static int8 lengthSyllables[10];
    char *position = word;
    while (FindNextTransfer(position, &(lengthSyllables[num])))
    {
        position += lengthSyllables[num];
        num++;
    }
    lengthSyllables[num + 1] = 0;
    if (std::strcmp(word, "���������") == 0)
    {
        int8 lengths[] = {5, 2, 2, 0};
        std::memcpy(lengthSyllables, lengths, 4);
    }
    else if (std::strcmp(word, "���������������") == 0)
    {
        int8 lenghts[] = {4, 3, 4, 5, 3, 0};
        std::memcpy(lengthSyllables, lenghts, 6);
    }
    return lengthSyllables;
}


char* PText::PartWordForTransfer(char *word, int8* lengthSyllables, int, int numSyllable, char buffer[30])
{
    int length = 0;

    for (int i = 0; i <= numSyllable; i++)
    {
        length += lengthSyllables[i];
    }

    std::memcpy((void*)buffer, (void*)word, (uint)length);
    buffer[length] = '-';
    buffer[length + 1] = '\0';
    return buffer;
}


int PText::DrawPartWord(char *word, int x, int y, int xRight, bool draw)
{
    int8 *lengthSyllables = BreakWord(word);
    int numSyllabels = 0;
    char buffer[30];
    for (int i = 0; i < 10; i++)
    {
        if (lengthSyllables[i] == 0)
        {
            numSyllabels = i;
            break;
        }
    }

    for (int i = numSyllabels - 2; i >= 0; i--)
    {
        char *subString = PartWordForTransfer(word, lengthSyllables, numSyllabels, i, buffer);
        int length = Font::GetLengthText(subString);
        if (xRight - x > length - 5)
        {
            if (draw)
            {
                String(subString).Draw(x, y);
            }

            return (int)std::strlen(subString) - 1;
        }
    }

    return 0;
}


int String::DrawInRectWithTransfers(int eX, int eY, int eWidth, int eHeight, Color::E color)
{
    Color::SetCurrent(color);

    int top = eY;
    int left = eX;
    int right = eX + eWidth;
    int bottom = eY + eHeight;

    pchar text = buffer;

    int numSymb = (int)std::strlen(text);

    int y = top - 1;
    int x = left;

    int curSymbol = 0;

    while (y < bottom && curSymbol < numSymb)
    {
        while (x < right - 1 && curSymbol < numSymb)
        {
            int length = 0;
            String word = PText::GetWord(text + curSymbol, &length);

            if (length <= 1)                            // ��� ��������� �������� ��� ����, �.�. ����� �� �������
            {
                char symbol = text[curSymbol++];
                if (symbol == '\n')
                {
                    x = right;
                    continue;
                }
                if (symbol == ' ' && x == left)
                {
                    continue;
                }
                x = Char(symbol).Draw(x, y);
            }
            else                                            // � ����� ������� �� ������� ���� ��� ��������� �������, �.�. ������� �����
            {
                int lengthString = Font::GetLengthText(word.c_str());
                if (x + lengthString > right + 5)
                {
                    int numSymbols = PText::DrawPartWord(word.c_str(), x, y, right, true);
                    x = right;
                    curSymbol += numSymbols;
                    continue;
                }
                else
                {
                    curSymbol += length;
                    x = String(word).Draw(x, y);
                }
            }
        }
        x = left;
        y += 9;
    }

    return y;
}


bool PText::GetHeightTextWithTransfers(int left, int top, int right, pchar text, int *height)
{
    int numSymb = (int)std::strlen(text);

    int y = top - 1;
    int x = left;

    int curSymbol = 0;

    while (y < 231 && curSymbol < numSymb)
    {
        while (x < right - 1 && curSymbol < numSymb)
        {
            int length = 0;
            String word = GetWord(text + curSymbol, &length);

            if (length <= 1)                            // ��� ��������� �������� ��� ����, �.�. ����� �� �������
            {
                char symbol = text[curSymbol++];
                if (symbol == '\n')
                {
                    x = right;
                    continue;
                }
                if (symbol == ' ' && x == left)
                {
                    continue;
                }
                x += Font::GetLengthSymbol((uint8)symbol);
            }
            else                                            // � ����� ������� �� ������� ���� ��� ��������� �������, �.�. ������� �����
            {
                int lengthString = Font::GetLengthText(word.c_str());
                if (x + lengthString > right + 5)
                {
                    int numSymbols = DrawPartWord(word.c_str(), x, y, right, false);
                    x = right;
                    curSymbol += numSymbols;
                    continue;
                }
                else
                {
                    curSymbol += length;
                    x += Font::GetLengthText(word.c_str());
                }
            }
        }
        x = left;
        y += 9;
    }

    LIMITATION(*height, y - top + 4, 0, 239);

    return curSymbol == numSymb;
}


int String::DrawInBoundedRectWithTransfers(int x, int y, int width, Color::E colorBackground, Color::E colorFill)
{
    int height = 0;

    PText::GetHeightTextWithTransfers(x + 3, y + 3, x + width - 8, buffer, &height);

    Rectangle(width, height).Draw(x, y, colorFill);
    Region(width - 2, height - 2).Fill(x + 1, y + 1, colorBackground);
    DrawInRectWithTransfers(x + 3, y + 3, width - 8, height, colorFill);
    return y + height;
}


void String::DrawInCenterRectOnBackground(int x, int y, int width, int height, Color::E colorText,
    int widthBorder, Color::E colorBackground)
{
    int lenght = Font::GetLengthText(buffer);
    int eX = DrawInCenterRect(x, y, width, height, colorBackground);
    int w = lenght + widthBorder * 2 - 2;
    int h = 7 + widthBorder * 2 - 1;
    Region(w, h).Fill(eX - lenght - widthBorder, y - widthBorder + 1);
    DrawInCenterRect(x, y, width, height, colorText);
}


int PText::DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, pchar text, Color::E colorBackground,
    Color::E colorFill)
{
    Rectangle(width, height).Draw(x, y, colorFill);
    Region(width - 2, height - 2).Fill(x + 1, y + 1, colorBackground);
    Color::SetCurrent(colorFill);
    return String(text).DrawInCenterRect(x, y, width, height);
}


void Painter::DrawHintsForSmallButton(int x, int y, int width, void *smallButton)
{
    SmallButton *sb = (SmallButton*)smallButton;
    Region(width, 239 - y).Fill(x, y, COLOR_BACK);
    Rectangle(width, 239 - y).Draw(x, y, COLOR_FILL);
    const StructHelpSmallButton *structHelp = &(*sb->hintUGO)[0];
    x += 3;
    y += 3;

    while (structHelp->funcDrawUGO)
    {
        Rectangle(WIDTH_SB, WIDTH_SB).Draw(x, y);
        structHelp->funcDrawUGO(x, y);
        int yNew = String(structHelp->helpUGO[LANG]).DrawInRectWithTransfers(x + 23, y + 1, width - 30, 20);
        y = ((yNew - y) < 22) ? (y + 22) : yNew;
        structHelp++;
    }
}


void PText::DrawRelativelyRight(int xRight, int y, pchar text, Color::E color)
{
    Color::SetCurrent(color);

    int lenght = Font::GetLengthText(text);
    String(text).Draw(xRight - lenght, y);
}


void PText::Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2)
{
    Char(symbol1).Draw(x, y, color1);
    Char(symbol2).Draw(x, y, color2);
}


void PText::Draw4SymbolsInRect(int x, int y, char eChar, Color::E color)
{
    Color::SetCurrent(color);

    for (int i = 0; i < 2; i++)
    {
        Char((char)(eChar + i)).Draw(x + 8 * i, y);
        Char((char)(eChar + i + 16)).Draw(x + 8 * i, y + 8);
    }
}


void PText::Draw10SymbolsInRect(int x, int y, char eChar)
{
    for (int i = 0; i < 5; i++)
    {
        Char((char)(eChar + i)).Draw(x + 8 * i, y);
        Char((char)(eChar + i + 16)).Draw(x + 8 * i, y + 8);
    }
}


void PText::DrawBig(int eX, int eY, int size, pchar text)
{
    int numSymbols = (int)std::strlen(text);

    int x = eX;

    for (int i = 0; i < numSymbols; i++)
    {
        x = DrawBigChar(x, eY, size, text[i]);
        x += size;
    }
}
