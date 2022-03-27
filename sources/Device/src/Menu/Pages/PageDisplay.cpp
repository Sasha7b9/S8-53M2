// 2022/2/11 19:48:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/MenuItems.h"
#include "Menu/Menu.h"
#include "Display/Tables.h"
#include "Display/Painter.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Pages/Definition.h"
#include "Data/DataExtensions.h"


extern const Page pDisplay;
extern const Page mspAccumulation;
extern const Page mspAveraging;
extern const Page mspGrid;
extern const Page mspSettings;
extern const Page mspSettings_Colors;


static const Choice mcMapping =
{
    TypeItem::Choice, &pDisplay, 0,
    {
        "�����������", "View",
        "����� ����� ����������� �������.",
        "Sets the display mode signal."
    },
    {
        {"������",  "Vector"},
        {"�����",   "Points"}
    },
    (int8 *)&MODE_DRAW_SIGNAL
};


static bool IsActive_MinMax() //-V524
{
    return !TBase::InModeRandomizer();
}


static void OnChanged_MinMax(bool)
{
    /*
    int maxMeasures = DS_NumberAvailableEntries();
    int numMinMax = sDisplay_NumberMinMax();

    if (maxMeasures < numMinMax)
    {
        Display::ShowWarningWithNumber(ExcessValues, maxMeasures);
    }
    */
}


static const Choice mcMinMax =
{
    TypeItem::ChoiceReg, &pDisplay, IsActive_MinMax,
    {
        "��� ����", "Min Max"
        ,
        "����� ���������� ��������� ���������, �� ������� �������� ��������������� �����, ��������� �������� � ��������� ���������."
        ,
        "Sets number of the last measurements on which the limiting lines which are bending around minima and maxima of measurements are under "
        "construction."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"2",           "2"},
        {"4",           "4"},
        {"8",           "8"},
        {"16",          "16"},
        {"32",          "32"},
        {"64",          "64"},
        {"128",         "128"}
    },
    (int8 *)&ENUM_MIN_MAX, OnChanged_MinMax
};


static const Choice mcSmoothing =
{
    TypeItem::ChoiceReg, &pDisplay, 0,
    {
        "�����������", "Smoothing",
        "������������� ���������� ����� ��� ������� ����������� �� �������� ������ �������.",
        "Establishes quantity of points for calculation of the signal smoothed on the next points."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"2 �����",     "2 points"},
        {"3 �����",     "3 points"},
        {"4 �����",     "4 points"},
        {"5 �����",     "5 points"},
        {"6 �����",     "6 points"},
        {"7 �����",     "7 points"},
        {"8 �����",     "8 points"},
        {"9 �����",     "9 points"},
        {"10 �����",    "10 points"}
    },
    (int8 *)&SMOOTHING
};


void OnChanged_RefreshFPS(bool)
{
    FPGA::SetNumSignalsInSec(ENumSignalsInSec::ToNum(ENUM_SIGNALS_IN_SEC));
}


static const Choice mcRefreshFPS =
{
    TypeItem::Choice, &pDisplay, 0,
    {
        "������� ������", "Refresh rate",
        "����� ������������ ����� ��������� � ������� ������.",
        "Sets the maximum number of the shots removed in a second."
    },
    {
        {"25",  "25"},
        {"10",  "10"},
        {"5",   "5"},
        {"2",   "2"},
        {"1",   "1"}
    },
    (int8 *)&ENUM_SIGNALS_IN_SEC, OnChanged_RefreshFPS
};


static const Choice mcTypeShift =
{
    TypeItem::Choice, &pDisplay, 0,
    {
        "��������", "Offset"
        ,
        "����� ����� ��������� �������� �� ���������\n1. \"����������\" - ����������� ���������� ��������.\n2. \"�������\" - ����������� ��������� "
        "�������� �� ������."
        ,
        "Sets the mode of retaining the vertical displacement\n1. \"Voltage\" - saved dressing bias.\n2. \"Divisions\" - retained the position of "
        "the offset on the screen."
    },
    {
        {"����������", "Voltage"},
        {"�������", "Divisions"}
    },
    (int8 *)&LINKING_RSHIFT
};


static const arrayItems itemsDisplay =
{
    (void *)&mcMapping,
    (void *)&mspAccumulation,
    (void *)&mspAveraging,
    (void *)&mcMinMax,
    (void *)&mcSmoothing,
    (void *)&mcRefreshFPS,
    (void *)&mspGrid,
    (void *)&mcTypeShift,
    (void *)&mspSettings
};

static const Page pDisplay                 // �������
(
    PageMain::self, 0,
    "�������", "DISPLAY",
    "�������� ��������� ����������� �������.",
    "Contains settings of display of the Display::",
    NamePage::Display, &itemsDisplay
);


const Page *PageDisplay::self = &pDisplay;


static const Choice mcAccumulation_Number =
{
    TypeItem::ChoiceReg, &mspAccumulation, 0,
    {
        "����������", "Number"
        ,
        "����� ������������ ���������� ��������� �������� �� ������. ���� � ��������� \"�����\" ������� \"�������������\", ����� ��������� ������ "
        "�������� ������ \"��������\"."
        "\"�������������\" - ������ ��������� ������� �� ������� �� ��� ���, ���� �� ����� ������ ������ \"��������\"."
        ,
        "Sets the maximum quantity of the last signals on the screen. If in control \"Mode\" it is chosen \"Infinity\", the screen is cleared only "
        "by pressing of the button \"Clear\"."
        "\"Infinity\" - each measurement remains on the display until the button \"Clear\" is pressed."
    },
    {
        {DISABLE_RU,        DISABLE_EN},
        {"2",               "2"},
        {"4",               "4"},
        {"8",               "8"},
        {"16",              "16"},
        {"32",              "32"},
        {"64",              "64"},
        {"128",             "128"},
        {"�������������",   "Infinity"}
    },
    (int8 *)&ENUM_ACCUM
};


static const Choice mcAccumulation_Mode =
{
    TypeItem::Choice, &mspAccumulation, 0,
    {
        "�����", "Mode"
        ,
        "1. \"����������\" - ����� ���������� ��������� ���������� ��������� ���������� ������� �������. ���� ����� ������, ����� ������ �� ������� "
        "��� ���������� ������� ���������� ���������.\n"
        "2. \"�� ����������\" - �� ������� ������ ��������� �������� ��� ������� (� ������ �������� ������) ���������� ���������. ����������� �������� "
        "������� �������������� � ������������� ���������� ��������� ���������� ��������� ��� ���������� ������."
        ,
        "1. \"Dump\" - after accumulation of the set number of measurement there is a cleaning of the Display:: This mode is convenient when memory "
        "isn't enough for preservation of the necessary number of measurements.\n"
        "2. \"Not to dump\" - the number of measurements is always output to the display set or smaller (in case of shortage of memory). Shortcoming "
        "is smaller speed and impossibility of accumulation of the set number of measurements at a lack of memory."
    },
    {
        {"�� ����������",   "Not to dump"},
        {"����������",      "Dump"}
    },
    (int8 *)&MODE_ACCUM
};


static bool IsActive_Accumulation_Clear()
{
    return ENUM_ACCUM_IS_INFINITY;
}


void OnPress_Accumulation_Clear()
{
    Flags::needFinishDraw = true;
}


static const Button mcAccumulation_Clear
(
    &mspAccumulation, IsActive_Accumulation_Clear,
    "��������", "Clear",
    "������� ����� �� ����������� ��������.",
    "Clears the screen of the saved-up signals.",
    OnPress_Accumulation_Clear
);


static const arrayItems itemsAccumulation =
{
    (void*)&mcAccumulation_Number,
    (void*)&mcAccumulation_Mode,
    (void*)&mcAccumulation_Clear
};


static bool IsActive_Accumulation() //-V524
{
    return !TBase::InModeRandomizer();
}


static const Page mspAccumulation
(
    &pDisplay, IsActive_Accumulation,
    "����������", "ACCUMULATION",
    "��������� ������ ����������� ��������� �������� �� ������.",
    "Mode setting signals to display the last screen.",
    NamePage::DisplayAccumulation, &itemsAccumulation
);


static const Choice mcAveraging_Number =
{
    TypeItem::ChoiceReg, &mspAveraging, 0,
    {
        "����������", "Number",
        "����� ���������� ��������� ���������, �� ������� ������������ ����������.",
        "Sets number of the last measurements on which averaging is made."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"2",           "2"},
        {"4",           "4"},
        {"8",           "8"},
        {"16",          "16"},
        {"32",          "32"},
        {"64",          "64"},
        {"128",         "128"},
        {"256",         "256"},
        {"512",         "512"}
    },
    (int8 *)&ENUM_AVE, nullptr, nullptr
};


static const Choice mcAveraging_Mode =
{
    TypeItem::Choice, &mspAveraging, 0,
    {
        "�����", "Mode"
        ,
        "1. \"�����\" - ������ ����� ����������, ����� � ������� ��������� ������ ��������� �������.\n"
        "2. \"��������������\" - ��������������� ����� ����������. ����� ����� ������������, ����� ������ ���������� ��������� �������, ��� ����� "
        "����������� � ������."
        ,
        "1. \"Accurately\" - the exact mode of averaging when only the last signals participate in calculation.\n"
        "2. \"Around\" - approximate mode of averaging. It makes sense to use when the number of measurements bigger is set, than can be located in "
        "memory."
    },
    {
        {"�����",           "Accurately"},
        {"��������������",  "Around"}
    },
    (int8 *)&MODE_AVE, nullptr, nullptr
};


static const arrayItems itemsAveraging =
{
    (void*)&mcAveraging_Number,
    (void*)&mcAveraging_Mode
};


static bool IsActive_Averaging()
{
    return true;
}


static const Page mspAveraging
(
    &pDisplay, IsActive_Averaging,
    "����������", "AVERAGE",
    "��������� ������ ���������� �� ��������� ����������.",
    "Settings of the mode of averaging on the last measurements.",
    NamePage::DisplayAverage, &itemsAveraging
);


static const Choice mcGrid_Type =
{
    TypeItem::Choice, &mspGrid, 0,
    {
        "���", "Type",
        "����� ���� �����.",
        "Choice like Grid::"
    },
    {
        {"��� 1", "Type 1"},
        {"��� 2", "Type 2"},
        {"��� 3", "Type 3"},
        {"��� 4", "Type 4"}
    },
    (int8 *)&TYPE_GRID
};


ColorType colorTypeGrid = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::GRID};

void OnChanged_Grid_Brightness()
{
    colorTypeGrid.SetBrightness(BRIGHTNESS_GRID / 1e2f);
    HAL_LTDC::LoadPalette();
}


static void BeforeDraw_Grid_Brightness()
{
    colorTypeGrid.Init();
    BRIGHTNESS_GRID = (int16)(colorTypeGrid.brightness * 100.0f);
}


static const Governor mgGrid_Brightness
(
    &mspGrid, 0,
    "�������", "Brightness",
    "������������� ������� �����.",
    "Adjust the brightness of the Grid::",
    &BRIGHTNESS_GRID, 0, 100, OnChanged_Grid_Brightness, BeforeDraw_Grid_Brightness
);


static const arrayItems itemsGrid =
{
    (void*)&mcGrid_Type,
    (void*)&mgGrid_Brightness
};


static const Page mspGrid
(
    &pDisplay, 0,
    "�����", "GRID",
    "�������� ��������� ����������� ������������ �����.",
    "Contains settings of display of a coordinate Grid::",
    NamePage::DisplayGrid, &itemsGrid
);


static void OnChanged_Settings_Brightness()
{
    HAL_DAC1::SetBrightness(BRIGHTNESS);
}


static const Governor mgSettings_Brightness
(
    &mspSettings, 0,
    "�������", "Brightness",
    "��������� ������� �������� �������",
    "Setting the brightness of the display",
    &BRIGHTNESS, 0, 100, OnChanged_Settings_Brightness
);


static const Governor mgSettings_Levels
(
    &mspSettings, 0,
    "������", "Levels",
    "����� �����, � ������� �������� ����� �������� ����� ������� ���������� �� ������ ������� ��������������� ����� ������ ��������",
    "Defines the time during which, after turning the handle visits to the voltage on the screen remains auxiliary label offset level",
    &TIME_SHOW_LEVELS, 0, 125
);


static const Governor mgSettings_TimeMessages
(
    &mspSettings, 0,
    "�����", "Time",
    "��������� �������, � ������� �������� ��������� ����� ���������� �� ������",
    "Set the time during which the message will be on the screen",
    &TIME_MESSAGES, 1, 99
);


static const Choice mcSettings_ShowStringNavigation =
{
    TypeItem::Choice, &mspSettings, 0,
    {
        "������ ����", "Path menu",
        "��� ������ \n����������\n ����� ������ ������ ��������� ������ ���� �� ������� �������� ����",
        "When choosing \nDisplay\n at the top left of the screen displays the full path to the current page menu"
    },
    {
        {"��������",    "Hide"},
        {"����������",  "Show"}
    },
    (int8 *)&SHOW_STRING_NAVIGATION
};


static void OnChanged_Settings_ShowAltMarkers(bool)
{
    RShift::ChangedMarkers();
}


static const Choice mcSettings_ShowAltMarkers =
{
    TypeItem::Choice, &mspSettings, 0,
    {
        "���. �������", "Alt. markers"
        ,
        "������������� ����� ����������� �������������� �������� ������� �������� � �������������:\n"
        "\"��������\" - �������������� ������� �� ������������,\n"
        "\"����������\" - �������������� ������� ������������ ������,\n"
        "\"����\" - �������������� ������� ������������ � ������� 5 ��� ����� �������� ����� �������� ������ �� ���������� ��� ������ �������������"
        ,
        "Sets the display mode of additional markers levels of displacement and synchronization:\n"
        "\"Hide\" - additional markers are not shown,\n"
        "\"Show\" - additional markers are shown always,\n"
        "\"Auto\" - additional markers are displayed for 5 seconds after turning the handle channel offset voltage or trigger level"
    },
    {
        {"��������",    "Hide"},
        {"����������",  "Show"},
        {"����",        "Auto"}
    },
    (int8 *)&ALT_MARKERS, OnChanged_Settings_ShowAltMarkers
};


static void OnChanged_Settings_AutoHide(bool autoHide)
{
    Menu::SetAutoHide(autoHide);
}


static const Choice mcSettings_AutoHide =
{
    TypeItem::Choice, &mspSettings, 0,
    {
        "��������", "Hide",
        "��������� ����� ���������� ������� ������ ��� �������� �����, �� ��������� �������� ���� ������������� ��������� � ������",
        "Installation after the last keystroke or turning the handle, after which the menu will automatically disappear"
    },
    {
        {"�������",         "Never"},
        {"����� 5 ���",     "Through 5 s"},
        {"����� 10 ���",    "Through 10 s"},
        {"����� 15 ���",    "Through 15 s"},
        {"����� 30 ���",    "Through 30 s"},
        {"����� 60 ���",    "Through 60 s"}
    },
    (int8 *)&MENU_AUTO_HIDE, OnChanged_Settings_AutoHide
};


static const arrayItems itemsSettings =
{
    (void*)&mspSettings_Colors,
    (void*)&mgSettings_Brightness,
    (void*)&mgSettings_Levels,
    (void*)&mgSettings_TimeMessages,
    (void*)&mcSettings_ShowStringNavigation,
    (void*)&mcSettings_ShowAltMarkers,
    (void*)&mcSettings_AutoHide
};

static const Page mspSettings
(
    &pDisplay, 0,
    "���������", "SETTINGS",
    "�������������� ��������� �������",
    "Additional display settings",
    NamePage::ServiceDisplay, &itemsSettings
);


static const Choice mcSettings_Colors_Scheme =
{
    TypeItem::Choice, &mspSettings_Colors, 0,
    {
        "�������� �����", "Color::E scheme",
        "����� ������ �����������",
        "Mode of operation of the calibrator"
    },
    {
        {"����� 1", "Scheme 1"},
        {"����� 2", "Scheme 2"}
    },
    (int8 *)&COLOR_SCHEME
};


static ColorType colorT1 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::DATA_A};

static const GovernorColor mgcSettings_Colors_ChannelA =
{
    TypeItem::GovernorColor, &mspSettings_Colors, 0,
    {
        "����� 1", "Channel 1",
        "",
        ""
    },
    &colorT1
};


static ColorType colorT2 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::DATA_B};

static const GovernorColor mgcSettings_Colors_ChannelB =
{
    TypeItem::GovernorColor, &mspSettings_Colors, 0,
    {
        "����� 2", "Channel 2",
        "",
        ""
    },
    &colorT2
};


static const GovernorColor mgcSettings_Colors_Grid =
{
    TypeItem::GovernorColor, &mspSettings_Colors, 0,
    {
        "�����", "Grid",
        "������������� ���� �����",
        "Sets the grid color"
    },
    &colorTypeGrid
};


static const arrayItems itemsSettings_Colors =
{
    (void*)&mcSettings_Colors_Scheme,
    (void*)&mgcSettings_Colors_ChannelA,
    (void*)&mgcSettings_Colors_ChannelB,
    (void*)&mgcSettings_Colors_Grid
};

static const Page mspSettings_Colors
(
    &mspSettings, 0,
    "�����", "COLORS",
    "����� ������ �������",
    "The choice of colors display",
    NamePage::ServiceDisplayColors, &itemsSettings_Colors
);
