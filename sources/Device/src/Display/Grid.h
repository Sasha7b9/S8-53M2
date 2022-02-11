// 2022/02/11 17:44:39 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


class Grid
{
public:
    static int Left();
    static int Right();
    static int Width();
    static float DeltaY();
    static float DeltaX();

    static int WidthInCells();

    static int FullBottom();
    static int FullHeight();
    static int FullCenterHeight();

    static int ChannelBottom();
    static int ChannelHeight();
    static int ChannelCenterHeight();

    static int MathHeight();
    static int MathBottom();
    static int MathTop();

    static int BottomMessages();

private:
    static int SignalWidth();
};
