// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Color;


class Display
{
public:
    static const int WIDTH = 320;
    static const int HEIGHT = 240;

    static void Init();

    static void Update();

    static bool IsRun();

    static void BeginFrame(const Color &color);

    static void EndFrame();

    static bool isRun;
    static float value;
    static float direction;
    static uint timePrev;

private:

    static void DrawProgressBar(uint dT);
    static void DrawBigMNIPI();
};
