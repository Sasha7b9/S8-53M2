// 2022/2/11 19:48:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/MenuItems.h"
#include "Menu/Menu.h"
#include "Tables.h"
#include "Display/Painter.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Pages/Definition.h"


extern const Page pDisplay;
extern const Page mspAccumulation;
extern const Page mspAveraging;
extern const Page mspGrid;


static bool IsActive_Averaging();                           // Активна ли страница ДИСПЛЕЙ-УСРЕДНЕНИЕ
extern const Choice mcAveraging_Number;                     // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Количество
extern const Choice mcAveraging_Mode;                       // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Режим

extern const Choice mcMinMax;                               //     ДИСПЛЕЙ - Мин Макс
static bool IsActive_MinMax();                              // Активна ли настройка ДИСПЛЕЙ-Мин Макс
static void OnChanged_MinMax(bool active);                  // Реакция на изменение ДИСПЛЕЙ-Мим Макс

extern const Choice mcSmoothing;                            //     ДИСПЛЕЙ - Сглаживание

extern const Choice mcRefreshFPS;                           //     ДИСПЛЕЙ - Частота обновл
void OnChanged_RefreshFPS(bool active);                     // Реакция на изменение ДИСПЛЕЙ-Частота обновл

extern const Choice mcGrid_Type;                            //     ДИСПЛЕЙ - СЕТКА - Тип
extern const Governor mgGrid_Brightness;                    // ДИСПЛЕЙ - СЕТКА - Яркость
void OnChanged_Grid_Brightness();                           // Реакция на изменение ДИСПЛЕЙ-СЕТКА-Яркость
static void BeforeDraw_Grid_Brightness();                   // Вызывается перед изменением ДИСПЛЕЙ-СЕТКА-Яркость

extern const Choice mcTypeShift;                            //     ДИСПЛЕЙ - Смещение

extern const Page mspSettings;                              //     ДИСПЛЕЙ - НАСТРОЙКИ
extern const Page mspSettings_Colors;                       // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА
extern const Choice mcSettings_Colors_Scheme;               // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Цветовая схема
extern const GovernorColor mgcSettings_Colors_ChannelA;     // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 1
extern const GovernorColor mgcSettings_Colors_ChannelB;     // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 2
extern const GovernorColor mgcSettings_Colors_Grid;         // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Сетка
extern const Governor mgSettings_Brightness;                // ДИСПЛЕЙ - НАСТРОЙКИ - Яркость
static void OnChanged_Settings_Brightness();                // Вызыватеся при изменении ДИСПЛЕЙ-НАСТРОЙКИ-Яркость
extern const Governor mgSettings_Levels;                    // ДИСПЛЕЙ - НАСТРОЙКИ - Уровни
extern const Governor mgSettings_TimeMessages;              // ДИСПЛЕЙ - НАСТРОЙКИ - Время
extern const Choice mcSettings_ShowStringNavigation;        // ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню
extern const Choice mcSettings_ShowAltMarkers;              // ДИСПЛЕЙ - НАСТРОЙКИ - Доп. маркеры
static void OnChanged_Settings_ShowAltMarkers(bool);
extern const Choice mcSettings_AutoHide;                    // ДИСПЛЕЙ - НАСТРОЙКИ - Скрывать
static void OnChanged_Settings_AutoHide(bool autoHide);     // Вызывается при изменении ДИСПЛЕЙ-НАСТРОЙКИ-Скрывать



static const Choice mcMapping =
{
    TypeItem::Choice, &pDisplay, 0,
    {
        "Отображение", "View",
        "Задаёт режим отображения сигнала.",
        "Sets the display mode signal."
    },
    {
        {"Вектор",  "Vector"},
        {"Точки",   "Points"}
    },
    (int8 *)&MODE_DRAW_SIGNAL
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

static const Page pDisplay                 // ДИСПЛЕЙ
(
    PageMain::self, 0,
    "ДИСПЛЕЙ", "DISPLAY",
    "Содержит настройки отображения дисплея.",
    "Contains settings of display of the Display::",
    NamePage::Display, &itemsDisplay
);


const Page *PageDisplay::self = &pDisplay;


static const Choice mcAccumulation_Number =
{
    TypeItem::ChoiceReg, &mspAccumulation, 0,
    {
        "Количество", "Number"
        ,
        "Задаёт максимальное количество последних сигналов на экране. Если в настройке \"Режим\" выбрано \"Бесконечность\", экран очищается только "
        "нажатием кнопки \"Очистить\"."
        "\"Бесконечность\" - каждое измерение остаётся на дисплее до тех пор, пока не будет нажата кнопка \"Очистить\"."
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
        {"Бесконечность",   "Infinity"}
    },
    (int8 *)&ENUM_ACCUM
};


static const Choice mcAccumulation_Mode =
{
    TypeItem::Choice, &mspAccumulation, 0,
    {
        "Режим", "Mode"
        ,
        "1. \"Сбрасывать\" - после накопления заданного количества измерения происходит очистка дисплея. Этот режим удобен, когда памяти не хватает "
        "для сохранения нужного количества измерений.\n"
        "2. \"Не сбрасывать\" - на дисплей всегда выводится заданное или меньшее (в случае нехватки памяти) количество измерений. Недостатком является "
        "меньшее быстродействие и невозможность накопления заданного количества измерений при недостатке памяти."
        ,
        "1. \"Dump\" - after accumulation of the set number of measurement there is a cleaning of the Display:: This mode is convenient when memory "
        "isn't enough for preservation of the necessary number of measurements.\n"
        "2. \"Not to dump\" - the number of measurements is always output to the display set or smaller (in case of shortage of memory). Shortcoming "
        "is smaller speed and impossibility of accumulation of the set number of measurements at a lack of memory."
    },
    {
        {"Не сбрасывать",   "Not to dump"},
        {"Сбрасывать",      "Dump"}
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
    "Очистить", "Clear",
    "Очищает экран от накопленных сигналов.",
    "Clears the screen of the saved-up signals.",
    OnPress_Accumulation_Clear
);


static const arrayItems itemsAccumulation =
{
    (void*)&mcAccumulation_Number,
    (void*)&mcAccumulation_Mode,
    (void*)&mcAccumulation_Clear
};


static bool IsActive_Accumulation()
{
    return !TBase::InModeRandomizer();
}


static const Page mspAccumulation
(
    &pDisplay, IsActive_Accumulation,
    "НАКОПЛЕНИЕ", "ACCUMULATION",
    "Настройки режима отображения последних сигналов на экране.",
    "Mode setting signals to display the last screen.",
    NamePage::DisplayAccumulation, &itemsAccumulation
);


static const arrayItems itemsAveraging =
{
    (void*)&mcAveraging_Number, // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Количество
    (void*)&mcAveraging_Mode    // ДИСПЛЕЙ - УСРЕДНЕНИЕ - Режим    
};


static const Page mspAveraging
(
    &pDisplay, IsActive_Averaging,
    "УСРЕДНЕНИЕ", "AVERAGE",
    "Настройки режима усреднения по последним измерениям.",
    "Settings of the mode of averaging on the last measurements.",
    NamePage::DisplayAverage, &itemsAveraging
);


static bool IsActive_Averaging()
{
    return true;
}


static const Choice mcAveraging_Number =
{
    TypeItem::ChoiceReg, &mspAveraging, 0,
    {
        "Количество", "Number",
        "Задаёт количество последних измерений, по которым производится усреднение.",
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
    (int8*)&ENUM_AVE
};


static const Choice mcAveraging_Mode =
{
    TypeItem::Choice, &mspAveraging, 0,
    {
        "Режим", "Mode"
        ,
        "1. \"Точно\" - точный режим усреднения, когда в расчёте участвуют только последние сигналы.\n"
        "2. \"Приблизительно\" - приблизительный режим усреднения. Имеет смысл использовать, когда задано количество измерений большее, чем может "
        "поместиться в памяти."
        ,
        "1. \"Accurately\" - the exact mode of averaging when only the last signals participate in calculation.\n"
        "2. \"Around\" - approximate mode of averaging. It makes sense to use when the number of measurements bigger is set, than can be located in "
        "memory."
    },
    {
        {"Точно",           "Accurately"},
        {"Приблизительно",  "Around"}
    },
    (int8*)&MODE_AVE
};


static const Choice mcMinMax =
{
    TypeItem::ChoiceReg, &pDisplay, IsActive_MinMax,
    {
        "Мин Макс", "Min Max"
        ,
        "Задаёт количество последних измерений, по которым строятся ограничительные линии, огибающие минимумы и максимумы измерений."
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
    (int8*)&ENUM_MIN_MAX, OnChanged_MinMax
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



// ДИСПЛЕЙ - Сглаживание ----
static const Choice mcSmoothing =
{
    TypeItem::ChoiceReg, &pDisplay, 0,
    {
        "Сглаживание", "Smoothing",
        "Устанавливает количество точек для расчёта сглаженного по соседним точкам сигнала.",
        "Establishes quantity of points for calculation of the signal smoothed on the next points."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"2 точки",     "2 points"},
        {"3 точки",     "3 points"},
        {"4 точки",     "4 points"},
        {"5 точек",     "5 points"},
        {"6 точек",     "6 points"},
        {"7 точек",     "7 points"},
        {"8 точек",     "8 points"},
        {"9 точек",     "9 points"},
        {"10 точек",    "10 points"}
    },
    (int8*)&SMOOTHING
};


// ДИСПЛЕЙ - Частота обновл -
static const Choice mcRefreshFPS =
{
    TypeItem::Choice, &pDisplay, 0,
    {
        "Частота обновл", "Refresh rate",
        "Задаёт максимальное число выводимых в секунду кадров.",
        "Sets the maximum number of the shots removed in a second."
    },
    {
        {"25",  "25"},
        {"10",  "10"},
        {"5",   "5"},
        {"2",   "2"},
        {"1",   "1"}
    },
    (int8*)&ENUM_SIGNALS_IN_SEC, OnChanged_RefreshFPS
};

void OnChanged_RefreshFPS(bool)
{
    FPGA::SetNumSignalsInSec(ENumSignalsInSec::ToNum(ENUM_SIGNALS_IN_SEC));
}


// ДИСПЛЕЙ - СЕТКА /////////////////
static const arrayItems itemsGrid =
{
    (void*)&mcGrid_Type,        // ДИСПЛЕЙ - СЕТКА - Тип
    (void*)&mgGrid_Brightness   // ДИСПЛЕЙ - СЕТКА - Яркость
};

static const Page mspGrid
(
    &pDisplay, 0,
    "СЕТКА", "GRID",
    "Содержит настройки отображения координатной сетки.",
    "Contains settings of display of a coordinate Grid::",
    NamePage::DisplayGrid, &itemsGrid
);


// ДИСПЛЕЙ - СЕТКА - Тип ----
static const Choice mcGrid_Type =
{
    TypeItem::Choice, &mspGrid, 0,
    {
        "Тип", "Type",
        "Выбор типа сетки.",
        "Choice like Grid::"
    },
    {
        {"Тип 1",   "Type 1"},
        {"Тип 2",   "Type 2"},
        {"Тип 3",   "Type 3"},
        {"Тип 4",   "Type 4"}
    },
    (int8*)&TYPE_GRID
};

// ДИСПЛЕЙ - СЕТКА - Яркость 
static const Governor mgGrid_Brightness
(
    &mspGrid, 0,
    "Яркость", "Brightness",
    "Устанавливает яркость сетки.",
    "Adjust the brightness of the Grid::",
    &BRIGHTNESS_GRID, 0, 100, OnChanged_Grid_Brightness, BeforeDraw_Grid_Brightness
);

ColorType colorTypeGrid = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::GRID};

void OnChanged_Grid_Brightness()
{
    colorTypeGrid.SetBrightness(BRIGHTNESS_GRID / 1e2f);
}

static void BeforeDraw_Grid_Brightness()
{
    colorTypeGrid.Init();
    BRIGHTNESS_GRID = (int16)(colorTypeGrid.brightness * 100.0f);
}


// ДИСПЛЕЙ - Смещение -------
static const Choice mcTypeShift =
{
    TypeItem::Choice, &pDisplay, 0,
    {
        "Смещение", "Offset"
        ,
        "Задаёт режим удержания смещения по вертикали\n1. \"Напряжение\" - сохраняется напряжение смещения.\n2. \"Деления\" - сохраняется положение "
        "смещения на экране."
        ,
        "Sets the mode of retaining the vertical displacement\n1. \"Voltage\" - saved dressing bias.\n2. \"Divisions\" - retained the position of "
        "the offset on the screen."
    },
    {
        {"Напряжение", "Voltage"},
        {"Деления", "Divisions"}
    },
    (int8*)&LINKING_RSHIFT
};


// ДИСПЛЕЙ - НАСТРОЙКИ /////////////
static const arrayItems itemsSettings =
{
    (void*)&mspSettings_Colors,                 // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА
    (void*)&mgSettings_Brightness,              // ДИСПЛЕЙ - НАСТРОЙКИ - Яркость
    (void*)&mgSettings_Levels,                  // ДИСПЛЕЙ - НАСТРОЙКИ - Уровни
    (void*)&mgSettings_TimeMessages,            // ДИСПЛЕЙ - НАСТРОЙКИ - Время
    (void*)&mcSettings_ShowStringNavigation,    // ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню
    (void*)&mcSettings_ShowAltMarkers,          // ДИСПЛЕЙ - НАСТРОЙКИ - Доп. маркеры
    (void*)&mcSettings_AutoHide                 // ДИСПЛЕЙ - НАСТРОЙКИ - Скрывать
};

static const Page mspSettings
(
    &pDisplay, 0,
    "НАСТРОЙКИ", "SETTINGS",
    "Дополнительные настройки дисплея",
    "Additional display settings",
    NamePage::ServiceDisplay, &itemsSettings
);


// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА /////
static const arrayItems itemsSettings_Colors =
{
    (void*)&mcSettings_Colors_Scheme,       // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Цветовая схема
    (void*)&mgcSettings_Colors_ChannelA,    // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 1
    (void*)&mgcSettings_Colors_ChannelB,    // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 2
    (void*)&mgcSettings_Colors_Grid,        // ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Сетка
    //(void*)&mgcColorChannel0alt,          // TODO Это для дополнительного цвета первого канала - для накопления, например.
    //(void*)&mgcColorChannel1alt,          // TODO Это для доп. цвета второго канала - для накопления, например.    
};

static const Page mspSettings_Colors
(
    &mspSettings, 0,
    "ЦВЕТА", "COLORS",
    "Выбор цветов дисплея",
    "The choice of colors display",
    NamePage::ServiceDisplayColors, &itemsSettings_Colors
);


// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Цветовая схема ------------------------------------------------------------------------------------------------------
static const Choice mcSettings_Colors_Scheme =
{
    TypeItem::Choice, &mspSettings_Colors, 0,
    {
        "Цветовая схема", "Color::E scheme",
        "Режим работы калибратора",
        "Mode of operation of the calibrator"
    },
    {   
        {"Схема 1", "Scheme 1"},
        {"Схема 2", "Scheme 2"}
    },
    (int8*)&COLOR_SCHEME
};

static ColorType colorT1 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::DATA_A};

// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 1 -------------------------------------------------------------------------------------------------------------
static const GovernorColor mgcSettings_Colors_ChannelA =
{
    TypeItem::GovernorColor, &mspSettings_Colors, 0,
    {
        "Канал 1", "Channel 1",
        "",
        ""
    },
    &colorT1
};

static ColorType colorT2 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, Color::DATA_B};

// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Канал 2 -------------------------------------------------------------------------------------------------------------
static const GovernorColor mgcSettings_Colors_ChannelB =
{
    TypeItem::GovernorColor, &mspSettings_Colors, 0,
    {
        "Канал 2", "Channel 2",
        "",
        ""
    },
    &colorT2
};


// ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА - Сетка ---------------------------------------------------------------------------------------------------------------
static const GovernorColor mgcSettings_Colors_Grid =
{
    TypeItem::GovernorColor, &mspSettings_Colors, 0,
    {
        "Сетка", "Grid",
        "Устанавливает цвет сетки",
        "Sets the grid color"
    },
    &colorTypeGrid
};


// ДИСПЛЕЙ - НАСТРОЙКИ - Яркость ---------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_Brightness
(
    &mspSettings, 0,
    "Яркость", "Brightness",
    "Установка яркости свечения дисплея",
    "Setting the brightness of the display",
    &BRIGHTNESS, 0, 100, OnChanged_Settings_Brightness
);

static void OnChanged_Settings_Brightness()
{
    HAL_DAC1::SetBrightness(BRIGHTNESS);
}


// ДИСПЛЕЙ - НАСТРОЙКИ - Уровни ----------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_Levels
(
    &mspSettings, 0,
    "Уровни", "Levels",
    "Задаёт время, в течение которого после поворота ручки сещения напряжения на экране остаётся вспомогательная метка уровня смещения",
    "Defines the time during which, after turning the handle visits to the voltage on the screen remains auxiliary label offset level",
    &TIME_SHOW_LEVELS, 0, 125
);


// ДИСПЛЕЙ - НАСТРОЙКИ - Время -----------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_TimeMessages
(
    &mspSettings, 0,
    "Время", "Time",
    "Установка времени, в течение которого сообщения будут находиться на экране",
    "Set the time during which the message will be on the screen",
    &TIME_MESSAGES, 1, 99
);


// ДИСПЛЕЙ - НАСТРОЙКИ - Строка меню -----------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_ShowStringNavigation =
{
    TypeItem::Choice, &mspSettings, 0,
    {
        "Строка меню", "Path menu",
        "При выборе \nПоказывать\n слева вверху экрана выводится полный путь до текущей страницы меню",
        "When choosing \nDisplay\n at the top left of the screen displays the full path to the current page menu"
    },
    {   
        {"Скрывать",    "Hide"},
        {"Показывать",  "Show"}
    },
    (int8*)&SHOW_STRING_NAVIGATION
};


// ДИСПЛЕЙ - НАСТРОЙКИ - Доп. маркеры ----------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_ShowAltMarkers =
{
    TypeItem::Choice, &mspSettings, 0,
    {
        "Доп. маркеры", "Alt. markers"
        ,
        "Устанавливает режим отображения дополнительных маркеров уровней смещения и синхронизации:\n"
        "\"Скрывать\" - дополнительные маркеры не показываются,\n"
        "\"Показывать\" - дополнительные маркеры показываются всегда,\n"
        "\"Авто\" - дополнительные маркеры показываются в течение 5 сек после поворота ручки смещения канала по напряжению или уровня синхронизации"
        ,
        "Sets the display mode of additional markers levels of displacement and synchronization:\n"
        "\"Hide\" - additional markers are not shown,\n"
        "\"Show\" - additional markers are shown always,\n"
        "\"Auto\" - additional markers are displayed for 5 seconds after turning the handle channel offset voltage or trigger level"
    },
    {   
        {"Скрывать",    "Hide"},
        {"Показывать",  "Show"},
        {"Авто",        "Auto"}
    },
    (int8*)&ALT_MARKERS, OnChanged_Settings_ShowAltMarkers
};

static void OnChanged_Settings_ShowAltMarkers(bool)
{
    Display::ChangedRShiftMarkers();
}


// ДИСПЛЕЙ - НАСТРОЙКИ - Скрывать --------------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_AutoHide =
{
    TypeItem::Choice, &mspSettings, 0,
    {
        "Скрывать", "Hide",
        "Установка после последнего нажатия кнопки или поворота ручки, по истечении которого меню автоматически убирается с экрана",
        "Installation after the last keystroke or turning the handle, after which the menu will automatically disappear"
    },
    {   
        {"Никогда",         "Never"},
        {"Через 5 сек",     "Through 5 s"},
        {"Через 10 сек",    "Through 10 s"},
        {"Через 15 сек",    "Through 15 s"},
        {"Через 30 сек",    "Through 30 s"},
        {"Через 60 сек",    "Through 60 s"}
    },
    (int8*)&MENU_AUTO_HIDE, OnChanged_Settings_AutoHide
};

static void OnChanged_Settings_AutoHide(bool autoHide)
{
    Menu::SetAutoHide(autoHide);
}
