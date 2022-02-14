// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "DisplayTypes.h"


void Painter_SetFont(TypeFont typeFont);
// Загрузить шрифта в дисплей
void Painter_LoadFont(TypeFont typeFont);

int Painter_DrawChar(int x, int y, char symbol);

int Painter_DrawCharC(int x, int y, char symbol, Color::E);

int Painter_DrawText(int x, int y, pchar text);

int Painter_DrawTextC(int x, int y, pchar text, Color::E);

int Painter_DrawTextWithLimitationC(int x, int y, const char* text, Color::E color, int limitX, int limitY, int limitWidth, int limitHeight);

int Painter_DrawTextInRectWithTransfersC(int x, int y, int width, int height, pchar text, Color::E);

int Painter_DrawTextFormatting(int x, int y, Color::E color, char *text, ...);

int Painter_DrawStringInCenterRect(int x, int y, int width, int height, pchar text);

int Painter_DrawStringInCenterRectC(int x, int y, int width, int height, pchar text, Color::E);
// Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра widthBorder цвета colorBackground
void Painter_DrawStringInCenterRectOnBackgroundC(int x, int y, int width, int height, pchar text, Color::E colorText, int widthBorder, Color::E colorBackground);

int Painter_DrawStringInCenterRectAndBoundItC(int x, int y, int width, int height, pchar text, Color::E colorBackground, Color::E colorFill);

void Painter_DrawTextInRect(int x, int y, int width, char *text);

void Painter_DrawTextRelativelyRight(int xRight, int y, pchar text);

void Painter_Draw2SymbolsC(int x, int y, char symbol1, char symbol2, Color::E color1, Color::E color2);

void Painter_Draw4SymbolsInRect(int x, int y, char eChar);

void Painter_Draw10SymbolsInRect(int x, int y, char eChar);
// Пишет текст с переносами
int Painter_DrawTextInRectWithTransfers(int x, int y, int width, int height, pchar text);

void Painter_DrawBigText(int x, int y, int size, pchar text);

void Painter_DrawBigTextInBuffer(int x, int y, int size, pchar text, uint8 buffer[320][240]);
