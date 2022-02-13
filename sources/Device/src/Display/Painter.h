// 2022/02/11 17:45:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Colors.h"
#include "DisplayTypes.h"
#include "PainterC.h"


namespace Painter 
{
    void SetPalette(Color::E);

    void BeginScene(Color::E color);

    void EndScene(bool endScene = true);
    // �� ��������� ����������� ������ ����� � ��������
    void SendFrame(bool first, bool noFonts = false);

    void LoadPalette();

    void SetPoint(int x, int y);

    void DrawHLine(int y, int x0, int x1);

    void DrawHLineC(int y, int x0, int x1, Color::E);

    void DrawVLine(int x, int y0, int y1);

    void DrawVLineC(int x, int y0, int y1, Color::E);

    void DrawVPointLine(int x, int y0, int y1, float delta, Color::E);

    void DrawHPointLine(int y, int x0, int x1, float delta);

    void DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color::E);

    void DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color::E);

    void DrawLine(int x0, int y0, int x1, int y1);

    void DrawLineC(int x0, int y0, int x1, int y1, Color::E);
    // \brief ������ ����������� �������������� �����. dFill - ����� ������, dEmpty - �����. ����� ��������.
    // ����� ������ ���������� �� ������. dStart ��������� �������� ������ �������� ����� ������������ ������ ������.
    void DrawDashedHLine(int y, int x0, int x1, int dFill, int dEmpty, int dStart);
    //  ������ ����������� ������������ �����.
    void DrawDashedVLine(int x, int y0, int y1, int dFill, int dEmpty, int dStart);

    void DrawRectangle(int x, int y, int width, int height);

    void DrawRectangleC(int x, int y, int width, int height, Color::E);

    void FillRegion(int x, int y, int width, int height);

    void FillRegionC(int x, int y, int width, int height, Color::E);

    void DrawVolumeButton(int x, int y, int width, int height, int thickness, Color::E normal, Color::E bright, Color::E dark, bool isPressed, bool inShade);

    uint16 ReduceBrightness(uint16 colorValue, float newBrightness);
    // ���������� ������ ������������ �����. ����� �������� ���� �� ������. y0y1 - ������ ������������ ���������.
    void DrawVLineArray(int x, int numLines, uint8 *y0y1, Color::E);
    // modeLines - true - �������, false - �������
    void DrawSignal(int x, uint8 data[281], bool modeLines);

    bool SaveScreenToFlashDrive();

    void SetFont(TypeFont typeFont);
    // ��������� ������ � �������.
    void LoadFont(TypeFont typeFont);

    int DrawChar(int x, int y, char symbol);

    int DrawCharC(int x, int y, char symbol, Color::E);

    int DrawText(int x, int y, const char * const text);

    int DrawTextC(int x, int y, const char *text, Color::E);
    // ������� ����� �� �������������� ����� colorBackgound
    int DrawTextOnBackground(int x, int y, const char *text, Color::E colorBackground);

    int DrawTextWithLimitationC(int x, int y, const char* text, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight);
    // ���������� ������ ���������� ��������������.
    int DrawTextInBoundedRectWithTransfers(int x, int y, int width, const char *text, Color::E colorBackground, Color::E colorFill);

    int DrawTextInRectWithTransfersC(int x, int y, int width, int height, const char *text, Color::E);

    int DrawFormatText(int x, int y, Color::E color, char *text, ...);

    int DrawStringInCenterRect(int x, int y, int width, int height, const char *text);

    int DrawStringInCenterRectC(int x, int y, int width, int height, const char *text, Color::E);
    // ����� ������ ������ � ������ �������(x, y, width, height)������ ColorText �� �������������� � ������� ������� widthBorder ����� colorBackground.
    void DrawStringInCenterRectOnBackgroundC(int x, int y, int width, int height, const char *text, Color::E colorText, int widthBorder, Color::E colorBackground);

    int DrawStringInCenterRectAndBoundItC(int x, int y, int width, int height, const char *text, Color::E colorBackground, Color::E colorFill);

    void DrawHintsForSmallButton(int x, int y, int width, void *smallButton);

    void DrawTextInRect(int x, int y, int width, int height, char *text);

    void DrawTextRelativelyRight(int xRight, int y, const char *text);

    void DrawTextRelativelyRightC(int xRight, int y, const char *text, Color::E);

    void Draw2SymbolsC(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

    void Draw4SymbolsInRect(int x, int y, char eChar);

    void Draw4SymbolsInRectC(int x, int y, char eChar, Color::E);

    void Draw10SymbolsInRect(int x, int y, char eChar);
    // ����� ����� � ����������.
    int DrawTextInRectWithTransfers(int x, int y, int width, int height, const char *text);

    void DrawBigText(int x, int y, int size, const char *text);
};
