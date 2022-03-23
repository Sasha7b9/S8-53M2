// 2022/02/15 09:14:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace PText
{
    int DrawInRectWithTransfers(int x, int y, int width, int height, pchar text, Color::E = Color::Count);

    // Возвращает нижнюю координату прямоугольника.
    int DrawInBoundedRectWithTransfers(int x, int y, int width, pchar text, Color::E colorBackground,
        Color::E colorFill);

    // Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра
    // widthBorder цвета colorBackground.
    void DrawStringInCenterRectOnBackground(int x, int y, int width, int height, pchar text, Color::E colorText,
        int widthBorder, Color::E colorBackground);

    int DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, pchar text, Color::E colorBackground,
        Color::E colorFill);

    void DrawRelativelyRight(int xRight, int y, pchar text, Color::E = Color::Count);

    void Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

    void Draw4SymbolsInRect(int x, int y, char eChar, Color::E = Color::Count);

    void Draw10SymbolsInRect(int x, int y, char eChar);

    void DrawBig(int x, int y, int size, pchar text);
}
