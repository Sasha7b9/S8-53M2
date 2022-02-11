#include "defines.h"
#include "Colors.h"
#include "Painter.h"
#include "font/Font.h"
#include "Log.h"
#include "Ethernet/LAN.h"
#include "Ethernet/TcpSocket.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "VCP/VCP.h"
#include "Hardware/FSMC.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "FlashDrive/FlashDrive.h"
#include "Menu/FileManager.h"



static bool inverseColors = false;
static Color currentColor = NUM_COLORS;

static enum StateTransmit
{
    StateTransmit_Free,
    StateTransmit_NeedForTransmitFirst,  // ��� ����� ����� �������� ������ ���� - ���������� ������
    StateTransmit_NeedForTransmitSecond, // ��� ����� ����� �������� ������ � ����������� ����� - ������ �� ����������
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
    if (currentColor == COLOR_FLASH_10)
    {
        SetColor(inverseColors ? COLOR_BACK : COLOR_FILL);
    }
    else if (currentColor == COLOR_FLASH_01)
    {
        SetColor(inverseColors ? COLOR_FILL : COLOR_BACK);
    }
}


void CalculateColor(uint8 *color)
{
    currentColor = (Color)*color;
    if (*color == COLOR_FLASH_10)
    {
        *color = inverseColors ? COLOR_BACK : COLOR_FILL;
    }
    else if (*color == COLOR_FLASH_01)
    {
        *color = inverseColors ? COLOR_FILL : COLOR_BACK;
    }
}


void InverseColor(Color *color)
{
    *color = (*color == COLOR_BLACK) ? COLOR_WHITE : COLOR_BLACK;
}


static void OnTimerFlashDisplay()
{
    inverseColors = !inverseColors;
}


void Painter::ResetFlash()
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


void Painter::DrawRectangleC(int x, int y, int width, int height, Color color)
{
    SetColor(color);
    DrawRectangle(x, y, width, height);
}


void Painter::DrawDashedVLine(int x, int y0, int y1, int deltaFill, int deltaEmtpy, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmtpy))
    {
        LOG_ERROR("������������ �������� deltaStart = %d", deltaStart);
        return;
    }
    int y = y0;
    if (deltaStart != 0)                 // ���� ����� ����� �������� �� � ������ ������
    {
        y += (deltaFill + deltaEmtpy - deltaStart);
        if (deltaStart < deltaFill)     // ���� ������ ����� ���������� �� �����
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
        LOG_ERROR("������������ �������� deltaStart = %d", deltaStart);
        return;
    }
    int x = x0;
    if (deltaStart != 0)                // ���� ����� ����� �������� �� � ������ ������
    {
        x += (deltaFill + deltaEmpty - deltaStart);
        if (deltaStart < deltaFill)     // ���� ������ ����� ���������� �� �����
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



void Painter::SendToVCP(uint8 *pointer, int size)
{
    if(stateTransmit == StateTransmit_InProcess)
    {
        VCP::SendDataSynch(pointer, size);
        TCPSocket_Send((const char *)pointer, (uint)size);
    }
}


static void Get4Bytes(uint8 bytes[4])
{
    while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET) {};
    bytes[0] = *ADDR_CDISPLAY;
    bytes[1] = *ADDR_CDISPLAY;
    bytes[2] = *ADDR_CDISPLAY;
    bytes[3] = *ADDR_CDISPLAY;
}


void Painter::SetColor(Color color)
{
    if (color != currentColor)
    {
        currentColor = color;
        if (currentColor > NUM_COLORS)
        {
            CalculateColor((uint8 *)&color);
        }

        CommandBuffer command(4, SET_COLOR);
        command.PushByte(color);
        SendToVCP(command.Data(), 2);

        SendToDisplay(command, 4);
    }
}


Color Painter::CurrentColor(void)
{
    return currentColor;
}


void Painter::DrawHLine(int y, int x0, int x1)
{
    CalculateCurrentColor();

    SendToDisplay(command, 8);

    CommandBuffer command(8, DRAW_HLINE);
    command.PushByte(y);
    command.PushHalfWord(x0);
    command.PushHalfWord(x1);
    SendToVCP(command.Data(), 6);
}


void Painter::DrawHLineC(int y, int x0, int x1, Color color)
{
    SetColor(color);
    DrawHLine(y, x0, x1);
}


void Painter::DrawVLine(int x, int y0, int y1)
{
    CalculateCurrentColor();

    SendToDisplay(command, 8);

    CommandBuffer command(8, DRAW_VLINE);
    command.PushHalfWord(x);
    command.PushByte(y0);
    command.PushByte(y1);
    SendToVCP(command.Data(), 5);
}


void Painter::DrawVLineC(int x, int y0, int y1, Color color)
{
    SetColor(color);
    DrawVLine(x, y0, y1);
}


void Painter::DrawLineC(int x0, int y0, int x1, int y1, Color color)
{
    SetColor(color);
    DrawLine(x0, y0, x1, y1);
}


void Painter::DrawVPointLine(int x, int y0, int y1, float delta, Color color)
{
    SetColor(color);

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
    SendToDisplay(command, 4);

    CommandBuffer command(4, SET_POINT);
    command.PushHalfWord(x);
    command.PushByte(y);
    SendToVCP(command.Data(), 4);
}


void Painter::DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color color) 
{
    if(numLines > 20) 
    {
        LOG_ERROR("����� ����� ������� ������� %d", numLines);
        return;
    }

    SetColor(color);

    SendToDisplay(command, numBytes);

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
    while(numBytes % 4) 
    {
        numBytes++;
    }

    SendToVCP(command.Data(), 1 + 1 + 1 + 1 + 1 + 1 + numLines * 2);
}


void Painter::DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color color)
{
    if (numLines > 20)
    {
        LOG_ERROR("����� ����� ������� ������� %d", numLines);
        return;
    }
    SetColor(color);

    SendToDisplay(command, numBytes);

    CommandBuffer command(30, DRAW_MULTI_HPOINT_LINES_2);
    command.PushByte(numLines);
    command.PushHalfWord(x);
    command.PushByte(count);
    command.PushByte(delta);
    for (int i = 0; i < numLines; i++)
    {
        command.PushByte(y[i]);
    }
    int numBytes = 1 +     // command
        1 +     // numLines
        2 +     // x
        numLines +    // numLines
        1 +
        1;
    while (numBytes % 4)
    {
        numBytes++;
    }

    SendToVCP(command.Data(), 1 + 1 + 2 + 1 + 1 + numLines);
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

    CommandBuffer command(8, FILL_REGION);
    command.PushHalfWord(x);
    command.PushByte(y);
    command.PushHalfWord(width);
    command.PushByte(height);
    SendToVCP(command.Data(), 7);
}


void Painter::FillRegionC(int x, int y, int width, int height, Color color)
{
    SetColor(color);
    FillRegion(x, y, width, height);
}


void Painter::DrawVolumeButton(int x, int y, int width, int height, int thickness, Color normal, Color bright, Color dark, bool isPressed, bool inShade)
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


void Painter::SetBrightnessDisplay(int16 brightness)
{
    float recValue = 1601.0f;
    if (brightness < 100)
    {
        recValue = 64.0f + (600.0f - 63.0f) / 100.0f / 100.0f * brightness * brightness;
    }

    SendToDisplay(command, 4);
}


int NumberColorsInSceneCol(void)
{
    return numberColorsUsed;
}


void Painter::DrawVLineArray(int x, int numLines, uint8 *y0y1, Color color)
{
    SetColor(color);

    for (; numLines > 0; numLines--)
    {
        int y0 = *y0y1++;
        int y1 = *y0y1++;

        DrawVLine(x++, y0, y1);
    }

    CommandBuffer command(255 * 2 + 4 + 4, DRAW_VLINES_ARRAY);
    command.PushHalfWord(x);
    if (numLines > 255)
    {
        numLines = 255;
    }
    command.PushHalfWord(numLines);
    for (int i = 0; i < numLines; i++)
    {
        command.PushByte(*(y0y1 + i * 2));
        command.PushByte(*(y0y1 + i * 2 + 1));
    }
    int numBytes = numLines * 2 + 4;
    while (numBytes % 4)
    {
        numBytes++;
    }

    SendToVCP(command.Data(), 1 + 2 + 1 + 2 * numLines);
}


void Painter::DrawSignal(int x, uint8 data[281], bool modeLines)
{
    SendToDisplay(command, 284);

    CommandBuffer command(284, modeLines ? DRAW_SIGNAL_LINES : DRAW_SIGNAL_POINTS);
    command.PushHalfWord(x);
    for (int i = 0; i < 281; i++)
    {
        command.PushByte(data[i]);
    }
    SendToVCP(command.Data(), 284);
}


void Painter::LoadPalette(void)
{
    for (int i = 0; i < NUM_COLORS; i++)
    {
        SetPalette((Color)i);
    }
}


void Painter::BeginScene(Color color)
{
    if (stateTransmit == StateTransmit_NeedForTransmitFirst || stateTransmit == StateTransmit_NeedForTransmitSecond)
    {
        bool needForLoadFontsAndPalette = stateTransmit == StateTransmit_NeedForTransmitFirst;
        stateTransmit = StateTransmit_InProcess;
        if(needForLoadFontsAndPalette) 
        {
            LoadPalette();
            if(!noFonts)                // ���� ��� ������ �� �������� �������
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


void Painter::RunDisplay()
{
    SendToDisplay(command, 4);
}


void Painter::EndScene(bool endScene)
{
    if (FRAMES_ELAPSED != 1)
    {
        FRAMES_ELAPSED = 1;
        return;
    }

    SendToDisplay(command, 4);

    CommandBuffer command(4, END_SCENE);

    if (endScene)
    {
        SendToVCP(command.Data(), 1);
    }
    if (stateTransmit == StateTransmit_InProcess)
    {
        VCP::Flush();
        stateTransmit = StateTransmit_Free;
    }

    RunDisplay();
}


Color Painter::GetColor(int x, int y)
{
    SendToDisplay(command, 4);

    CommandBuffer command(4, GET_POINT);
    command.PushHalfWord(x);
    command.PushByte(y);

    Get4Bytes(command.Data());

    return (Color)(command[0] & 0x0f);
}


uint8 Get2Points(int x, int y)
{
    while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET) {};
    *ADDR_CDISPLAY = GET_POINT;
    *ADDR_CDISPLAY = (uint8)x;
    *ADDR_CDISPLAY = (uint8)(x >> 8);
    *ADDR_CDISPLAY = (uint8)y;
    while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET) {};
    return *ADDR_CDISPLAY;
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


static uint8 Read2points(int x, int y)
{
    while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET) {};
    Timer::PauseOnTicks(50);
    *ADDR_CDISPLAY = GET_POINT;
    *ADDR_CDISPLAY = (uint8)x;
    *ADDR_CDISPLAY = (uint8)(x >> 8);
    *ADDR_CDISPLAY = (uint8)y;
    int counter = 0;
    while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET) { counter++; };
    Timer::PauseOnTicks(50);
    uint8 retValue = *ADDR_CDISPLAY;
    return retValue;
}


bool Painter::SaveScreenToFlashDrive(void) {

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

    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);     // ��������� ����� ���������� FPGA, ����� ����� ������������ ������ �������
    for(int y = 239; y >= 0; y--)
    {
        for(int x = 1; x < 320; x += 2)
        {
            uint8 color = Read2points(x, y);

            while (color != Read2points(x, y))
            {
                color = Read2points(x, y);
            }
            
            if(color != 0)
            {
                color = color;
            }

            buffer[x / 2] = (uint8)(((color & 0x0f) << 4) + (color >> 4));
        }
        FlashDrive::WriteToFile(buffer, 160, &structForWrite);
    }
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);   // ���������� ����� ���������� FPGA
    
    FlashDrive::CloseFile(&structForWrite);
    
    return true;
}
