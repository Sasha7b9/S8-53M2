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
#include "SettingsNRST.h"
#include "SettingsMemory.h"
#include "SettingsMeasures.h"
#include "SettingsMath.h"


#define LANG            (gset.menu.lang)
#define LANG_RU         (LANG == Lang::Ru)
#define LANG_EN         (LANG == Lang::En)

#define POS_ACT_ITEM(name_page) gset.menu.posActItem[name_page]


struct SettingsMenu
{
    int8        posActItem[NamePage::Count];        // ������� �������� ������. bit7 == 1 - item is opened, 0x7f - ��� ��������� ������
    int8        currentSubPage[NamePage::Count];    // ����� ������� �����������.
    Lang::E     lang;                               // ���� ����.
    unsigned    isShown : 1;                        // ���� ��������.
};


// �������� ������ ��� ��������� �������.
struct Settings
{
    static const int SIZE_FIELD_RECORD = 1024;

    uint                crc32;
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
    SettingsMenu        menu;               // ��������� ����.
    uint                notUsed;

    void Load();                    // ��������� ���������. ���� _default == true, ����������� ��������� ��
                                    // ���������, ����� �������� ��������� ��������� �� ���, � � ������ ������� -
                                    // ���� ��������� �� ���������.
    bool Save();                    // ��������� ��������� �� ����-������ ����� �����������

    void Reset();                   // �������� ���������, �������� ������������� ���������
    void RunAfterLoad();            // ����� �������� �������� ����� ��������� ��� �������

    uint CalculateCRC32();
};

extern Settings gset;

