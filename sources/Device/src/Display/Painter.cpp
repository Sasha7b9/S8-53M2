// 2022/02/11 17:44:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Display/font/Font.h"
#include "Log.h"
#include "LAN/LAN.h"
#include "LAN/SocketTCP.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Hardware/VCP/VCP.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "FDrive/FDrive.h"
#include "Menu/FileManager.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/InterCom.h"
#include <cstring>


bool     Color::inverse = false;
Color::E Color::current = Color::Count;


namespace Display
{
    static const int SIZE_BUFFER = WIDTH * HEIGHT;
    uint8 back[240][320];                                           // Здесь будем рисовать изображение

    uint8 * const back_buffer = &back[0][0];
    uint8 * const back_buffer_end = back_buffer + SIZE_BUFFER;      // Конец буфера отрисовки
}


namespace Painter
{
    bool noFonts = false;

    void BoundingX(int &x) { if (x < 0) x = 0; if (x >= Display::WIDTH) x = Display::WIDTH - 1; }
    void BoundingY(int &y) { if (y < 0) y = 0; if (y >= Display::HEIGHT) y = Display::HEIGHT - 1; }

    Color::E GetColor(int x, int y);
}


static enum StateTransmit
{
    StateTransmit_Free,
    StateTransmit_NeedForTransmitFirst,  // Это когда нужно передать первый кадр - передаются шрифты
    StateTransmit_NeedForTransmitSecond, // Это когда нужно передать второй и последующий кадры - шрифты не передаются
    StateTransmit_InProcess
} stateTransmit = StateTransmit_Free;


void Painter::SendFrame(bool first, bool noFonts_)
{
    noFonts = noFonts_;

    if (stateTransmit == StateTransmit_Free)
    {
        stateTransmit = (first ? StateTransmit_NeedForTransmitFirst : StateTransmit_NeedForTransmitSecond);
    }
}


void Color::CalculateColor()
{
    if (current == Color::FLASH_10)
    {
        current = inverse ? COLOR_BACK : COLOR_FILL;
    }
    else if (current == Color::FLASH_01)
    {
        current = inverse ? COLOR_FILL : COLOR_BACK;
    }
}


void Color::OnTimerFlashDisplay()
{
    inverse = !inverse;
}


void Painter::DrawRectangle(int x, int y, int width, int height, Color::E color)
{
    Color::SetCurrent(color);

    DrawHLine(y, x, x + width);
    DrawVLine(x, y, y + height);
    DrawHLine(y + height, x, x + width);

    if (x + width < SCREEN_WIDTH)
    {
        DrawVLine(x + width, y, y + height);
    }
}


void Painter::DrawDashedVLine(int x, int y0, int y1, int deltaFill, int deltaEmtpy, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmtpy))
    {
        LOG_ERROR_TRACE("Неправильный аргумент deltaStart = %d", deltaStart);
        return;
    }
    int y = y0;
    if (deltaStart != 0)                 // Если линию нужно рисовать не с начала штриха
    {
        y += (deltaFill + deltaEmtpy - deltaStart);
        if (deltaStart < deltaFill)     // Если начало линии приходится на штрих
        {
            DrawVLine(x, y0, y - 1);
        }
    }

    while (y < y1)
    {
        DrawVLine(x, y, y + deltaFill - 1);
        y += (deltaFill + deltaEmtpy);
    }
}


void Painter::DrawDashedHLine(int y, int x0, int x1, int deltaFill, int deltaEmpty, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmpty))
    {
        LOG_ERROR_TRACE("Неправильный аргумент deltaStart = %d", deltaStart);
        return;
    }
    int x = x0;
    if (deltaStart != 0)                // Если линию нужно рисовать не с начала штриха
    {
        x += (deltaFill + deltaEmpty - deltaStart);
        if (deltaStart < deltaFill)     // Если начало линии приходится на штрих
        {
            DrawHLine(y, x0, x - 1);
        }
    }

    while (x < x1)
    {
        DrawHLine(y, x, x + deltaFill - 1);
        x += (deltaFill + deltaEmpty);
    }
}


void InterCom::Send(const uint8 *pointer, int size)
{
    VCP::Send(pointer, size);
    SocketTCP::Send((pchar )pointer, (uint)size);
}


bool InterCom::TransmitGUIinProcess()
{
    return (stateTransmit == StateTransmit_InProcess);
}


void Color::SetCurrent(Color::E color)
{
    if (color != current && color != Color::Count)
    {
        current = color;

        if (current > Color::Count)
        {
            CalculateColor();
        }

        if (InterCom::TransmitGUIinProcess())
        {
            CommandBuffer command(4, SET_COLOR);
            command.PushByte(color);
            command.Transmit(2);
        }
    }
}


Color::E Color::GetCurrent()
{
    return current;
}


void Painter::DrawHLine(int y, int x0, int x1, Color::E color)
{
    Color::SetCurrent(color);

    if (y < 0 || y >= Display::HEIGHT)
    {
        return;
    }

    BoundingX(x0);
    BoundingX(x1);

    if (x0 > x1)
    {
        Math::Swap(&x0, &x1);
    }

    uint8 *start = Display::back_buffer + y * Display::WIDTH + x0;

    std::memset(start, Color::GetCurrent(), (uint)(x1 - x0 + 1));

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(8, DRAW_HLINE);
        command.PushByte(y);
        command.PushHalfWord(x0);
        command.PushHalfWord(x1);
        command.Transmit(6);
    }
}


void Painter::DrawVLine(int x, int y0, int y1, Color::E color)
{
    Color::SetCurrent(color);

    if (x < 0 || x >= Display::WIDTH)
    {
        return;
    }

    BoundingY(y0);
    BoundingY(y1);

    Math::Sort(&y0, &y1);

    uint8 *address = Display::back_buffer + Display::WIDTH * y0 + x;

    int counter = y1 - y0 + 1;

    uint8 value = (uint8)Color::GetCurrent();

    do
    {
        *address = value;
        address += Display::WIDTH;

    } while (--counter > 0);

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(8, DRAW_VLINE);
        command.PushHalfWord(x);
        command.PushByte(y0);
        command.PushByte(y1);
        command.Transmit(5);
    }
}


void Painter::DrawVPointLine(int x, int y0, int y1, float delta, Color::E color)
{
    Color::SetCurrent(color);

    for (int y = y0; y <= y1; y += (int)delta)
    {
        SetPoint(x, y);
    }
}


void Painter::DrawHPointLine(int y, int x0, int x1, float delta)
{
    for (int x = x0; x <= x1; x += (int)delta)
    {
        SetPoint(x, y);
    }
}


void Painter::SetPoint(int x, int y)
{
    if (x < 0 || y < 0 || (x > Display::WIDTH - 1) || (y > Display::HEIGHT - 1))
    {
        return;
    }

    uint8 *address = Display::back_buffer + Display::WIDTH * y + x;

    if (address < Display::back_buffer_end)
    {
        *address = (uint8)Color::GetCurrent();
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(4, SET_POINT);
        command.PushHalfWord(x);
        command.PushByte(y);
        command.Transmit(4);
    }
}


Color::E Painter::GetColor(int x, int y)
{
    if (x < 0 || y < 0 || (x > Display::WIDTH - 1) || (y > Display::HEIGHT - 1))
    {
        return COLOR_BACK;
    }

    uint8 *address = Display::back_buffer + Display::WIDTH * y + x;

    if (address < Display::back_buffer_end)
    {
        return (Color::E)*address;
    }

    return COLOR_BACK;
}


void Painter::DrawMultiVPointLine(int numLines, const int y, const uint16 x[], int delta, int count, Color::E color) 
{
    numLines = Math::Limitation<int>(numLines, 0, 20);

    Color::SetCurrent(color);

    for (int i = 0; i < numLines; i++)
    {
        DrawVPointLine(x[i], y, count, delta);
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(60, DRAW_MULTI_VPOINT_LINES);
        command.PushByte(numLines);
        command.PushByte(y);
        command.PushByte(count);
        command.PushByte(delta);
        command.PushByte(0);
        for (int i = 0; i < numLines; i++)
        {
            command.PushHalfWord(x[i]);
        }
        int numBytes = 1 + 1 + 1 + numLines * 2 + 1 + 1;
        while (numBytes % 4)
        {
            numBytes++;
        }

        command.Transmit(1 + 1 + 1 + 1 + 1 + 1 + numLines * 2);
    }
}


void Painter::DrawVPointLine(int x, int y, int count, int delta)
{
    for (int i = 0; i < count; i++)
    {
        SetPoint(x, y);
        y += delta;
    }
}


void Painter::DrawMultiHPointLine(int numLines, int x, const uint8 y[], int delta, int count, Color::E color)
{
    numLines = Math::Limitation<int>(numLines, 0, 20);

    Color::SetCurrent(color);

    for (int i = 0; i < numLines; i++)
    {
        DrawHPointLine(x, y[i], count, delta);
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(30, DRAW_MULTI_HPOINT_LINES_2);
        command.PushByte(numLines);
        command.PushHalfWord(x);
        command.PushByte(count);
        command.PushByte(delta);
        for (int i = 0; i < numLines; i++)
        {
            command.PushByte(y[i]);
        }
        int numBytes = 1 +      // command
            1 +                 // numLines
            2 +                 // x
            numLines +          // numLines
            1 +
            1;
        while (numBytes % 4)
        {
            numBytes++;
        }

        command.Transmit(1 + 1 + 2 + 1 + 1 + numLines);
    }
}


void Painter::DrawHPointLine(int x, int y, int count, int delta)
{
    for (int i = 0; i < count; i++)
    {
        SetPoint(x, y);
        x += delta;
    }
}


void Painter::DrawLine(int x0, int y0, int x1, int y1, Color::E color)
{
    Color::SetCurrent(color);

    if (x0 == x1)
    {
        DrawVLine(x0, y0, y1);
    }
    else if (y0 == y1)
    {
        DrawHLine(y0, x0, x1);
    }
}


void Painter::FillRegion(int x, int y, int width, int height, Color::E color)
{
    Color::SetCurrent(color);

    if (width == 0 || height == 0)
    {
        return;
    }

    for (int i = y; i <= y + height; i++)
    {
        DrawHLine(i, x, x + width);
    }

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(8, FILL_REGION);
        command.PushHalfWord(x);
        command.PushByte(y);
        command.PushHalfWord(width);
        command.PushByte(height);
        command.Transmit(7);
    }
}


void Painter::DrawVolumeButton(int x, int y, int width, int height, int thickness, Color::E normal, Color::E bright,
    Color::E dark, bool isPressed, bool inShade)
{
    if (inShade)
    {
        thickness = 1;
    }
    FillRegion(x + thickness, y + thickness, width - thickness * 2, height - thickness * 2, normal);
    if (isPressed || inShade)
    {
        for (int i = 0; i < thickness; i++)
        {
            DrawHLine(y + i, x + i, x + width - i, dark);
            DrawVLine(x + i, y + 1 + i, y + height - i);
            DrawVLine(x + width - i, y + 1 + i, y + height - i, bright);
            DrawHLine(y + height - i, x + 1 + i, x + width - i);
        }
    }
    else
    {
        for (int i = 0; i < thickness; i++)
        {
            DrawHLine(y + i, x + i, x + width - i, bright);
            DrawVLine(x + i, y + 1 + i, y + height - i);
            DrawVLine(x + width - i, y + 1 + i, y + height - i, dark);
            DrawHLine(y + height - i, x + 1 + i, x + width - i);
        }
    }
}


void Painter::DrawVLineArray(int x, int num_lines, uint8 *y0y1, Color::E color, uint8)
{
    Color::SetCurrent(color);

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(255 * 2 + 4 + 4, DRAW_VLINES_ARRAY);
        command.PushHalfWord(x);

        if (num_lines > 255)
        {
            num_lines = 255;
        }

        command.PushHalfWord(num_lines);

        for (int i = 0; i < num_lines; i++)
        {
            command.PushByte(*(y0y1 + i * 2));
            command.PushByte(*(y0y1 + i * 2 + 1));
        }

        int numBytes = num_lines * 2 + 4;

        while (numBytes % 4)
        {
            numBytes++;
        }

        command.Transmit(1 + 2 + 1 + 2 * num_lines);
    }

    for (; num_lines > 0; num_lines--)
    {
        int y0 = *y0y1++;
        int y1 = *y0y1++;

        DrawVLine(x++, y0, y1);
    }
}


void Painter::BeginScene(Color::E color)
{
    if (stateTransmit == StateTransmit_NeedForTransmitFirst || stateTransmit == StateTransmit_NeedForTransmitSecond)
    {
        bool needForLoadFontsAndPalette = stateTransmit == StateTransmit_NeedForTransmitFirst;
        stateTransmit = StateTransmit_InProcess;
        if(needForLoadFontsAndPalette) 
        {
            HAL_LTDC::LoadPalette();
            if(!noFonts)                // Если был запрос на загрузку шрифтов
            {
                Font::Load(TypeFont::_5);
                Font::Load(TypeFont::_8);
                Font::Load(TypeFont::UGO);
                Font::Load(TypeFont::UGO2);
            }
        }
    }

    FillRegion(0, 0, 319, 239, color);
}


void Painter::EndScene(bool endScene)
{
    if (Display::framesElapsed != 1)
    {
        Display::framesElapsed = 1;
        return;
    }

#ifdef DEBUG

    PText::Draw(262, 207, "Отладка", COLOR_FILL);

#endif

    HAL_LTDC::ToggleBuffers();

    if (endScene)
    {
        if (InterCom::TransmitGUIinProcess())
        {
            CommandBuffer command(4, END_SCENE);
            command.Transmit(1);
        }
    }

    if (stateTransmit == StateTransmit_InProcess)
    {
        stateTransmit = StateTransmit_Free;
    }
}


uint Painter::ReduceBrightness(uint colorValue, float newBrightness)
{
    int red = (int)(R_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(red, red, 0, 255);

    int green = (int)(G_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(green, green, 0, 255);

    int blue = (int)(B_FROM_COLOR(colorValue) * newBrightness);
    LIMITATION(blue, blue, 0, 255);

    return Color::Make((uint8)red, (uint8)green, (uint8)blue);
}


bool Painter::SaveScreenToFlashDrive() {

#pragma pack(1)
    struct BITMAPFILEHEADER
    {
        char    type0;      // 0
        char    type1;      // 1
        uint    size;       // 2
        uint16  res1;       // 6
        uint16  res2;       // 8
        uint    offBits;    // 10
    };
    // 14

    struct BITMAPINFOHEADER
    {
        uint    size;           // 14
        int     width;          // 18
        int     height;         // 22
        uint16  planes;         // 26
        uint16  bitCount;       // 28
        uint    compression;    // 30
        uint    sizeImage;      // 34
        int     xPelsPerMeter;  // 38
        int     yPelsPerMeter;  // 42
        uint    clrUsed;        // 46
        uint    clrImportant;   // 50
        //uint    notUsed[15];
    };
    // 54
#pragma pack(4)

    BITMAPFILEHEADER bmFH =
    {
        0x42,
        0x4d,
        14 + 40 + 1024 + 320 * 240 / 2,
        0,
        0,
        14 + 40 + 1024
    };

    StructForWrite structForWrite;
    char fileName[255];
    
    if(!FM::GetNameForNewFile(fileName))
    {
        return false;
    }
    
    FDrive::OpenNewFileForWrite(fileName, &structForWrite);

    FDrive::WriteToFile((uint8*)(&bmFH), 14, &structForWrite);

    BITMAPINFOHEADER bmIH =
    {
        40, // size;
        320,// width;
        240,// height;
        1,  // planes;
        4,  // bitCount;
        0,  // compression;
        0,  // sizeImage;
        0,  // xPelsPerMeter;
        0,  // yPelsPerMeter;
        0,  // clrUsed;
        0   // clrImportant;
    };  

    FDrive::WriteToFile((uint8*)(&bmIH), 40, &structForWrite);

    uint8 buffer[320 * 3] = {0};
    
    struct RGBQUAD
    {
        uint8    blue; 
        uint8    green; 
        uint8    red; 
        uint8    rgbReserved; 
    };
    
    RGBQUAD colorStruct;    

    for(int i = 0; i < 16; i++)
    {
        uint color = set.display.colors[i];
        colorStruct.blue = (uint8)((float)B_FROM_COLOR(color) / 31.0f * 255.0f);
        colorStruct.green = (uint8)((float)G_FROM_COLOR(color) / 63.0f * 255.0f);
        colorStruct.red = (uint8)((float)R_FROM_COLOR(color) / 31.0f * 255.0f);
        colorStruct.rgbReserved = 0;
        ((RGBQUAD*)(buffer))[i] = colorStruct;
    }
    

    for(int i = 0; i < 4; i++)
    {
        FDrive::WriteToFile(buffer, 256, &structForWrite);
    }

    for(int y = 239; y >= 0; y--)
    {
        for(int x = 1; x < 320; x += 2)
        {
            uint8 color = (uint8)GetColor(x, y);

            buffer[x / 2] = (uint8)(((color & 0x0f) << 4) + (color >> 4));
        }
        FDrive::WriteToFile(buffer, 160, &structForWrite);
    }
    
    FDrive::CloseFile(&structForWrite);
    
    return true;
}


void Painter::SetPalette(Color::E)
{

}
