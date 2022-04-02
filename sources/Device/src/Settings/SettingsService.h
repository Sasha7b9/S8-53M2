// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define SOUND_ENABLED       (set.service.soundEnabled)

#define CALIBRATOR          (set.service.calibrator)
#define CALIBRATOR_IS_FREQ  (CALIBRATOR == CalibratorMode::Freq)
#define CALIBRATOR_IS_DC    (CALIBRATOR == CalibratorMode::DC)

#define IP_ADDRESS          (set.service.IPaddress)

#define COLOR_SCHEME                    (set.service.colorScheme)
#define IS_COLOR_SCHEME_WHITE_LETTERS   (COLOR_SCHEME == ColorScheme::WhiteLetters)


// Цветовая схема
struct ColorScheme
{
    enum E
    {
        WhiteLetters,   // В этом случае заголовки элементов меню пишутся белым - не очень хорошо видно снизу.
        BlackLetters    // В этом случае заголовки элементов меню пишутся чёрным - не очень красиво выглядит.
    };
};


// Настройки меню СЕРВИС.
struct SettingsService
{ //-V802
    bool              screenWelcomeEnable;    // Будет ли показываться экран приглашения при включении прибора. \todo убрать этот артефакт.
    bool              soundEnabled;           // Включены ли звуки.
    CalibratorMode::E calibrator;             // Режим работы калибратора.
    int8              IPaddress;              // IP-адрес (временно).
    ColorScheme::E    colorScheme;            // Цветовая схеама.
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
