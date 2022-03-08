// 2022/02/15 09:14:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace PText
{
    int Draw(int x, int y, pchar text, Color::E = Color::Count);

    int DrawStringInCenterRect(int x, int y, int width, int height, pchar text, Color::E = Color::Count);

    int DrawChar(int x, int y, char symbol, Color::E = Color::Count);

    // Выводит текст на прямоугольнике цвета colorBackgound
    int DrawOnBackground(int x, int y, pchar text, Color::E colorBackground);

    int DrawWithLimitation(int x, int y, pchar  text, Color::E color, int limitX, int limitY, int limitWidth,
        int limitHeight);

    int DrawTextInRectWithTransfers(int x, int y, int width, int height, pchar text, Color::E = Color::Count);

    // Возвращает нижнюю координату прямоугольника.
    int DrawTextInBoundedRectWithTransfers(int x, int y, int width, pchar text, Color::E colorBackground,
        Color::E colorFill);

    int DrawFormatText(int x, int y, Color::E color, char *text, ...);

    // Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра
    // widthBorder цвета colorBackground.
    void DrawStringInCenterRectOnBackground(int x, int y, int width, int height, pchar text, Color::E colorText,
        int widthBorder, Color::E colorBackground);

    int DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, pchar text, Color::E colorBackground,
        Color::E colorFill);

    void DrawTextInRect(int x, int y, int width, int height, char *text);

    void DrawTextRelativelyRight(int xRight, int y, pchar text, Color::E = Color::Count);

    void Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

    void Draw4SymbolsInRect(int x, int y, char eChar, Color::E = Color::Count);

    void Draw10SymbolsInRect(int x, int y, char eChar);

    void DrawBigText(int x, int y, int size, pchar text);
}
