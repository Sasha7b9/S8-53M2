// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "SettingsTypes.h"
#include "Data/Measures.h"
#include "Menu/MenuItems.h"
#include "Panel/Controls.h"
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

#define RSHIFT_ADD(ch, range, closed) set.chan[ch].rShiftAdd[range][closed]

 // Настройки каналов
struct SettingsChannel
{
    int8          balanceADC;                   // Баланс АЦП (смещение первой точки относительно второй)
    float         stretchADC;                   // Коэффицент растяжки (на него умножается сигнал)
    int8          shiftADC;                     // Добавочное смещение для балансировки АЦП.

    RShift        rShiftRel;
    int16         rShiftAdd[Range::Count][2];   // Добавочное смещение для открытого (0) и закрытого (1) входов.

    ModeCouple::E modeCouple;                   // Режим по входу.
    Divider::E    divider;                      // Множитель.
    Range::E      range;                        // Масштаб по напряжению.
    bool          enable;                       // Включён ли канал.
    bool          inverse;                      // Инвертирован ли канал.
    bool          filtr;                        // Фильтр
};


#define MAC_ADDR0       (set.eth.mac0)
#define MAC_ADDR1       (set.eth.mac1)
#define MAC_ADDR2       (set.eth.mac2)
#define MAC_ADDR3       (set.eth.mac3)
#define MAC_ADDR4       (set.eth.mac4)
#define MAC_ADDR5       (set.eth.mac5)

#define IP_ADDR0        (set.eth.ip0)
#define IP_ADDR1        (set.eth.ip1)
#define IP_ADDR2        (set.eth.ip2)
#define IP_ADDR3        (set.eth.ip3)

#define LAN_PORT        (set.eth.port)

#define NETMASK_ADDR0   (set.eth.mask0)
#define NETMASK_ADDR1   (set.eth.mask1)
#define NETMASK_ADDR2   (set.eth.mask2)
#define NETMASK_ADDR3   (set.eth.mask3)

#define GW_ADDR0        (set.eth.gw0)
#define GW_ADDR1        (set.eth.gw1)
#define GW_ADDR2        (set.eth.gw2)
#define GW_ADDR3        (set.eth.gw3)

#define ETH_ENABLE      (set.eth.enable)

struct SettingsEthernet
{
    uint8 mac0;
    uint8 mac1;
    uint8 mac2;
    uint8 mac3;
    uint8 mac4;
    uint8 mac5;

    uint8 ip0;
    uint8 ip1;
    uint8 ip2;
    uint8 ip3;

    uint16 port;

    uint8 mask0;
    uint8 mask1;
    uint8 mask2;
    uint8 mask3;

    uint8 gw0;
    uint8 gw1;
    uint8 gw2;
    uint8 gw3;

    bool enable;
};



#define LANG            (set.common.lang)       // SettingsCommon.lang
#define LANG_RU         (LANG == Russian)
#define LANG_EN         (LANG == English)
                                    
struct SettingsCommon
{
    int     countEnables;               // Количество включений. Увеличивается при каждом включении.
    int     countErasedFlashData;       // Сколько раз стирался первый сектор с ресурсами.
    int     countErasedFlashSettings;   // Сколько раз стирался сектор с настройкаи.
    int     workingTimeInSecs;          // Время работы в секундах.
    Language lang;                      // Язык меню.
};



struct SettingsMenu
{
    int8        posActItem[Page_NumPages];      // Позиция ативного пункта. bit7 == 1 - item is opened, 0x7f - нет активного пункта
    int8        currentSubPage[Page_NumPages];  // Номер текущей подстраницы.
    bool        pageDebugActive;                // Активна ли кнопка отладки в меню.
    unsigned    isShown : 1;                    // Меню показано.
};



int8 MenuPosActItem(NamePage namePage);                         // Возвращает позицию активного пункта на странице namePage.
void SetMenuPosActItem(NamePage namePage, int8 pos);            // Установить позицию активного пункта на странице namePage.
int8 MenuCurrentSubPage(NamePage namePage);                     // Возвращает номер текущей подстраницы страницы namePage.
void SetMenuCurrentSubPage(NamePage namePage, int8 posSubPage); // Устанавливает номер текущей подстраницы в странице namePage.
bool MenuIsShown();                                             // Отображено ли меню на экране.
void ShowMenu(bool show);                                       // Отобразить/скрыть меню.
bool MenuIsMinimize();                                          // Если true - меню находится в дополнительном режиме.
bool MenuPageDebugIsActive();                                   // Активна ли страница отладки.
void SetMenuPageDebugActive(bool active);                       // Сделать/разделать активной страницу отладки.
void CurrentPageSBregSet(int angle);                            // Повернуть ручку УСТАНОВКА на текущей странице малых кнопок.
const SmallButton*  GetSmallButton(Key::E);         // Вернуть указатель на малую кнопку, соответствующую данной кнопки панели.


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
    uint                crc32;              // контрольная сумма. Используется для проверки корректности сохранённых настроек
    //int temp[5];
    static void Load(bool _default_);   // Загрузить настройки. Если _default == true, загружаются настройки по умолчанию, иначе пытается 
                                        // загрузить настройки из ПЗУ, а в случае неудачи - тоже настройки по умолчанию.
    static void SaveBeforePowerDown();  // Сохранить настройки во флеш-память перед выключением
    static void NeedSave();             // Установить признак того, что настройки нуждаются в сохранении
    static void SaveIfNeed();           // Сохранить настройки, если нужно
    static bool DebugModeEnable();      // Возвращает true, если включён режим отладки.

private:

    static bool need_save;              // Признак того, что настройки нужно сохранить в главном цикле
    static uint time_save;              // В это время нужно сохранять
};

extern Settings set;

