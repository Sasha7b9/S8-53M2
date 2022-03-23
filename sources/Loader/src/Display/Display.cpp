// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Display.h"
#include "Painter.h"
#include "Hardware/Timer.h"
#include "main.h"
#include "Utils/Math.h"
#include <math.h>



typedef struct
{
    uint16 x;
    uint8 y;
} Vector;


int numPoints = 0;
#define SIZE_ARRAY 7000
Vector array[SIZE_ARRAY];



static void DrawProgressBar(uint dT);
static void DrawBigMNIPI();
static void InitPoints();
//static void DrawSeconds();
//static void DrawFrames();



void InitHardware()
{
    GPIO_InitTypeDef isGPIO_ =
    {
        GPIO_PIN_11,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SPEED_HIGH,
        GPIO_AF0_MCO,
    };
    // Сигнал готовности дисплея  к приёму команды
    HAL_GPIO_Init(GPIOG, &isGPIO_);
}


void Display_Init()
{
    ms->display.value = 0.0f;
    ms->display.isRun = false;
    ms->display.timePrev = 0;
    ms->display.direction = 10.0f;

    gColorBack = Color::BLACK;
    gColorFill = Color::WHITE;

    Painter_ResetFlash();

    InitHardware();

    Painter_SetFont(TypeFont_8);
    
    InitPoints();
}


void DrawButton(int x, int y, char *text)
{
    int width = 25;
    int height = 20;
    Painter_DrawRectangle(x, y, width, height);
    Painter_DrawStringInCenterRect(x, y, width + 2, height - 1, text);
}


void Display_Update1()
{
    static uint min = 1000;
    static uint max = 0;
    static uint current = 0;

    uint time = HAL_GetTick();

    Painter_BeginScene(Color::BLACK);

    Painter_DrawTextFormatting(5, 200, Color::WHITE, "%f секунд", HAL_GetTick() / 1000.0f);

//    Painter_DrawTextFormatting(5, 210, Color::WHITE, "%d frames", numFrames);

    Painter_DrawTextFormatting(5, 220, Color::WHITE, "min = %d max = %d, current = %d", min , max, current);

    Painter_EndScene();

    current = HAL_GetTick() - time;

    if (current < min)
    {
        min = current;
    }
    if (current > max)
    {
        max = current;
    }
}


void Display_Update()
{
    ms->display.isRun = true;

    uint dT = HAL_GetTick() - ms->display.timePrev;
    ms->display.timePrev = HAL_GetTick();

    Painter_BeginScene(Color::BLACK);

    Painter_SetColor(Color::WHITE);

    if (ms->state == State_Start || ms->state == State_Ok)
    {
        Painter_BeginScene(gColorBack);
        Painter_SetColor(gColorFill);
        Painter_DrawRectangle(0, 0, 319, 239);
        DrawBigMNIPI();
        Painter_SetColor(Color::WHITE);
        Painter_DrawStringInCenterRect(0, 180, 320, 20, "Для получения помощи нажмите и удерживайте кнопку ПОМОЩЬ");
        Painter_DrawStringInCenterRect(0, 205, 320, 20, "Отдел маркетинга: тел./факс. 8-017-270-02-00");
        Painter_DrawStringInCenterRect(0, 220, 320, 20, "Разработчики: e-mail: mnipi-24(@)tut.by, тел. 8-017-270-02-23");
    }
    else if (ms->state == State_Mount)
    {
        DrawProgressBar(dT);
    }
    else if (ms->state == State_WrongFlash)
    {
        Painter_DrawStringInCenterRectC(0, 0, 320, 200, "НЕ УДАЛОСЬ ПРОЧИТАТЬ ДИСК", Color::FLASH_10);
        Painter_DrawStringInCenterRectC(0, 20, 320, 200, "УБЕДИТЕСЬ, ЧТО ФАЙЛОВАЯ СИСТЕМА FAT32", Color::WHITE);
    }
    else if (ms->state == State_RequestAction)
    {
        Painter_DrawStringInCenterRect(0, 0, 320, 200, "Обнаружено программное обеспечение");
        Painter_DrawStringInCenterRect(0, 20, 320, 200, "Установить его?");

        DrawButton(290, 55, "ДА");
        DrawButton(290, 195, "НЕТ");
    }
    else if (ms->state == State_Upgrade)
    {
        Painter_DrawStringInCenterRect(0, 0, 320, 190, "Подождите завершения");
        Painter_DrawStringInCenterRect(0, 0, 320, 220, "установки программного обеспечения");

        int height = 30;
        int fullWidth = 280;
        int width = (int)(fullWidth * ms->percentUpdate);

        Painter_FillRegion(20, 130, width, height);
        Painter_DrawRectangle(20, 130, fullWidth, height);
    }

    //DrawFrames();
    //DrawSeconds();

    Painter_EndScene();
    ms->display.isRun = false;
}


void DrawProgressBar(uint dT)
{
    const int WIDTH = 300;
    const int HEIGHT = 20;
    const int X = 10;
    const int Y = 200;

    float step = dT / ms->display.direction;

    ms->display.value += step;

    if (ms->display.direction > 0.0f && ms->display.value > WIDTH)
    {
        ms->display.direction = -ms->display.direction;
        ms->display.value -= step;
    }
    else if (ms->display.direction < 0.0f && ms->display.value < 0)
    {
        ms->display.direction = -ms->display.direction;
        ms->display.value -= step;
    }

    int dH = 15;
    int y0 = 50;

    Painter_DrawStringInCenterRectC(X, y0, WIDTH, 10, "Обнаружен USB-диск.", Color::WHITE);
    Painter_DrawStringInCenterRect(X, y0 + dH, WIDTH, 10, "Идёт поиск программного обеспечения");
    Painter_DrawStringInCenterRect(X, y0 + 2 * dH, WIDTH, 10, "Подождите...");

    Painter_DrawRectangle(X, Y, WIDTH, HEIGHT);
    Painter_FillRegion(X, Y, (int)ms->display.value, HEIGHT);
}


bool Display_IsRun()
{
    return ms->display.isRun;
}


static void DrawBigMNIPI()
{
    static uint startTime = 0;
    static bool first = true;

    if (first)
    {
        first = false;
        startTime = HAL_GetTick();
    }

    uint time = HAL_GetTick() - startTime;

    int numColor = 0;
    LIMITATION(numColor, (int)(time / (float)TIME_WAIT * 13.0f), 0, 13);
    Painter_SetColor((Color::E)(numColor + 2));

    float amplitude = 3.0f - (time / (TIME_WAIT / 2.0f)) * 3;
    LIMIT_BELOW(amplitude, 0.0f);
    float frequency = 0.05f;

    float radius = 5000.0f * (TIME_WAIT) / 3000.0f / time;
    LIMIT_BELOW(radius, 0);

    float shift[240];

    for (int i = 0; i < 240; i++)
    {
        shift[i] = amplitude * sin(frequency * time + i / 5.0f);
    }

    for (int i = 0; i < numPoints; i++)
    {
        int x = (int)(array[i].x + shift[array[i].y]);
        int y = array[i].y;
        if (x > 0 && x < 319 && y > 0 && y < 239)
        {
            Painter_SetPoint(x, y);
        }
    }
}


static void InitPoints()
{
    uint8 buffer[320][240];

    Painter_DrawBigTextInBuffer(31, 70, 9, "МНИПИ", buffer);

    for (int x = 0; x < 320; x++)
    {
        for (int y = 0; y < 240; y++)
        {
            if (buffer[x][y])
            {
                array[numPoints].x = (uint16)x;
                array[numPoints].y = (uint8)y;
                numPoints++;
            }
        }
    }
}
