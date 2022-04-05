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
    int     countEnables;               // ���������� ���������. ������������� ��� ������ ���������.
    int     countErasedFlashData;       // ������� ��� �������� ������ ������ � ���������.
    int     countErasedFlashSettings;   // ������� ��� �������� ������ � ����������.
    int     workingTimeInSecs;          // ����� ������ � ��������.
    Lang::E lang;                      // ���� ����.
};


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

    static bool isLoaded;               // ��� ���������� ����� ��� ����, ����� ��������� ������ ������ ������ ��������
                                        // ��-�� ������������� ��������� ������� (��� ����������� �� ����� �����������
                                        // ��������� ������ ������ �� ������ ���������� ������� �������� �������, ���
                                        // ��������� �������� ��������� ���������).

    static bool needReset;              // ���� 1, �� ��������� ����� ��������

    static void Load();                 // ��������� ���������. ���� _default == true, ����������� ��������� ��
                                        // ���������, ����� �������� ��������� ��������� �� ���, � � ������ ������� -
                                        // ���� ��������� �� ���������.
    static void Reset();                // �������� ���������, �������� ������������� ���������
    static void SaveBeforePowerDown();  // ��������� ��������� �� ����-������ ����� �����������
    static void NeedSave();             // ���������� ������� ����, ��� ��������� ��������� � ����������
    static void SaveIfNeed();           // ��������� ���������, ���� �����
    static void RunAfterLoad();         // ����� �������� �������� ����� ��������� ��� �������

private:

    static bool need_save;              // ������� ����, ��� ��������� ����� ��������� � ������� �����
    static uint time_save;              // � ��� ����� ����� ���������
};

extern Settings set;

