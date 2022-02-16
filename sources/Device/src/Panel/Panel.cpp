// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Panel.h"
#include "Controls.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Containers/Queue.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#include <stdio.h>
#include <string.h>
#include "Panel/PanelFunctions.cpp"


#define MAX_DATA            20

#define LED_CHAN0_ENABLE    129U
#define LED_CHAN0_DISABLE   1U
#define LED_CHAN1_ENABLE    130U
#define LED_CHAN1_DISABLE   2U
#define LED_TRIG_ENABLE     131
#define LED_TRIG_DISABLE    3
#define POWER_OFF           4

static PanelButton pressedKey = B_Empty;
static volatile PanelButton pressedButton = B_Empty;         // Ёто используетс€ дл€ отслеживани€ нажатой кнопки при отключенной панели
static uint16 dataTransmitted[MAX_DATA] = {0x00};
static uint16 numDataForTransmitted = 0;
static uint timePrevPressButton = 0;
static uint timePrevReleaseButton = 0;


static void(*funcOnKeyDown[B_NumButtons])()    =
{    
    0,
    EmptyFuncVV,    // B_ChannelA
    EmptyFuncVV,    // B_Service
    EmptyFuncVV,    // B_ChannelB
    EmptyFuncVV,    // B_Display
    EmptyFuncVV,    // B_Time
    EmptyFuncVV,    // B_Memory                                         
    EmptyFuncVV,    // B_Sinchro
    StartDown,      // B_Start
    EmptyFuncVV,    // B_Cursors
    EmptyFuncVV,    // B_Measures
    PowerDown,      // B_Power
    EmptyFuncVV,    // B_Help
    EmptyFuncVV,    // B_Menu
    EmptyFuncVV,    // B_F1
    EmptyFuncVV,    // B_F2
    EmptyFuncVV,    // B_F3
    EmptyFuncVV,    // B_F4
    EmptyFuncVV     // B_F5
};

static void (*funcOnKeyUp[B_NumButtons])()    =
{
    0,
    EmptyFuncVV,    // B_ChannelA
    EmptyFuncVV,    // B_Service
    EmptyFuncVV,    // B_ChannelB
    EmptyFuncVV,    // B_Display
    EmptyFuncVV,    // B_Time
    EmptyFuncVV,    // B_Memory
    EmptyFuncVV,    // B_Sinchro
    EmptyFuncVV,    // B_Start
    EmptyFuncVV,    // B_Cursors
    EmptyFuncVV,    // B_Measures
    EmptyFuncVV,    // B_Power
    EmptyFuncVV,    // B_Help
    EmptyFuncVV,    // B_Menu
    EmptyFuncVV,    // B_F1
    EmptyFuncVV,    // B_F2
    EmptyFuncVV,    // B_F3
    EmptyFuncVV,    // B_F4
    EmptyFuncVV     // B_F5
};

static void (*funcOnLongPressure[B_NumButtons])()    =
{
    0,
    Channel0Long,   // B_ChannelA
    EmptyFuncVV,    // B_Service
    Channel1Long,   // B_ChannelB
    EmptyFuncVV,    // B_Display
    TimeLong,       // B_Time
    EmptyFuncVV,    // B_Memory
    TrigLong,       // B_Sinchro
    EmptyFuncVV,    // B_Start
    EmptyFuncVV,    // B_Cursors
    EmptyFuncVV,    // B_Measures
    EmptyFuncVV,    // B_Power
    HelpLong,       // B_Help
    MenuLong,       // B_Menu
    F1Long,         // B_F1
    F2Long,         // B_F2
    F3Long,         // B_F3
    F4Long,         // B_F4
    F5Long          // B_F5
};

static void (*funculatorLeft[R_Set + 1])()    =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    Range0Left,     // R_RangeA
    RShift0Left,    // R_RShiftA
    Range1Left,     // R_RangeB
    RShift1Left,    // R_RShiftB
    TBaseLeft,      // R_TBase
    TShiftLeft,     // R_TShift
    TrigLevLeft,    // R_TrigLev
    SetLeft         // R_Set
};
static void (*funculatorRight[R_Set + 1])() =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    Range0Right,    // R_RangeA
    RShift0Right,   // R_RShiftA
    Range1Right,    // R_RangeB
    RShift1Right,   // R_RShiftB
    TBaseRight,     // R_TBase
    TShiftRight,    // R_TShift
    TrigLevRight,   // R_TrigLev
    SetRight        // R_Set
};


namespace Panel
{
    Queue<uint16> input_buffer;

    // ѕреобразует данные из новой панели в данные, опознаваемые старой прошивкой
    uint16 TranslateCommand(const uint8 *data, uint size);
}



PanelButton ButtonIsRelease(uint16 command)
{
    PanelButton button = B_Empty;

    if(command < B_NumButtons && command > B_Empty)
    {
        if(gTimerMS - timePrevReleaseButton > 100)
        {
            button = (PanelButton)command;
            timePrevReleaseButton = gTimerMS;
        }
    }

    return button;
}

PanelButton ButtonIsPress(uint16 command)
{
    PanelButton button = B_Empty;

    if (((command & 0x7f) < B_NumButtons) && ((command & 0x7f) > B_Empty))
    {
        if(gTimerMS - timePrevPressButton > 100)
        {
            button = (PanelButton)(command & 0x7f);
            timePrevPressButton = gTimerMS;
        }
    }

    return button;
}

Regulator RegulatorLeft(uint16 command)
{
    if(command >= 20 && command <= 27)
    {
        return (Regulator)command;
    }

    return R_Empty;
}


Regulator RegulatorRight(uint16 command)
{
    if(((command & 0x7f) >= 20) && ((command & 0x7f) <= 27))
    {
        return (Regulator)(command & 0x7f);
    }
    return R_Empty;
}


void OnTimerPressedKey()
{
    if(pressedKey != B_Empty)
    {
        void (*func)() = funcOnLongPressure[pressedKey];
        Menu::Handlers::ReleaseButton(pressedKey);
        if(func != 0)
        {
            func();
        }
        pressedKey = B_Empty;
    }
    Timer::Disable(TypeTimer::PressKey);
}


void Panel::ProcessingCommandFromPIC(uint16 command)
{
    static int allRecData = 0;

    PanelButton releaseButton = ButtonIsRelease(command);
    PanelButton pressButton = ButtonIsPress(command);
    Regulator regLeft = RegulatorLeft(command);
    Regulator regRight = RegulatorRight(command);

    if (pressButton != B_Empty)
    {
        pressedButton = pressButton;
    }

    if(PANEL_IS_RUNNING == 0)
    {
        return;
    }

    if(command != 0)
    {
        allRecData++;
    }
    else
    {
        allRecData++;
        allRecData--;
    }

    if(releaseButton != B_Empty)
    {
        Menu::Handlers::ReleaseButton(releaseButton);

        funcOnKeyUp[releaseButton]();

        if(pressedKey != B_Empty)
        {
            Menu::Handlers::ShortPressureButton(releaseButton);
            pressedKey = B_Empty;
        }
    }
    else if(pressButton != B_Empty)
    {
        funcOnKeyDown[pressButton]();
        Menu::Handlers::PressButton(pressButton);
        pressedKey = pressButton;
        Timer::Enable(TypeTimer::PressKey, 500, OnTimerPressedKey);
    }
    else if(regLeft != R_Empty)
    {
         funculatorLeft[regLeft]();
    }
    else if(regRight != R_Empty)
    {
        funculatorRight[regRight]();
    }
}


void Panel::EnableLEDChannel0(bool enable)
{
    Panel::TransmitData(enable ? LED_CHAN0_ENABLE : LED_CHAN0_DISABLE);
}


void Panel::EnableLEDChannel1(bool enable)
{
    Panel::TransmitData(enable ? LED_CHAN1_ENABLE : LED_CHAN1_DISABLE);
}


void Panel::EnableLEDTrig(bool enable)
{
    static uint timeEnable = 0;
    static bool first = true;
    static bool fired = false;
    if(first)
    {
        Panel::TransmitData(LED_TRIG_DISABLE);
        Display::EnableTrigLabel(false);
        timeEnable = gTimerMS;
        first = false;
    }

    if(enable)
    {
        timeEnable = gTimerMS;
    }

    if(enable != fired)
    {
        if(enable)
        {
            Panel::TransmitData(LED_TRIG_ENABLE);
            Display::EnableTrigLabel(true);
            fired = true;
        }
        else if(gTimerMS - timeEnable > 100)
        {
            Panel::TransmitData(LED_TRIG_DISABLE);
            Display::EnableTrigLabel(false);
            fired = false;
        }
    }
}


void Panel::TransmitData(uint16 data)
{
    if(numDataForTransmitted >= MAX_DATA)
    {
        LOG_WRITE("Ќе могу послать в панель - мало места");
    }
    else
    {
        dataTransmitted[numDataForTransmitted] = data;
        numDataForTransmitted++;
    }
}


uint16 Panel::NextData()
{
    if (numDataForTransmitted > 0)
    {
        numDataForTransmitted--;
        return dataTransmitted[numDataForTransmitted];
    }
    return 0;
}


void Panel::Disable()
{
    PANEL_IS_RUNNING = 0;
}


void Panel::Enable()
{
    PANEL_IS_RUNNING = 1;
}


void Panel::Init()
{
    Panel::EnableLEDRegSet(false);
    Panel::EnableLEDTrig(false);
}


void Panel::EnableLEDRegSet(bool enable)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, enable ? GPIO_PIN_RESET : GPIO_PIN_SET);
}


PanelButton Panel::WaitPressingButton()
{
    pressedButton = B_Empty;
    while (pressedButton == B_Empty) {};
    return pressedButton;
}


uint16 Panel::TranslateCommand(const uint8 *data, uint size)
{
    static const int NUM_BUTTONS = 27;

    static const uint16 commands[NUM_BUTTONS] =
    {
        B_Empty,
        B_F1,
        B_F2,
        B_F3,
        B_F4,
        B_F5,
        B_ChannelA,
        B_ChannelB,
        B_Time,
        B_Trig,
        B_Cursors,
        B_Measures,
        B_Display,
        B_Help,
        B_Start,
        B_Memory,
        B_Service,
        B_Menu,
        B_Power,
        R_RangeA,
        R_RangeB,
        R_RShiftA,
        R_RShiftB,
        R_TBase,
        R_TShift,
        R_TrigLev,
        R_Set
    };

    uint16 command = 0;

    uint8 key = data[1];

    if (key > 0 && key < NUM_BUTTONS)
    {
        command = commands[key];

        if (command >= 20)
        {
            if (data[2] == 3)       { }
            else if (data[2] == 4)  { command |= 0x80; }
            else                    { command = 0; }
        }
        else
        {
            if (data[2] == 0)       { command |= 0x80; }
        }
    }

    return command;
}


void Panel::CallbackOnReceiveSPI5(const uint8 *data, uint size)
{
    uint16 command = TranslateCommand(data, size);

    if (command != 0)
    {
        input_buffer.Push(command);
    }
}


void Panel::Update()
{
    if (!input_buffer.IsEmpty())
    {
        uint16 command = input_buffer.Front();
        ProcessingCommandFromPIC(command);
    }
}
