// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Definition.h"
#include "FPGA/FPGA.h"
#include "Menu/Menu.h"
#include "Panel/Panel.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Utils/Text/Warnings.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Symbols.h"
#include "Display/Screen/Console.h"
#include <cstdio>


static void FuncDraw()
{
    Painter::BeginScene(COLOR_BACK);

    String<>("����������� ����� �������� �������� ������ ����/����.\n������� ����� ������ ������, ���� ����� �� �����.")
        .DrawInRectWithTransfers(30, 110, 300, 200, COLOR_FILL);

    Console::Draw();

    Painter::EndScene();
}


static void OnPress_ResetSettings()
{
    PageService::ResetSettings(true);
}


void PageService::ResetSettings(bool wait_key)
{
    if (wait_key)
    {
        Panel::Disable();
        Display::SetDrawMode(DrawMode::Timer, FuncDraw);

        if (Panel::WaitPressingButton() != Key::Start)
        {
            Display::SetDrawMode(DrawMode::Normal);
            Panel::Enable();

            return;
        }

        Display::SetDrawMode(DrawMode::Normal);
        Panel::Enable();
    }

    bool running = FPGA::IsRunning();
    FPGA::Stop();

    gset.Reset();
    FPGA::Init();

    if (running)
    {
        FPGA::Start();
    }
}


const Button bResetSettings
(
    PageService::self, nullptr,
    "����� ��������", "Reset settings",
    "����� �������� �� ��������� �� ���������",
    "Reset to default settings",
    OnPress_ResetSettings
);


void PageService::OnPress_AutoSearch()
{
    FPGA::AutoFinder::FindSignal();
};


static const Button bAutoSearch
(
    PageService::self, 0,
    "����� �������", "Find signal",
    "������������� ����������� ��������� ������������ ��� ������� � ������ 1",
    "Sets optimal settings for the oscilloscope signal on channel 1",
    PageService::OnPress_AutoSearch
);


static void OnChanged_Calibrator_Mode(bool)
{
    CalibratorMode::Set(CALIBRATOR);
}


static const Choice cCalibrator_Mode =
{
    TypeItem::Choice, PageService::Calibrator::self, 0,
    {
        "����������",  "Calibrator",
        "����� ������ �����������",
        "Mode of operation of the calibrator"
    },
    {
        {"�����",   "DC"},
        {"����",    "AC"},
        {"0�",      "OV"}
    },
    (int8 *)&CALIBRATOR, OnChanged_Calibrator_Mode
};


static void OnPress_Calibrator_Calibrate()
{
    FPGA::state.needCalibration = true;
}


static const Button cCalibrator_Calibrate
(
    PageService::Calibrator::self, 0,
    "�����������", "Calibrate",
    "������ ��������� ����������",
    "Running the calibration procedure",
    OnPress_Calibrator_Calibrate
);


static const arrayItems itemsCalibrator =
{
    (void *)&cCalibrator_Mode,
    (void *)&cCalibrator_Calibrate
};


static const Page ppCalibrator
(
    PageService::self, 0,
    "����������", "CALIBRATOR",
    "����������� ������������ � ���������� ������������",
    "Item of the calibrator and calibration of an oscillograph",
    NamePage::ServiceCalibrator, &itemsCalibrator
);


static const arrayItems itemsMath =
{
    (void *)PageService::Math::Function::self,
    (void *)PageService::Math::FFT::self
};


static const Page ppMath
(
    PageService::self, 0,
    "����������", "MATH",
    "�������������� ������� � ���",
    "Mathematical functions and FFT",
    NamePage::Math, &itemsMath
);


static const SmallButton sbMath_Function_Exit
(
    PageService::Math::Function::self,
    COMMON_BEGIN_SB_EXIT,
    EmptyFuncVV,
    DrawSB_Exit
);


static void Draw_Math_Function_ModeDraw_Disable(int x, int y)
{
    String<>(LANG_RU ? "���" : "Dis").Draw(x + 2 + (LANG_EN ? 2 : 0), y + 5);
}


static void Draw_Math_Function_ModeDraw_Separate(int x, int y)
{
    Rectangle(13, 9).Draw(x + 3, y + 5);
    Painter::DrawHLine(true, y + 9, x + 3, x + 16);
    Painter::DrawHLine(true, y + 10, x + 3, x + 16);
}


static void Draw_Math_Function_ModeDraw_Together(int x, int y)
{
    Rectangle(13, 9).Draw(x + 3, y + 5);
}


static const arrayHints hintsMath_Function_ModeDraw =
{
    {Draw_Math_Function_ModeDraw_Disable,  "����� �������������� ������� ��������",
                                            "The conclusion of mathematical function is disconnected"},
    {Draw_Math_Function_ModeDraw_Separate, "������� � �������������� ������� ��������� � ������ �����",
                                            "Signals and mathematical function are removed in different windows"},
    {Draw_Math_Function_ModeDraw_Together, "������� � �������������� ������� ��������� � ����� ����",
                                            "Signals and mathematical function are removed in one window"}
};


static void OnPress_Math_Function_ModeDraw()
{
    if (ENABLED_FFT)
    {
        Warning::ShowBad(Warning::ImpossibleEnableMathFunction);
    }
    else
    {
        Math::CircleIncrease<int8>((int8 *)&MODE_DRAW_MATH, 0, 2);
    }
}


static void Draw_Math_Function_ModeDraw(int x, int y)
{
    static const pFuncVII funcs[3] =
    {
        Draw_Math_Function_ModeDraw_Disable,
        Draw_Math_Function_ModeDraw_Separate,
        Draw_Math_Function_ModeDraw_Together
    };
    funcs[MODE_DRAW_MATH](x, y);
}


static const SmallButton sbMath_Function_ModeDraw
(
    PageService::Math::Function::self, 0,
    "�����", "Display",
    "�������� ����� ����������� ��������������� �������",
    "Chooses the mode of display of a mathematical signal",
    OnPress_Math_Function_ModeDraw,
    Draw_Math_Function_ModeDraw,
    &hintsMath_Function_ModeDraw
);


static void OnPress_Math_Function_Type()
{
    Math::CircleIncrease<int8>((int8 *)&MATH_FUNC, 0, 1);
}


static void Draw_Math_Function_Type_Sum(int x, int y)
{
    Painter::DrawHLine(true, y + 9, x + 4, x + 14);
    Painter::DrawVLine(true, x + 9, y + 4, y + 14);
}


static void Draw_Math_Function_Type_Mul(int x, int y)
{
    Font::Set(TypeFont::UGO2);
    Text::Draw4SymbolsInRect(x + 4, y + 3, SYMBOL_MATH_FUNC_MUL);
    Font::Set(TypeFont::_8);
}


static void Draw_Math_Function_Type(int x, int y)
{
    const pFuncVII funcs[2] = {Draw_Math_Function_Type_Sum, Draw_Math_Function_Type_Mul};
    funcs[MATH_FUNC](x, y);
}


static const arrayHints hintsMath_Function_Type =
{
    { Draw_Math_Function_Type_Sum,      "��������",     "Addition"       },
    { Draw_Math_Function_Type_Mul,      "���������",    "Multiplication" }
};


static const SmallButton sbMath_Function_Type
(
    PageService::Math::Function::self, 0,
    "���", "Type",
    "����� �������������� �������",
    "Choice of mathematical function",
    OnPress_Math_Function_Type,
    Draw_Math_Function_Type,
    &hintsMath_Function_Type
);


static void OnPress_Math_Function_ModeRegSet()
{
    Math::CircleIncrease<int8>((int8 *)&MATH_MODE_REG_SET, 0, 1);
}


static void Draw_Math_Function_ModeRegSet_Range(int x, int y)
{
    Char(LANG_RU ? 'M' : 'S').Draw(true, x + 7, y + 5);
}


static void Draw_Math_Function_ModeRegSet_RShift(int x, int y)
{
    String<>(LANG_RU ? "��" : "Shif").Draw(x + 5 - (LANG_EN ? 3 : 0), y + 5);
}


static void Draw_Math_Function_ModeRegSet(int x, int y)
{
    const pFuncVII funcs[2] = {Draw_Math_Function_ModeRegSet_Range, Draw_Math_Function_ModeRegSet_RShift};
    funcs[MATH_MODE_REG_SET](x, y);
}


static const arrayHints hintsMath_Function_ModeRegSet =
{
    {Draw_Math_Function_ModeRegSet_Range,  "���������� ���������", "Management of scale"},
    {Draw_Math_Function_ModeRegSet_RShift, "���������� ���������", "Management of shift"}
};


static const SmallButton sbMath_Function_ModeRegSet
(
    PageService::Math::Function::self, 0,
    "����� ����� ���������", "Mode regulator SET",
    "����� ������ ����� ��������� - ���������� ��������� ��� ���������",
    "Choice mode regulcator ��������� - management of scale or shift",
    OnPress_Math_Function_ModeRegSet,
    Draw_Math_Function_ModeRegSet,
    &hintsMath_Function_ModeRegSet
);


static void OnPress_Math_Function_RangeA()
{
    SET_RANGE_MATH = SET_RANGE_A;
    MATH_MULTIPLIER = SET_DIVIDER_A;
}


static void Draw_Math_Function_RangeA(int x, int y)
{
    Char('1').Draw(true, x + 8, y + 5);
}


static const SmallButton sbMath_Function_RangeA
(
    PageService::Math::Function::self, 0,
    "������� 1-�� ������", "Scale of the 1st channel",
    "���� ������� ��� ��������������� ������� �� ������� ������",
    "Takes scale for a mathematical signal from the first channel",
    OnPress_Math_Function_RangeA,
    Draw_Math_Function_RangeA
);


static void OnPress_Math_Function_RangeB()
{
    SET_RANGE_MATH = SET_RANGE_B;
    MATH_MULTIPLIER = SET_DIVIDER_B;
}


static void Draw_Math_Function_RangeB(int x, int y)
{
    Char('2').Draw(true, x + 8, y + 5);
}


static const SmallButton sbMath_Function_RangeB
(
    PageService::Math::Function::self, 0,
    "������� 2-�� ������", "Scale of the 2nd channel",
    "���� ������� ��� ��������������� ������� �� ������� ������",
    "Takes scale for a mathematical signal from the second channel",
    OnPress_Math_Function_RangeB,
    Draw_Math_Function_RangeB
);


static const arrayItems itemsMath_Function =
{
    (void*)&sbMath_Function_Exit,
    (void*)&sbMath_Function_ModeDraw,
    (void*)&sbMath_Function_Type,
    (void*)&sbMath_Function_ModeRegSet,
    (void*)&sbMath_Function_RangeA,
    (void*)&sbMath_Function_RangeB
};


static bool IsActive_Math_Function()
{
    return !ENABLED_FFT;
}


static void OnPress_Math_Function()
{
    if (ENABLED_FFT)
    {
        Warning::ShowBad(Warning::ImpossibleEnableMathFunction);
    }
}


static void OnRegSet_Math_Function(int delta)
{
    if (DISABLED_DRAW_MATH)
    {
        return;
    }

    if (MATH_MODE_REG_SET_IS_RSHIFT)
    {
        int16 prevRShift = SET_RSHIFT_MATH;
        int16 rshift = prevRShift;
        if (delta > 0)
        {
            if (rshift < RShift::MAX)
            {
                rshift += 4 * RShift::STEP;
                LIMIT_ABOVE(rshift, RShift::MAX);

                if (prevRShift < RShift::ZERO && rshift > RShift::ZERO)
                {
                    rshift = RShift::ZERO;
                }

                SET_RSHIFT_MATH = rshift;
            }
        }
        else if (delta < 0)
        {
            if (rshift > RShift::MIN)
            {
                rshift -= 4 * RShift::STEP;
                LIMIT_BELOW(rshift, RShift::MIN);

                if (prevRShift > RShift::ZERO && rshift < RShift::ZERO)
                {
                    rshift = RShift::ZERO;
                }

                SET_RSHIFT_MATH = rshift;
            }
        }
    }
    if (MATH_MODE_REG_SET_IS_RANGE)
    {
        static int sum = 0;
        sum -= delta;

        float rShiftAbs = SET_RSHIFT_MATH.ToAbs(SET_RANGE_MATH);

        if (sum > 2)
        {
            if (SET_RANGE_MATH < Range::Count - 1)
            {
                ++SET_RANGE_MATH;
                SET_RSHIFT_MATH = (int16)RShift::ToRel(rShiftAbs, SET_RANGE_MATH);
            }
            sum = 0;
        }
        else if (sum < -2)
        {
            if (SET_RANGE_MATH > 0)
            {
                --SET_RANGE_MATH;
                SET_RSHIFT_MATH = (int16)RShift::ToRel(rShiftAbs, SET_RANGE_MATH);
            }
            sum = 0;
        }
    }
}


static const Page pppMath_Function
(
    &ppMath, IsActive_Math_Function,
    "�������", "FUNCTION",
    "��������� � ����� �������������� ������� - �������� ��� ���������",
    "Installation and selection of mathematical functions - addition or multiplication",
    NamePage::SB_MathFunction, &itemsMath_Function, OnPress_Math_Function, EmptyFuncVV, OnRegSet_Math_Function
);


const Page *PageService::Math::Function::self = &pppMath_Function;


static const Choice cMath_FFT_Enable =
{
    TypeItem::Choice, PageService::Math::FFT::self, 0,
    {
        "�����������", "Display",
        "�������� � ��������� ����������� �������",
        "Enables or disables the display of the spectrum"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&ENABLED_FFT
};


static const Choice cMath_FFT_Scale =
{
    TypeItem::Choice, PageService::Math::FFT::self, 0,
    {
        "�����", "Scale",
        "����� ������� ������ ������� - �������� ��� ���������������",
        "Sets the scale of the output spectrum - linear or logarithmic"
    },
    {
        {"��������",    "Log"},
        {"��������",    "Linear"}
    },
    (int8 *)&SCALE_FFT
};


static const Choice cMath_FFT_Source =
{
    TypeItem::Choice, PageService::Math::FFT::self, 0,
    {
        "��������", "Source",
        "����� ��������� ��� ������� �������",
        "Selecting the source for the calculation of the spectrum"
    },
    {
        {"����� 1",     "Channel 1"},
        {"����� 2",     "Channel 2"},
        {"����� 1 + 2", "Channel 1 + 2"}
    },
    (int8 *)&SOURCE_FFT
};


static const Choice cMath_FFT_Window =
{
    TypeItem::Choice, PageService::Math::FFT::self, 0,
    {
        "����", "Window",
        "����� ���� ��� ������� �������",
        "Sets the window to calculate the spectrum"
    },
    {
        {"�����������", "Rectangle"},
        {"��������",    "Hamming"},
        {"��������",    "Blackman"},
        {"�����",       "Hann"}
    },
    (int8 *)&WINDOW_FFT
};


static bool IsActive_Math_FFT_Limit()
{
    return SCALE_FFT_IS_LOG;
}


static const Choice cMath_FFT_Limit =
{
    TypeItem::Choice, PageService::Math::FFT::self,  IsActive_Math_FFT_Limit,
    {
        "��������", "Range",
        "����� ����� ������ ������ ���������� �� ��������� �������",
        "Here you can set the limit of monitoring the power spectrum"
    },
    {
        {"-40��",   "-40dB"},
        {"-60��",   "-60dB"},
        {"-80��",   "-80dB"}
    },
    (int8 *)&FFT_MAX_DB
};


static const arrayItems itemsMath_FFT =
{
    (void*)&cMath_FFT_Enable,
    (void*)&cMath_FFT_Scale,
    (void*)&cMath_FFT_Source,
    (void*)&cMath_FFT_Window,
    (void*)PageService::Math::FFT::Cursors::self,
    (void*)&cMath_FFT_Limit
};


static bool IsActive_Math_FFT()
{
    return DISABLED_DRAW_MATH;
}


static void OnPress_Math_FFT()
{
    if (!IsActive_Math_FFT())
    {
        Warning::ShowBad(Warning::ImpossibleEnableFFT);
    }
}


static const Page pppMath_FFT
(
    &ppMath, IsActive_Math_FFT,
    "������", "SPECTRUM",
    "����������� ������� �������� �������",
    "Mapping the input signal spectrum",
    NamePage::MathFFT, &itemsMath_FFT, OnPress_Math_FFT
);


static const SmallButton cMath_FFT_Cursors_Exit
(
    PageService::Math::FFT::Cursors::self,
    COMMON_BEGIN_SB_EXIT,
    EmptyFuncVV,
    DrawSB_Exit
);


static void OnPress_Math_FFT_Cursors_Source()
{
    FFT_CUR_CURSOR = (uint8)((FFT_CUR_CURSOR + 1) % 2);
}


static void Draw_Math_FFT_Cursors_Source(int x, int y)
{
    String<>(FFT_CUR_CURSOR_IS_0 ? "1" : "2").Draw(x + 7, y + 5);
}


static const SmallButton cMath_FFT_Cursors_Source
(
    PageService::Math::FFT::Cursors::self, 0,
    "��������", "Source",
    "����� ��������� ��� ������� �������",
    "Source choice for calculation of a range",
    OnPress_Math_FFT_Cursors_Source,
    Draw_Math_FFT_Cursors_Source
);


static const arrayItems itemsMath_FFT_Cursors =
{
    (void*)&cMath_FFT_Cursors_Exit,
    (void*)&cMath_FFT_Cursors_Source,
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)0
};


static bool IsActive_Math_FFT_Cursors()
{
    return ENABLED_FFT;
}


static void OnRegSet_Math_FFT_Cursors(int angle)
{
    FFT_POS_CURSOR(FFT_CUR_CURSOR) += (uint8)angle;
}


static const Page ppppMath_FFT_Cursors
(
    &pppMath_FFT, IsActive_Math_FFT_Cursors,
    "�������", "CURSORS",
    "�������� ������� ��� ��������� ���������� �������",
    "Includes cursors to measure the parameters of the spectrum",
    NamePage::SB_MathCursorsFFT, &itemsMath_FFT_Cursors, EmptyFuncVV, EmptyFuncVV, OnRegSet_Math_FFT_Cursors
);


static void OnChanged_Ethernet_Enable(bool)
{
    Warning::ShowGood(Warning::NeedRebootDevice2);
    Warning::ShowGood(Warning::NeedRebootDevice1);
}


static const Choice cEthernet_Enable =
{
    TypeItem::Choice, PageService::LAN::self, 0,
    {
        "Ethernet", "Ethernet"
        ,
        "����� ������������� ethernet, �������� \"��������\" � ��������� ������.\n"
        "����� ��������� ethernet, �������� \"���������\" � ��������� ������."
        ,
        "To involve ethernet, choose \"Included\" and switch off the device.\n"
        "To disconnect ethernet, choose \"Disconnected\" and switch off the device."
    },
    {
        {"��������",    "Included"},
        {"���������",   "Disconnected"}
    },
    (int8 *)&ETH_ENABLE, OnChanged_Ethernet_Enable
};


static const IPaddressStruct structIP =
{
    { TypeItem::IP, PageService::LAN::self, 0,
    {   "IP �����", "IP-address",
        "��������� IP ������",
        "Set of IP-address",  } },
    &IP_ADDR0, &IP_ADDR1, &IP_ADDR2, &IP_ADDR3,
    OnChanged_Ethernet_Enable,
    &LAN_PORT
};


static const IPaddress ipEthernet_IP
(
    &structIP
);


static const IPaddressStruct structMask =
{
    { TypeItem::IP, PageService::LAN::self, 0,
    {   "����� �������", "Network mask",
        "��������� ����� �������",
        "Set of network mask",    } },
    &NETMASK_ADDR0, &NETMASK_ADDR1, &NETMASK_ADDR2, &NETMASK_ADDR3,
    OnChanged_Ethernet_Enable
};


static const IPaddress ipEthernet_Mask
(
    &structMask
);


static const IPaddressStruct structGateway =
{
    { TypeItem::IP, PageService::LAN::self, 0,
    {   "����", "Gateway",
        "��������� ������ ��������� �����",
        "Set of gateway address", } },
    &GW_ADDR0, &GW_ADDR1, &GW_ADDR2, &GW_ADDR3,
    OnChanged_Ethernet_Enable
};


static const IPaddress ipEthernet_Gateway
(
    &structGateway
);


static const MACaddress macEthernet_MAC =
{
    TypeItem::MAC, PageService::LAN::self, 0,
    {
        "��� �����", "MAC-address",
        "��������� ����������� ������",
        "Set of MAC-address"
    },
    &MAC_ADDR0, &MAC_ADDR1, &MAC_ADDR2, &MAC_ADDR3, &MAC_ADDR4, &MAC_ADDR5,
    OnChanged_Ethernet_Enable
};


static const arrayItems itemsEthernet =
{
    (void*)&cEthernet_Enable,
    (void*)&ipEthernet_IP,
    (void*)&ipEthernet_Mask,
    (void*)&ipEthernet_Gateway,
    (void*)&macEthernet_MAC
};


static const Page ppEthernet
(
    PageService::self, 0,
    "ETHERNET", "ETHERNET",
    "��������� ethernet",
    "Settings of ethernet",
    NamePage::ServiceEthernet, &itemsEthernet
);


static const Choice cSound =
{
    TypeItem::Choice, PageService::self, 0,
    {
        "����", "Sound",
        "���������/���������� �����",
        "Inclusion/switching off of a sound"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SOUND_ENABLED
};


static const Choice cLang =
{
    TypeItem::Choice, PageService::self, 0,
    {
        "����", "Language",
        "��������� ������� ���� ����",
        "Allows you to select the menu language"
    },
    {
        {"�������",     "Russian"},
        {"����������",  "English"}
    },
    (int8*)&LANG
};


static int8 dServicetime = 0;
static int8 hours = 0, minutes = 0, secondes = 0, year = 0, month = 0, day = 0;

static const TimeControl tTime =
{
    TypeItem::Time, PageService::self, 0,
    {
        "�����", "Time"
        ,
        "��������� �������� �������.\n������� ������:\n"
        "������ �� ������� ���� \"�����\". ��������� ���� ��������� �������� �������. ��������� ��������� ������ �� �������� ����������, �������������� "
        "�������� ���������� \"�����\", �������� ����, ������, �������, ���, �����, ��� �����. ���������� ������� ������������ �������� ��������. "
        "��������� ����� ��������� ���������� ����������� ��������. ����� �������� ����� \"���������\", ������ � ���������� ����� 0.5 ��� ������ �� ������ "
        "����������. ���� ��������� �������� ������� ��������� � ����������� ������ �������� �������. ������� ���������� ��������� ������ �� ����� ������ �������� "
        "������� � �������� ���� ��������� �������� ������� ��� ���������� ������ �������� �������"
        ,
        "Setting the current time. \nPoryadok work:\n"
        "Click on the menu item \"Time\".The menu set the current time.By briefly pressing the button on the numeric keypad of conformity "
        "Item \"Time\", highlight the hours, minutes, seconds, year, month, or a number.The selected item is indicated by a flashing area. "
        "Turn the setting knob to set the desired value. Then highlight \"Save\", press and udrezhivat more than 0.5 seconds, the button on the panel "
        "Item. Menu Setting the current time will be closed to the conservation of the new current time. Pressing a button on the prolonged retention of any other element "
        "will lead to the closure of the current time setting menu without saving the new current time"
    },
    &dServicetime, &hours, &minutes, &secondes, &month, &day, &year
};


static const Choice cModeLongPressButtonTrig =
{
    TypeItem::Choice, PageService::self, 0,
    {
        "��� ���� �����", "Mode long �����"
        ,
        "������������� �������� ��� ����������� ������� ������ �����:\n\"����� ������\" - ��������� ������ ������������� � ����,\n\"�����������\" "
        "- �������������� ����������� � ��������� ������ �������������"
        ,
        "Sets the action for long press CLOCK:\n\"Reset trig lvl\" - to set the trigger level to zero, \n\"Auto level\""
        "- Automatically detect and install the trigger level"
    },
    {
        {"����� ������",    "Reset trig level"},
        {"�����������",     "Autolevel"}
    },
    (int8*)&MODE_LONG_PRESS_TRIG
};


static void OnPress_Information_Exit()
{
    Display::SetDrawMode(DrawMode::Normal);
}


const SmallButton sbInformation_Exit
(
    PageService::Information::self,
    COMMON_BEGIN_SB_EXIT,
    OnPress_Information_Exit,
    DrawSB_Exit
);


static const arrayItems itemsInformation =
{
    (void *)&sbInformation_Exit,
    (void *)0,
    (void *)0,
    (void *)0,
    (void *)0,
    (void *)0
};


static void Information_Draw()
{
    Painter::BeginScene(Color::BLACK);
    int x = 100;
    int dY = 20;
    int y = 20;
    Rectangle(319, 239).Draw(0, 0, COLOR_FILL);
    y += dY;
    String<>(LANG_RU ? "����������" : "INFORMATION").Draw(x, y);
    y += dY;
    String<>(LANG_RU ? "������ : �8-53/1" : "Model : S8-53/1").Draw(x, y);
    y += dY;

    String<>(LANG_RU ? "����������� �����������:" : "Software:").Draw(x, y);
    y += dY;
    String<>((pchar)(LANG_RU ? "������ %s" : "version %s"), NUMBER_VER).Draw(x, y);
    y += dY;

    String<>("CRC32 : %X", HAL::CalculateCRC32()).Draw(x, y, COLOR_FILL);

    dY = -10;
    String<>(LANG_RU ? "��� ��������� ������ ������� ������ ������" : "For help, click the HELP button").DrawInCenterRect(0, 190 + dY, 320, 20);

    Menu::Draw();
    Painter::EndScene();
}


static void OnPress_Information()
{
    PageService::Information::self->OpenAndSetCurrent();
    Display::SetDrawMode(DrawMode::Timer, Information_Draw);
}


static const Page ppInformation
(
    PageService::self, 0,
    "����������", "INFORMATION",
    "������� �� ����� ����������������� ������ ������������",
    "Displays identification data of the oscilloscope",
    NamePage::SB_Information, &itemsInformation, OnPress_Information, EmptyFuncVV, EmptyFuncVI
);


static const arrayItems itemsService =
{
    (void *)&bResetSettings,
    (void *)&bAutoSearch,
    (void *)&ppCalibrator,
    (void *)&ppMath,
    (void *)&ppEthernet,
    (void *)&cSound,
    (void *)&cLang,
    (void *)&tTime,
    (void *)&cModeLongPressButtonTrig,
    (void *)&ppInformation
};


static const Page pService
(
    PageMain::self, 0,
    "������", "SERVICE",
    "�������������� ���������, ����������, ����� �������, �������������� �������",
    "Additional settings, calibration, signal search, mathematical functions",
    NamePage::Service, &itemsService
);


const Page *PageService::self = &pService;
const Page *PageService::Calibrator::self = &ppCalibrator;
const Page *PageService::Math::FFT::self = &pppMath_FFT;
const Page *PageService::Math::FFT::Cursors::self = &ppppMath_FFT_Cursors;
const Page *PageService::LAN::self = &ppEthernet;
const Page *PageService::Information::self = &ppInformation;
