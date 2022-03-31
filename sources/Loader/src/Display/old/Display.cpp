// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "main.h"
#include "common/Display/Colors_.h"
#include "common/Display/Font/Font_.h"
#include "common/Display/Painter/Primitives_.h"
#include "common/Display/Painter/Text_.h"
#include "common/Hardware/HAL/HAL_.h"
#include "common/Utils/Math_.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include <cmath>


bool Display::isRun = false;
float Display::value = 0.0f;
uint Display::timePrev = 0;
float Display::direction = 0.0f;


struct Vector
{
    uint16 x;
    uint8 y;
};


int numPoints = 0;
#define SIZE_ARRAY 7000
Vector array[SIZE_ARRAY];


uint8 *display_back_buffer = nullptr;
uint8 *display_back_buffer_end = nullptr;


void Display::Init()
{
    value = 0.0F;
    isRun = false;
    timePrev = 0;
    direction = 10.0F;

    for (int i = 0; i < 14; i++)
    {
        float red = (float)(i) / 14.0F * 31.0F + 0.5F;
        float green = (float)(i) / 14.0F * 63.0F + 0.5F;
        float blue = (float)(i) / 14.0F * 31.0F + 0.5F;
        set.display.colors[i + 2] = Color::Make((uint8)red, (uint8)green, (uint8)blue);
    }

    Color::ResetFlash();

    Font::Set(TypeFont::S8);
}


void DrawButton(int x, int y, pchar text)
{
    int width = 25;
    int height = 20;
    Rectangle(width, height).Draw(x, y);
    Text(text).DrawInCenterRect(x, y, width + 2, height - 1);
}


void Display::Update()
{
    isRun = true;

    timePrev = TIME_MS;

    BeginFrame(Color::BLACK);

    Color::WHITE.SetAsCurrent();

    if (MainStruct::state == State::Start || MainStruct::state == State::Ok)
    {
        Display::BeginFrame(Color::BACK);
        Color::FILL.SetAsCurrent();
        Rectangle(319, 239).Draw(0, 0);
        DrawBigMNIPI();
        Color::WHITE.SetAsCurrent();
        Text("Для получения помощи нажмите и удерживайте кнопку ПОМОЩЬ").DrawInCenterRect(0, 180, 320, 20);
        Text("Отдел маркетинга: тел./факс. 8-017-270-02-00").DrawInCenterRect(0, 205, 320, 20);
        Text("Разработчики: e-mail: mnipi-24(@)tut.by, тел. 8-017-270-02-23").DrawInCenterRect(0, 220, 320, 20);
    }
    else if (MainStruct::state == State::Mounted)
    {
        uint dT = TIME_MS - timePrev;
        DrawProgressBar(dT);
    }
    else if (MainStruct::state == State::WrongFlash)
    {
        Text("НЕ УДАЛОСЬ ПРОЧИТАТЬ ДИСК").DrawInCenterRect(0, 0, 320, 200, Color::FLASH_10);
        Text("УБЕДИТЕСЬ, ЧТО ФАЙЛОВАЯ СИСТЕМА FAT32").DrawInCenterRect(0, 20, 320, 200, Color::WHITE);
    }
    else if (MainStruct::state == State::RequestAction)
    {
        Text("Обнаружено программное обеспечение").DrawInCenterRect(0, 0, 320, 200);
        Text("Установить его?").DrawInCenterRect(0, 20, 320, 200);

        DrawButton(290, 55, "ДА");
        DrawButton(290, 195, "НЕТ");
    }
    else if (MainStruct::state == State::Upgrade)
    {
        Text("Подождите завершения").DrawInCenterRect(0, 0, 320, 190);
        Text("установки программного обеспечения").DrawInCenterRect(0, 0, 320, 220);

        int height = 30;
        int fullWidth = 280;
        int width = (int)((float)(fullWidth) * MainStruct::percentUpdate);

        Region(width, height).Fill(20, 130);
        Rectangle(fullWidth, height).Draw(20, 130);
    }

    Display::EndFrame();
    isRun = false;
}


void Display::DrawProgressBar(uint dT)
{
    const int W = 300;
    const int H = 20;
    const int X = 10;
    const int Y = 200;

    float step = (float)(dT) / direction;

    value += step;

    if (direction > 0.0F && value > W)
    {
        direction = -direction;
        value -= step;
    }
    else if (direction < 0.0F && value < 0)
    {
        direction = -direction;
        value -= step;
    }

    int dH = 15;
    int y0 = 50;

    Text("Обнаружен USB-диск.").DrawInCenterRect(X, y0, W, 10, Color::WHITE);
    Text("Идёт поиск программного обеспечения").DrawInCenterRect(X, y0 + dH, W, 10);
    Text("Подождите...").DrawInCenterRect(X, y0 + 2 * dH, W, 10);

    Rectangle(W, H).Draw(X, Y);
    Region((int)(value), H).Fill(X, Y);
}


bool Display::IsRun()
{
    return isRun;
}


void Display::DrawBigMNIPI()
{
    static uint startTime = 0;
    static bool first = true;

    if (first)
    {
        first = false;
        startTime = TIME_MS;
    }

    uint time = TIME_MS - startTime;

    int numColor = Math::Limitation((int)((float)(time) / (float)TIME_WAIT * 13.0F), 0, 13);

    Color((uint8)(numColor + 2)).SetAsCurrent();

    float amplitude = 3.0F - ((float)(time) / (TIME_WAIT / 2.0F)) * 3;
    Math::LimitBelow(&amplitude, 0.0F);
    float frequency = 0.05F;

    float radius = 5000.0F * (TIME_WAIT) / 3000.0F / (float)(time);
    Math::LimitBelow(&radius, 0.0F);

    float shift[240];

    for (int i = 0; i < 240; i++)
    {
        shift[i] = amplitude * std::sin(frequency * (float)(time) + (float)(i) / 5.0F);
    }

    for (int i = 0; i < numPoints; i++)
    {
        int x = (int)((float)(array[i].x) + shift[array[i].y]); //-V537

        int y = array[i].y;

        if (x > 0 && x < 319 && y > 0 && y < 239)
        {
            Point().Draw(x, y);
        }
    }
}


void Display::BeginFrame(const Color & /*color*/)
{

}


void Display::EndFrame()
{

}
