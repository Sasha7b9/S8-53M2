// 2022/02/11 17:44:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Display/font/Font.h"
#include "Log.h"
#include "Ethernet/LAN.h"
#include "Ethernet/TcpSocket.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "VCP/VCP.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "FlashDrive/FlashDrive.h"
#include "Menu/FileManager.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/InterCom.h"
#include <cstring>


inline void BoundingX(int &x) { if (x < 0) x = 0; if (x >= Display::WIDTH) x = Display::WIDTH - 1; }
inline void BoundingY(int &y) { if (y < 0) y = 0; if (y >= Display::HEIGHT) y = Display::HEIGHT - 1; }


namespace Display
{
    uint8 back[240][320];

    static const int SIZE_BUFFER = WIDTH * HEIGHT;
    uint8* display_back_buffer = &back[0][0];
    uint8* display_back_buffer_end = display_back_buffer + SIZE_BUFFER;
}


static bool inverseColors = false;
static Color::E currentColor = Color::COUNT;

static enum StateTransmit
{
    StateTransmit_Free,
    StateTransmit_NeedForTransmitFirst,  // Это когда нужно передать первый кадр - передаются шрифты
    StateTransmit_NeedForTransmitSecond, // Это когда нужно передать второй и последующий кадры - шрифты не передаются
    StateTransmit_InProcess
} stateTransmit = StateTransmit_Free;

static bool noFonts = false;



void Painter::SendFrame(bool first, bool noFonts_)
{
    noFonts = noFonts_;

    if (stateTransmit == StateTransmit_Free)
    {
        stateTransmit = (first ? StateTransmit_NeedForTransmitFirst : StateTransmit_NeedForTransmitSecond);
    }
}


void Painter::CalculateCurrentColor()
{
    if (currentColor == Color::FLASH_10)
    {
        Color::SetCurrent(inverseColors ? COLOR_BACK : COLOR_FILL);
    }
    else if (currentColor == Color::FLASH_01)
    {
        Color::SetCurrent(inverseColors ? COLOR_FILL : COLOR_BACK);
    }
}


void CalculateColor(uint8 *color)
{
    currentColor = (Color::E)*color;
    if (*color == Color::FLASH_10)
    {
        *color = inverseColors ? COLOR_BACK : COLOR_FILL;
    }
    else if (*color == Color::FLASH_01)
    {
        *color = inverseColors ? COLOR_FILL : COLOR_BACK;
    }
}


void InverseColor(Color::E *color)
{
    *color = (*color == Color::BLACK) ? Color::WHITE : Color::BLACK;
}


static void OnTimerFlashDisplay()
{
    inverseColors = !inverseColors;
}


void Color::ResetFlash()
{
    Timer::Enable(kFlashDisplay, 400, OnTimerFlashDisplay);
    inverseColors = false;
}


void Painter::DrawRectangle(int x, int y, int width, int height)
{
    DrawHLine(y, x, x + width);
    DrawVLine(x, y, y + height);
    DrawHLine(y + height, x, x + width);
    if (x + width < SCREEN_WIDTH)
    {
        DrawVLine(x + width, y, y + height);
    }
}


void Painter::DrawRectangleC(int x, int y, int width, int height, Color::E color)
{
    Color::SetCurrent(color);
    DrawRectangle(x, y, width, height);
}


void Painter::DrawDashedVLine(int x, int y0, int y1, int deltaFill, int deltaEmtpy, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmtpy))
    {
        LOG_ERROR("Неправильный аргумент deltaStart = %d", deltaStart);
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
        LOG_ERROR("Неправильный аргумент deltaStart = %d", deltaStart);
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


static int numberColorsUsed = 0;



void InterCom::Send(uint8 *pointer, int size)
{
    VCP::SendDataSynch(pointer, size);
    TCPSocket_Send((const char *)pointer, (uint)size);
}


bool InterCom::TransmitGUIinProcess()
{
    return (stateTransmit == StateTransmit_InProcess);
}


void Color::SetCurrent(Color::E color)
{
    if (color != currentColor)
    {
        currentColor = color;

        if (currentColor > Color::COUNT)
        {
            CalculateColor((uint8 *)&color);
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
    return currentColor;
}


void Painter::DrawHLine(int y, int x0, int x1)
{
    CalculateCurrentColor();

    if (y < 0 || y >= Display::HEIGHT)
    {
        return;
    }

    BoundingX(x0);
    BoundingX(x1);

    if (x0 > x1)
    {
        Math_Swap(&x0, &x1);
    }

    uint8 *start = &Display::back[y][x0];

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


void Painter::DrawHLineC(int y, int x0, int x1, Color::E color)
{
    Color::SetCurrent(color);
    DrawHLine(y, x0, x1);
}


void Painter::DrawVLine(int x, int y0, int y1)
{
    CalculateCurrentColor();

    if (x < 0 || x >= Display::WIDTH)
    {
        return;
    }

    BoundingY(y0);
    BoundingY(y1);

    Math_Sort(&y0, &y1);

    uint8 *address = Display::display_back_buffer + Display::WIDTH * y0 + x;

    int counter = y1 - y0 + 1;

    uint8 value = Color::GetCurrent();

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


void Painter::DrawVLineC(int x, int y0, int y1, Color::E color)
{
    Color::SetCurrent(color);
    DrawVLine(x, y0, y1);
}


void Painter::DrawLineC(int x0, int y0, int x1, int y1, Color::E color)
{
    Color::SetCurrent(color);
    DrawLine(x0, y0, x1, y1);
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
    for (int x = x0; x <= x1; x += delta)
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

    uint8 *address = Display::display_back_buffer + Display::WIDTH * y + x;

    if (address < Display::display_back_buffer_end)
    {
        *address = Color::GetCurrent();
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

    uint8 *address = Display::display_back_buffer + Display::WIDTH * y + x;

    if (address < Display::display_back_buffer_end)
    {
        return (Color::E)*address;
    }

    return COLOR_BACK;
}


void Painter::DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color::E color) 
{
    if(numLines > 20) 
    {
        LOG_ERROR("Число линий слишком большое %d", numLines);
        return;
    }

    Color::SetCurrent(color);

    for (int i = 0; i < numLines; i++)
    {
        for (int j = 0; j < count; j++)
        {
            SetPoint(x[j], y);
            y += delta;
        }
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


void Painter::DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color::E color)
{
    if (numLines > 20)
    {
        LOG_ERROR("Число линий слишком большое %d", numLines);
        return;
    }
    Color::SetCurrent(color);

    for (int i = 0; i < numLines; i++)
    {
        for (int j = 0; j < count; j++)
        {
            SetPoint(x, y[j]);
            x += delta;
        }
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


void Painter::DrawLine(int x0, int y0, int x1, int y1)
{
    if (x0 == x1)
    {
        DrawVLine(x0, y0, y1);
    }
    else if (y0 == y1)
    {
        DrawHLine(y0, x0, x1);
    }
}


void Painter::FillRegion(int x, int y, int width, int height)
{
    CalculateCurrentColor();

    if (width == 0 || height == 0)
    {
        return;
    }

    for (int i = y; i <= y + height - 1; i++)
    {
        DrawHLine(i, x, x + width - 1);
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


void Painter::FillRegionC(int x, int y, int width, int height, Color::E color)
{
    Color::SetCurrent(color);
    FillRegion(x, y, width, height);
}


void Painter::DrawVolumeButton(int x, int y, int width, int height, int thickness, Color::E normal, Color::E bright, Color::E dark, bool isPressed, bool inShade)
{
    if (inShade)
    {
        thickness = 1;
    }
    FillRegionC(x + thickness, y + thickness, width - thickness * 2, height - thickness * 2, normal);
    if (isPressed || inShade)
    {
        for (int i = 0; i < thickness; i++)
        {
            DrawHLineC(y + i, x + i, x + width - i, dark);
            DrawVLine(x + i, y + 1 + i, y + height - i);
            DrawVLineC(x + width - i, y + 1 + i, y + height - i, bright);
            DrawHLine(y + height - i, x + 1 + i, x + width - i);
        }
    }
    else
    {
        for (int i = 0; i < thickness; i++)
        {
            DrawHLineC(y + i, x + i, x + width - i, bright);
            DrawVLine(x + i, y + 1 + i, y + height - i);
            DrawVLineC(x + width - i, y + 1 + i, y + height - i, dark);
            DrawHLine(y + height - i, x + 1 + i, x + width - i);
        }
    }
}


int NumberColorsInSceneCol()
{
    return numberColorsUsed;
}


void Painter::DrawVLineArray(int x, int num_lines, uint8 *y0y1, Color::E color)
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


void Painter::DrawSignal(int x, uint8 data[281], bool modeLines)
{
//    SendToDisplay(command, 284);

    if (InterCom::TransmitGUIinProcess())
    {
        CommandBuffer command(284, modeLines ? DRAW_SIGNAL_LINES : DRAW_SIGNAL_POINTS);
        command.PushHalfWord(x);
        for (int i = 0; i < 281; i++)
        {
            command.PushByte(data[i]);
        }
        command.Transmit(284);
    }
}


void Painter::LoadPalette()
{
    for (int i = 0; i < Color::COUNT; i++)
    {
        SetPalette((Color::E)i);
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
            LoadPalette();
            if(!noFonts)                // Если был запрос на загрузку шрифтов
            {
                LoadFont(TypeFont_5);
                LoadFont(TypeFont_8);
                LoadFont(TypeFont_UGO);
                LoadFont(TypeFont_UGO2);
            }
        }
    }

    FillRegionC(0, 0, 319, 239, color);
}


void Painter::EndScene(bool endScene)
{
    if (FRAMES_ELAPSED != 1)
    {
        FRAMES_ELAPSED = 1;
        return;
    }

#ifdef DEBUG

    DrawTextC(260, 21, LANG_RU ? "Отладка" : "Debug", COLOR_FILL);

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
        VCP::Flush();
        stateTransmit = StateTransmit_Free;
    }
}


uint16 Painter::ReduceBrightness(uint16 colorValue, float newBrightness)
{
    int red = R_FROM_COLOR(colorValue) * newBrightness;
    LIMITATION(red, red, 0, 31);
    int green = G_FROM_COLOR(colorValue) * newBrightness;
    LIMITATION(green, green, 0, 63);
    int blue = B_FROM_COLOR(colorValue) * newBrightness;
    LIMITATION(blue, blue, 0, 31);
    return MAKE_COLOR(red, green, blue);
}


bool Painter::SaveScreenToFlashDrive() {

#pragma pack(1)
    typedef struct
    {
        char    type0;      // 0
        char    type1;      // 1
        uint    size;       // 2
        uint16  res1;       // 6
        uint16  res2;       // 8
        uint    offBits;    // 10
    } BITMAPFILEHEADER;
    // 14

    typedef struct
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
    } BITMAPINFOHEADER;
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
    
    FlashDrive::OpenNewFileForWrite(fileName, &structForWrite);

    FlashDrive::WriteToFile((uint8*)(&bmFH), 14, &structForWrite);

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

    FlashDrive::WriteToFile((uint8*)(&bmIH), 40, &structForWrite);

    uint8 buffer[320 * 3] = {0};
    
    typedef struct tagRGBQUAD 
    {
        uint8    blue; 
        uint8    green; 
        uint8    red; 
        uint8    rgbReserved; 
    } RGBQUAD;
    
    RGBQUAD colorStruct;    

    for(int i = 0; i < 16; i++)
    {
        uint16 color = set.display.colors[i];
        colorStruct.blue = (float)B_FROM_COLOR(color) / 31.0f * 255.0f;
        colorStruct.green = (float)G_FROM_COLOR(color) / 63.0f * 255.0f;
        colorStruct.red = (float)R_FROM_COLOR(color) / 31.0f * 255.0f;
        colorStruct.rgbReserved = 0;
        ((RGBQUAD*)(buffer))[i] = colorStruct;
    }
    

    for(int i = 0; i < 4; i++)
    {
        FlashDrive::WriteToFile(buffer, 256, &structForWrite);
    }

    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);     // Отключаем буфер управления FPGA, чтобы снять шунтирование чтения дисплея
    for(int y = 239; y >= 0; y--)
    {
        for(int x = 1; x < 320; x += 2)
        {
            uint8 color = GetColor(x, y);

            buffer[x / 2] = (uint8)(((color & 0x0f) << 4) + (color >> 4));
        }
        FlashDrive::WriteToFile(buffer, 160, &structForWrite);
    }
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);   // Подключаем буфер управления FPGA
    
    FlashDrive::CloseFile(&structForWrite);
    
    return true;
}


void Painter::SetPalette(Color::E)
{

}
