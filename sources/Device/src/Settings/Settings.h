// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "SettingsTypes.h"
#include "Data/Measures.h"
#include "Menu/MenuItems.h"
#include "SettingsChannel.h"
#include "SettingsDisplay.h"
#include "SettingsMemory.h"
#include "SettingsTime.h"
#include "SettingsTrig.h"
#include "SettingsService.h"
#include "SettingsCursors.h"
#include "SettingsDebug.h"
#include "SettingsMemory.h"
#include "SettingsMeasures.h"
#include "SettingsMath.h"


#define LANG            (set.common.lang)       // SettingsCommon.lang
#define LANG_RU         (LANG == Lang::Ru)
#define LANG_EN         (LANG == Lang::En)

#define POS_ACT_ITEM(name_page) set.menu.posActItem[name_page]


struct SettingsCommon
{
    int     countEnables;               // Количество включений. Увеличивается при каждом включении.
    int     countErasedFlashData;       // Сколько раз стирался первый сектор с ресурсами.
    int     countErasedFlashSettings;   // Сколько раз стирался сектор с настройкаи.
    int     workingTimeInSecs;          // Время работы в секундах.
    Lang::E lang;                      // Язык меню.
};


struct SettingsMenu
{
    int8        posActItem[NamePage::Count];        // Позиция ативного пункта. bit7 == 1 - item is opened, 0x7f - нет активного пункта
    int8        currentSubPage[NamePage::Count];    // Номер текущей подстраницы.
    unsigned    isShown : 1;                        // Меню показано.
};


// Струкура хранит все настройки прибора.
struct Settings
{
    uint                size;
    SettingsDisplay     display;            // настройки изображения          (меню ДИСПЛЕЙ).
    SettingsChannel     chan[Chan::Count];  // настройки каналов              (меню КАНАЛ 1 и КАНАЛ 2).
    SettingsTrig        trig;               // настройки синхронизации        (меню СИНХР).
    SettingsTime        time;               // временнЫе настройки            (меню РАЗВЁРТКА).
    SettingsCursors     cursors;            // настройки курсорных измерений  (меню КУРСОРЫ).
    SettingsMemory      memory;             // настройки режимов памяти       (меню ПАМЯТЬ).
    SettingsMeasures    measures;           // настройки измерений            (меню ИЗМЕРЕНИЯ).
    SettingsMath        math;               // настройки режима математических измерений.
    SettingsService     service;            // дополнительные настройки       (меню СЕРВИС).
    SettingsEthernet    eth;                // настройки для соединения по локальной сети.
    SettingsCommon      common;             // системные настройки.
    SettingsMenu        menu;               // состояние меню.
    SettingsNRST        debug;              // настройки режима отладки       (меню ОТЛАДКА).

    static bool isLoaded;               // Эта переменная нужна для того, чтобы исключить ложную запись пустых настроек
                                        // из-за неправильного включения прибора (при исключённом из схемы программном
                                        // включении иногда сигнал от кнопки отключения питания приходит быстрее, чем
                                        // программа успевает настроить настройки).

    static bool needReset;              // Если 1, то необходим сброс настроек

    static void Load();                 // Загрузить настройки. Если _default == true, загружаются настройки по
                                        // умолчанию, иначе пытается загрузить настройки из ПЗУ, а в случае неудачи -
                                        // тоже настройки по умолчанию.
    static void Reset();                // Сбросить настройки, сохранив калибровочные настройки
    static void SaveBeforePowerDown();  // Сохранить настройки во флеш-память перед выключением
    static void NeedSave();             // Установить признак того, что настройки нуждаются в сохранении
    static void SaveIfNeed();           // Сохранить настройки, если нужно
    static void RunAfterLoad();         // После загрузки настроек нужно выполнить эти команды

private:

    static bool need_save;              // Признак того, что настройки нужно сохранить в главном цикле
    static uint time_save;              // В это время нужно сохранять
};

extern Settings set;

