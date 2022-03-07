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
    int     countEnables;               // ���������� ���������. ������������� ��� ������ ���������.
    int     countErasedFlashData;       // ������� ��� �������� ������ ������ � ���������.
    int     countErasedFlashSettings;   // ������� ��� �������� ������ � ����������.
    int     workingTimeInSecs;          // ����� ������ � ��������.
    Language lang;                      // ���� ����.
};


#define POS_ACT_ITEM(name_page) set.menu.posActItem[name_page]


struct SettingsMenu
{
    int8        posActItem[NamePage::Count];        // ������� �������� ������. bit7 == 1 - item is opened, 0x7f - ��� ��������� ������
    int8        currentSubPage[NamePage::Count];    // ����� ������� �����������.
    unsigned    isShown : 1;                        // ���� ��������.
};


// �������� ������ ��� ��������� �������.
struct Settings
{
    uint                size;
    SettingsDisplay     display;            // ��������� �����������          (���� �������).
    SettingsChannel     chan[Chan::Count];  // ��������� �������              (���� ����� 1 � ����� 2).
    SettingsTrig        trig;               // ��������� �������������        (���� �����).
    SettingsTime        time;               // ��������� ���������            (���� ���¨����).
    SettingsCursors     cursors;            // ��������� ��������� ���������  (���� �������).
    SettingsMemory      memory;             // ��������� ������� ������       (���� ������).
    SettingsMeasures    measures;           // ��������� ���������            (���� ���������).
    SettingsMath        math;               // ��������� ������ �������������� ���������.
    SettingsService     service;            // �������������� ���������       (���� ������).
    SettingsEthernet    eth;                // ��������� ��� ���������� �� ��������� ����.
    SettingsCommon      common;             // ��������� ���������.
    SettingsMenu        menu;               // ��������� ����.
    SettingsNRST        debug;              // ��������� ������ �������       (���� �������).
    uint                crc32;              // ����������� �����. ������������ ��� �������� ������������ ����������� ��������

    static bool isLoaded;               // ��� ���������� ����� ��� ����, ����� ��������� ������ ������ ������ �������� ��-�� ������������� 
                                        // ��������� ������� (��� ����������� �� ����� ����������� ��������� ������ ������ �� ������ ���������� 
                                        // ������� �������� �������, ��� ��������� �������� ��������� ���������).

    static bool needReset;              // ���� 1, �� ��������� ����� ��������

    static void Load(bool _default_);   // ��������� ���������. ���� _default == true, ����������� ��������� �� ���������, ����� �������� 
                                        // ��������� ��������� �� ���, � � ������ ������� - ���� ��������� �� ���������.
    static void SaveBeforePowerDown();  // ��������� ��������� �� ����-������ ����� �����������
    static void NeedSave();             // ���������� ������� ����, ��� ��������� ��������� � ����������
    static void SaveIfNeed();           // ��������� ���������, ���� �����

private:

    static bool need_save;              // ������� ����, ��� ��������� ����� ��������� � ������� �����
    static uint time_save;              // � ��� ����� ����� ���������
};

extern Settings set;

