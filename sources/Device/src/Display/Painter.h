// 2022/02/11 17:45:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Display/DisplayTypes.h"
#include "Display/font/Font.h"
#include "Display/Text.h"


namespace Painter 
{
    void SetPalette(Color::E);

    void BeginScene(Color::E color);

    void EndScene(bool endScene = true);

    // По умолчанию запрашиваем первый фрейм с шрифтами
    void SendFrame(bool first, bool noFonts = false);

    void SetPoint(int x, int y);

    void DrawHLine(int y, int x0, int x1, Color::E = Color::Count);

    void DrawVLine(int x, int y0, int y1, Color::E = Color::Count);

    void DrawVPointLine(int x, int y0, int y1, float delta, Color::E);

    void DrawHPointLine(int y, int x0, int x1, float delta);

    void DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color::E);

    void DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color::E);

    void DrawLine(int x0, int y0, int x1, int y1, Color::E = Color::Count);

    // \brief Рисует прерывистую горизонтальную линию. dFill - длина штриха, dEmpty - расст. между штрихами.
    // Линия всегда начинается со штриха. dStart указывает смещение первой рисуемой точки относительно начала штриха.
    void DrawDashedHLine(int y, int x0, int x1, int dFill, int dEmpty, int dStart);

    //  Рисует прерывистую вертикальную линию.
    void DrawDashedVLine(int x, int y0, int y1, int dFill, int dEmpty, int dStart);

    void DrawRectangle(int x, int y, int width, int height, Color::E = Color::Count);

    void FillRegion(int x, int y, int width, int height, Color::E = Color::Count);

    void DrawVolumeButton(int x, int y, int width, int height, int thickness, Color::E normal, Color::E bright,
        Color::E dark, bool isPressed, bool inShade);

    uint ReduceBrightness(uint colorValue, float newBrightness);

    // Нарисовать массив вертикальных линий. Линии рисуются одна за другой. y0y1 - массив вертикальных координат.
    void DrawVLineArray(int x, int numLines, uint8 *y0y1, Color::E);

    // modeLines - true - точками, false - точками
    void DrawSignal(int x, uint8 data[281], bool modeLines);

    bool SaveScreenToFlashDrive();

    void DrawHintsForSmallButton(int x, int y, int width, void *smallButton);
};
