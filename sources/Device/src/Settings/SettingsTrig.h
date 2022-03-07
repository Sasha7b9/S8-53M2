// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "FPGA/SettingsFPGA.h"


#define START_MODE                          (set.trig.startMode)
#define START_MODE_IS_SINGLE                (START_MODE == StartMode::Single)
#define START_MODE_IS_AUTO                  (START_MODE == StartMode::Auto)

#define TRIG_SOURCE                         (set.trig.source)
#define TRIG_SOURCE_IS_EXT                  (TRIG_SOURCE == TrigSource::Ext)

#define TRIG_POLARITY                       (set.trig.polarity)
#define TRIG_POLARITY_IS_FRONT              (TRIG_POLARITY == TrigPolarity::Front)

#define TRIG_INPUT                          (set.trig.input)
#define TRIG_INPUT_IS_AC                    (TRIG_INPUT == TrigInput::AC)

#define TRIG_LEVEL(source)                  (set.trig.levelRel[source])
#define TRIG_LEVEL_SOURCE                   (TRIG_LEVEL(TRIG_SOURCE))
#define TRIG_LEVEL_A                        (TRIG_LEVEL(Chan::A))
#define TRIG_LEVEL_B                        (TRIG_LEVEL(Chan::B))

#define MODE_LONG_PRESS_TRIG                (set.trig.modeLongPressTrig)
#define MODE_LONG_PRESS_TRIG_IS_LEVEL_ZERO  (MODE_LONG_PRESS_TRIG == ModeLongPressTrig::LevelZero)

#define TRIG_MODE_FIND                      (set.trig.modeFind)
#define TRIG_MODE_FIND_IS_AUTO              (TRIG_MODE_FIND == TrigModeFind::Auto)
#define TRIG_MODE_FIND_IS_HAND              (TRIG_MODE_FIND == TrigModeFind::Hand)




// ����� �������.
struct StartMode
{
    enum E
    {
        Auto,       // ��������������.
        Wait,       // ������.
        Single      // �����������.
    };
};


// ����� ����������� ������� ������ �����.
struct ModeLongPressTrig
{
    enum E
    {
        LevelZero,  // ����� ������ ������������� � 0.
        Auto        // �������������� ����� ������������� - ������� ��������������� ���������� ����� ���������� � ���������.
    };
};


// ����� ��������� �������������.
struct TrigModeFind
{
    enum E
    {
        Hand,       // ������� ������������� ��������������� �������.
        Auto        // ���������� ������ ������������� ������������ ������������� ����� ������� ������ ���������� �������.
    };
};




// ��������� �������������
struct SettingsTrig
{
    StartMode::E         startMode;          // ����� �������.
    TrigSource::E        source;             // ��������.
    TrigPolarity::E      polarity;           // ��� �������������.
    TrigInput::E         input;              // ���� �������������.
    TrigLev              levelRel[3];        // ������� ������������� ��� ��� ����������.
    ModeLongPressTrig::E modeLongPressTrig;  // ����� ������ ����������� ������� ������ �����.
    TrigModeFind::E      modeFind;           // ����� ������������� - ������� ��� �������������.
};
