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
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cstdlib>


//pchar const DString::_ERROR = "---.---";


namespace PText
{
    // Отрисовка непосредственно символа
    int DrawCharHard(int x, int y, char symbol);

    int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight);

    // Возвращает высоту экрана, которую займёт текст text, при выводе от left до right в переменной height. Если
    // bool == false, то текст не влезет на экран 
    bool GetHeightTextWithTransfers(int left, int top, int right, pchar text, int *height);

    bool ByteFontNotEmpty(int eChar, int byte);

    bool BitInFontIsExist(int eChar, int numByte, int bit);

    void DrawCharInColorDisplay(int eX, int eY, uchar symbol);

    int DrawBigChar(int eX, int eY, int size, char symbol);

    bool IsLetter(char symbol);

    bool IsConsonant(char symbol);

    // Находит следующий перенос. C letters начинается часть слово, где нужно найти перенос, в lettersInSyllable будет
    // записано число букв в найденном слоге. Если слово закончилось, функция возвращает false
    bool FindNextTransfer(char *letters, int8 *lettersInSyllable);

    int8 *BreakWord(char *word);

    bool CompareArrays(const bool *array1, const bool *array2, int numElems);

    // Возвращает часть слова до слога numSyllable(включительн) вместе со знаком переноса
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
        CommandBuffer<4> command(SET_FONT);
        command.PushByte(typeFont);
        command.Transmit(2);
    }
}


void Font::Load(TypeFont::E typeFont)
{
    if (InterCom::TransmitGUIinProcess())
    {
        uint8 *pFont = (uint8 *)Font::fonts[typeFont];

        CommandBuffer<2 + 4> command(LOAD_FONT);
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
    static uint8 prevByte = 0;      // WARN здесь точно статики нужны?
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


int DString::DrawWithLimitation(int x, int y, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight)
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


String<> PText::GetWord(pchar firstSymbol, int *length)
{
    *length = 0;

    String<> result;

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

#define VOWEL       0   // Гласная
#define CONSONANT   1   // Согласная

    *lettersInSyllable = (int8)std::strlen(letters);

    if (std::strlen(letters) <= 3) //-V1051
    {
        return false;
    }

    static const bool template1[3] = {false, true, true};               //     011     2   // После второго символа перенос
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
    if (std::strcmp(word, "структуру") == 0)
    {
        int8 lengths[] = {5, 2, 2, 0};
        std::memcpy(lengthSyllables, lengths, 4);
    }
    else if (std::strcmp(word, "соответствующей") == 0)
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
                DString(subString).Draw(x, y);
            }

            return (int)std::strlen(subString) - 1;
        }
    }

    return 0;
}


int DString::DrawInRectWithTransfers(int eX, int eY, int eWidth, int eHeight, Color::E color)
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
            String<> word = PText::GetWord(text + curSymbol, &length);

            if (length <= 1)                            // Нет буквенных символов или один, т.е. слово не найдено
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
            else                                            // А здесь найдено по крайней мере два буквенных символа, т.е. найдено слово
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
                    x = String<>(word).Draw(x, y);
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
            String<> word = GetWord(text + curSymbol, &length);

            if (length <= 1)                            // Нет буквенных символов или один, т.е. слово не найдено
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
            else                                            // А здесь найдено по крайней мере два буквенных символа, т.е. найдено слово
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


int DString::DrawInBoundedRectWithTransfers(int x, int y, int width, Color::E colorBackground, Color::E colorFill)
{
    int height = 0;

    PText::GetHeightTextWithTransfers(x + 3, y + 3, x + width - 8, buffer, &height);

    Rectangle(width, height).Draw(x, y, colorFill);
    Region(width - 2, height - 2).Fill(x + 1, y + 1, colorBackground);
    DrawInRectWithTransfers(x + 3, y + 3, width - 8, height, colorFill);
    return y + height;
}


void DString::DrawInCenterRectOnBackground(int x, int y, int width, int height, Color::E colorText,
    int widthBorder, Color::E colorBackground)
{
    int lenght = Font::GetLengthText(buffer);
    int eX = DrawInCenterRect(x, y, width, height, colorBackground);
    int w = lenght + widthBorder * 2 - 2;
    int h = 7 + widthBorder * 2 - 1;
    Region(w, h).Fill(eX - lenght - widthBorder, y - widthBorder + 1);
    DrawInCenterRect(x, y, width, height, colorText);
}


int DString::DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, Color::E colorBackground,
    Color::E colorFill)
{
    Rectangle(width, height).Draw(x, y, colorFill);
    Region(width - 2, height - 2).Fill(x + 1, y + 1, colorBackground);
    Color::SetCurrent(colorFill);
    return DrawInCenterRect(x, y, width, height);
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
        int yNew = DString(structHelp->helpUGO[LANG]).DrawInRectWithTransfers(x + 23, y + 1, width - 30, 20);
        y = ((yNew - y) < 22) ? (y + 22) : yNew;
        structHelp++;
    }
}


void DString::DrawRelativelyRight(int xRight, int y, Color::E color)
{
    Color::SetCurrent(color);

    int lenght = Font::GetLengthText(buffer);
    Draw(xRight - lenght, y);
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


DString::DString() : buffer(nullptr), capacity(0)
{
    SetString("");
}


DString::DString(const DString &rhs) : buffer(nullptr), capacity(0)
{
    SetString(rhs);
}


DString::DString(char symbol) : buffer(nullptr), capacity(0)
{
    if (Allocate(2))
    {
        buffer[0] = symbol;
        buffer[1] = 0;
    }
}


DString::DString(pchar format, ...) : buffer(nullptr), capacity(0)
{
    char temp_buffer[1024];

    std::va_list args;
    va_start(args, format);
    int num_symbols = std::vsprintf(temp_buffer, format, args);
    va_end(args);

    if (capacity < num_symbols)
    {
        Allocate(num_symbols);
    }

    if (buffer)
    {
        std::strcpy(buffer, temp_buffer);
    }
}


void DString::SetFormat(pchar format, ...)
{
    char temp_buffer[1024];

    std::va_list args;
    va_start(args, format);
    int num_symbols = std::vsprintf(temp_buffer, format, args);
    va_end(args);

    if (num_symbols == 0)
    {
        num_symbols = 2;
    }

    if (capacity < num_symbols)
    {
        Allocate(num_symbols);
    }

    if (buffer)
    {
        std::strcpy(buffer, temp_buffer);
    }
}


void DString::SetString(pchar string)
{
    Allocate((int)std::strlen(string) + 1);

    std::strcpy(buffer, string);
}


void DString::SetString(const DString &str)
{
    SetString(str.c_str());
}


void DString::Append(pchar str)
{
    Append(str, (int)std::strlen(str));
}


void DString::Append(const DString &str)
{
    Append(str.c_str());
}


void DString::Append(pchar str, int num_symbols)
{
    if (!str || *str == '\0')
    {
        return;
    }

    int need_size = Size() + num_symbols + 1;

    if (capacity < need_size)
    {
        DString old(*this);
        Allocate(need_size);
        std::strcpy(buffer, old.c_str());
    }

    std::memcpy(buffer + std::strlen(buffer), str, (uint)num_symbols);
    buffer[need_size - 1] = '\0';
}


void DString::Append(char symbol)
{
    char b[2] = {symbol, '\0'};
    Append(b);
}


DString::~DString()
{
    Free();
}


void DString::Free()
{
    if (buffer)
    {
        std::free(buffer);
        buffer = nullptr;
        capacity = 0;
    }
}


bool DString::Allocate(int size)
{
    std::free(buffer);

    capacity = NeedMemory(size);

    buffer = (char *)(std::malloc((uint)capacity));

    if (buffer)
    {
        return true;
    }

    capacity = 0;

    return false;
}


int DString::NeedMemory(int size)
{
    if (size % SIZE_SEGMENT == 0)
    {
        return size;
    }
    else
    {
        return (size / SIZE_SEGMENT) * SIZE_SEGMENT + SIZE_SEGMENT;
    }
}


int DString::Size() const
{
    if (buffer == nullptr)
    {
        return 0;
    }

    return (int)std::strlen(buffer);
}


char &DString::operator[](int i) const
{
    static char result = 0;

    if (buffer == nullptr || Size() < (int)i)
    {
        return result;
    }

    return buffer[i];
}


bool DString::ToInt(int *out)
{
    char *str = buffer;

    int sign = str[0] == '-' ? -1 : 1;

    if (str[0] < '0' || str[0] > '9')
    {
        ++str;
    }

    int length = (int)(std::strlen(str));

    if (length == 0)
    {
        return false;
    }

    *out = 0;

    int pow = 1;

    for (int i = length - 1; i >= 0; i--)
    {
        int val = str[i] & (~(0x30));
        if (val < 0 || val > 9)
        {
            return false;
        }
        *out += val * pow;
        pow *= 10;
    }

    if (sign == -1)
    {
        *out *= -1;
    }

    return true;
}


DString &DString::operator=(const DString &rhs)
{
    SetString(rhs);

    return *this;
}


int DString::Draw(int x, int y, Color::E color)
{
    Color::SetCurrent(color);

    if (Size() == 0)
    {
        return x;
    }

    y += (8 - Font::GetSize());

    pchar text = c_str();

    if (InterCom::TransmitGUIinProcess())
    {
        const int SIZE_BUFFER = 100;

        CommandBuffer<SIZE_BUFFER> command(DRAW_TEXT);
        command.PushHalfWord(x);
        command.PushByte(y + 1);
        command.PushByte(0);

        uint8 length = 0;

        int counter = 0;
        while (*text && length < (SIZE_BUFFER - 7))
        {
            command.PushByte(*text);
            text++;
            length++;
            counter++;
        }

        if (*text != 0)
        {
            LOG_WRITE("big string - %s", text);
        }

        command.PushByte(0);
        *command.GetByte(4) = length;

        command.Transmit(1 + 2 + 1 + 1 + length);
    }

    text = c_str();

    while (*text != '\0')
    {
        x = Char(*text).Draw(x, y) + 1;
        text++;
    }

    return x;
}


int DString::DrawInCenterRect(int eX, int eY, int width, int eHeight, Color::E color)
{
    Color::SetCurrent(color);

    int lenght = Font::GetLengthText(buffer);
    int height = Font::GetHeightSymbol(buffer[0]);
    int x = eX + (width - lenght) / 2;
    int y = eY + (eHeight - height) / 2;

    return Draw(x, y);
}


void DString::DrawInRect(int x, int y, int width, int, int dy)
{
    int xStart = x;
    int xEnd = xStart + width;

    char *text = buffer;

    while (*text != 0)
    {
        int length = GetLenghtSubString(text);

        if (length + x > xEnd)
        {
            x = xStart;
            y += Font::GetHeightSymbol(*text);
            y += dy;
        }

        int numSymbols = 0;
        numSymbols = DrawSubString(x, y, text);
        text += numSymbols;
        x += length;
        x = DrawSpaces(x, y, text, &numSymbols);
        text += numSymbols;
    }
}


int DString::GetLenghtSubString(char *text)
{
    int result = 0;

    while (((*text) != ' ') && ((*text) != '\0'))
    {
        result += Font::GetLengthSymbol((uint8)*text);
        text++;
        result++;
    }

    return result;
}


int DString::DrawSubString(int x, int y, char *text)
{
    int numSymbols = 0;

    while (((*text) != ' ') && ((*text) != '\0'))
    {
        x = Char(*text).Draw(x, y);
        numSymbols++;
        text++;
        x++;
    }

    return numSymbols;
}


int DString::DrawSpaces(int x, int y, char *text, int *numSymbols)
{
    *numSymbols = 0;

    while (*text == ' ')
    {
        x = Char(*text).Draw(x, y);
        text++;
        (*numSymbols)++;
    }

    return x;
}
