// 2022/02/11 17:45:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Display/DisplayTypes.h"
#include "Display/PainterText.h"


namespace Painter 
{
    void SetPalette(Color::E);

    void BeginScene(Color::E color);

    void EndScene(bool endScene = true);
    // По умолчанию запрашиваем первый фрейм с шрифтами
    void SendFrame(bool first, bool noFonts = false);

    void LoadPalette();

    void SetPoint(int x, int y);

    void DrawHLine(int y, int x0, int x1, Color::E = Color::COUNT);

    void DrawVLine(int x, int y0, int y1, Color::E = Color::COUNT);

    void DrawVPointLine(int x, int y0, int y1, float delta, Color::E);

    void DrawHPointLine(int y, int x0, int x1, float delta);

    void DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color::E);

    void DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color::E);

    void DrawLine(int x0, int y0, int x1, int y1, Color::E = Color::COUNT);
    // \brief Рисует прерывистую горизонтальную линию. dFill - длина штриха, dEmpty - расст. между штрихами.
    // Линия всегда начинается со штриха. dStart указывает смещение первой рисуемой точки относительно начала штриха.
    void DrawDashedHLine(int y, int x0, int x1, int dFill, int dEmpty, int dStart);
    //  Рисует прерывистую вертикальную линию.
    void DrawDashedVLine(int x, int y0, int y1, int dFill, int dEmpty, int dStart);

    void DrawRectangle(int x, int y, int width, int height, Color::E = Color::COUNT);

    void FillRegion(int x, int y, int width, int height, Color::E = Color::COUNT);

    void DrawVolumeButton(int x, int y, int width, int height, int thickness, Color::E normal, Color::E bright, Color::E dark, bool isPressed, bool inShade);

    uint16 ReduceBrightness(uint16 colorValue, float newBrightness);
    // Нарисовать массив вертикальных линий. Линии рисуются одна за другой. y0y1 - массив вертикальных координат.
    void DrawVLineArray(int x, int numLines, uint8 *y0y1, Color::E);
    // modeLines - true - точками, false - точками
    void DrawSignal(int x, uint8 data[281], bool modeLines);

    bool SaveScreenToFlashDrive();

    void SetFont(TypeFont::E typeFont);
    // Загрузить шрифта в дисплей.
    void LoadFont(TypeFont::E typeFont);

    int DrawChar(int x, int y, char symbol, Color::E = Color::COUNT);

    int DrawText(int x, int y, pchar text, Color::E = Color::COUNT);
    // Выводит текст на прямоугольнике цвета colorBackgound
    int DrawTextOnBackground(int x, int y, pchar text, Color::E colorBackground);

    int DrawTextWithLimitationC(int x, int y, pchar  text, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight);
    // Возвращает нижнюю координату прямоугольника.
    int DrawTextInBoundedRectWithTransfers(int x, int y, int width, pchar text, Color::E colorBackground, Color::E colorFill);

    int DrawTextInRectWithTransfers(int x, int y, int width, int height, pchar text, Color::E = Color::COUNT);

    int DrawFormatText(int x, int y, Color::E color, char *text, ...);

    int DrawStringInCenterRect(int x, int y, int width, int height, pchar text, Color::E = Color::COUNT);
    // Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра widthBorder цвета colorBackground.
    void DrawStringInCenterRectOnBackground(int x, int y, int width, int height, pchar text, Color::E colorText, int widthBorder, Color::E colorBackground);

    int DrawStringInCenterRectAndBoundIt(int x, int y, int width, int height, pchar text, Color::E colorBackground, Color::E colorFill);

    void DrawHintsForSmallButton(int x, int y, int width, void *smallButton);

    void DrawTextInRect(int x, int y, int width, int height, char *text);

    void DrawTextRelativelyRight(int xRight, int y, pchar text, Color::E = Color::COUNT);

    void Draw2Symbols(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

    void Draw4SymbolsInRect(int x, int y, char eChar, Color::E = Color::COUNT);

    void Draw10SymbolsInRect(int x, int y, char eChar);

    void DrawBigText(int x, int y, int size, pchar text);
};
