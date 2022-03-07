// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "SettingsTypes.h"


#define START_MODE              (set.trig.startMode)
#define START_MODE_IS_SINGLE    (START_MODE == StartMode::Single)
#define START_MODE_IS_AUTO      (START_MODE == StartMode::Auto)

#define TRIG_SOURCE             (set.trig.source)
#define TRIG_SOURCE_IS_EXT      (TRIG_SOURCE == TrigSource::Ext)

#define TRIG_POLARITY           (set.trig.polarity)                     // SettingsTrig.polarity
#define TRIG_POLARITY_IS_FRONT  (TRIG_POLARITY == TrigPolarity::Front)   // \c true, если синхронизация по фронту.

#define TRIG_INPUT              (set.trig.input)                        // SettingsTrig.input
#define TRIG_INPUT_IS_AC        (TRIG_INPUT == TrigInput::AC)            // \c true, если закрытый вход синхронизации.

#define TRIG_LEVEL(source)      (set.trig.levelRel[source])             // SettingsTrig.levelRel
#define TRIG_LEVEL_SOURCE       (TRIG_LEVEL(TRIG_SOURCE))               // set.trig.levelRel[set.trig.source]
#define TRIG_LEVEL_A            (TRIG_LEVEL(Chan::A))                         // set.trig.levelRel[Chan::A]
#define TRIG_LEVEL_B            (TRIG_LEVEL(Chan::B))                         // set.trig.levelRel[Chan::B]

#define MODE_LONG_PRESS_TRIG    (set.trig.modeLongPressTrig)            // SettingsTrig.modeLongPressTrig
// если \c true, то длительное нажатие кнопки \b СИНХР сбрасывает уровень синхронизации в 0.
#define MODE_LONG_PRESS_TRIG_IS_LEVEL0  (MODE_LONG_PRESS_TRIG == ModeLongPRessTrig_Level0)

#define TRIG_MODE_FIND          (set.trig.modeFind)                     // SettingsTrig.modeFind
#define TRIG_MODE_FIND_IS_AUTO  (TRIG_MODE_FIND == TrigModeFind_Auto)   // \c true, если автоматическая установка синхронизации.
#define TRIG_MODE_FIND_IS_HAND  (TRIG_MODE_FIND == TrigModeFind_Hand)   // \c true, если синхронизация устанавливается вручную.




// Режим запуска.
struct StartMode
{
    enum E
    {
        Auto,             // Автоматический.
        Wait,             // Ждущий.
        Single            // Однократный.
    };
};


// Режим длительного нажатия кнопки СИНХР.
enum ModeLongPressTrig
{
    ModeLongPRessTrig_Level0,   // Сброс уровня синхронизации в 0.
    ModeLongPressTrig_Auto      // Автоматический поиск синхронизации - уровень устанавливается посередине между максимумом и минимумом.
};


// Режим установки синхронизации.
enum TrigModeFind
{
    TrigModeFind_Hand,          // Уровень синхронизации устанавливается вручную.
    TrigModeFind_Auto           // Подстройки уровня синхронизации производится автоматически после каждого нового считанного сигнала.
};




// Настройки синхронизации
struct SettingsTrig
{
    StartMode::E        startMode;          // Режим запуска.
    TrigSource::E       source;             // Источник.
    TrigPolarity::E     polarity;           // Тип синхронизации.
    TrigInput::E        input;              // Вход синхронизации.
    TrigLev             levelRel[3];        // Уровень синхронизации для трёх источников.
    ModeLongPressTrig   modeLongPressTrig;  // Режим работы длительного нажатия кнопки СИНХР.
    TrigModeFind        modeFind;           // Поиск синхронизации - вручную или автоматически.
};
