// 2022/02/11 17:46:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Ethernet/LAN.h"
#include "TcpSocket.h"
#include "main.h"
#include "Hardware/Timer.h"
#include "ethernetif.h"
#include "VCP/SCPI/SCPI.h"
#include "Log.h"
#include <lwip/init.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>


static void Netif_Config();

static struct netif gnetif;


static void FuncConnect()
{
}


char *GetStringFromBuffer(pchar buffer, uint length, char *string)
{
    memcpy(string, buffer, (uint)length);
    string[length] = 'E';
    string[length + 1] = '\0';
    return string;
}


static void FuncReceiver(pchar buffer, uint length)
{
    SCPI::AddNewData((uint8 *)buffer, length);
}


void LAN::Init()
{
    lwip_init();

    Netif_Config();

    TCPSocket_Init(FuncConnect, FuncReceiver);
}


void LAN::Update(uint timeMS)
{
    uint time = TIME_MS;

    do 
    {
        CABLE_LAN_IS_CONNECTED = (TIME_MS - gEthTimeLastEthifInput <= 1500) ? 1U : 0U;

        ethernetif_input(&gnetif);
        sys_check_timeouts();
    } while (TIME_MS - time < timeMS);
}


static void Netif_Config()
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    // IP address default setting
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    // add the network interface
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    // Registers the default network interface
    netif_set_default(&gnetif);

    if (netif_is_link_up(&gnetif))
    {
        // When the netif is fully configured this function must be called
        netif_set_up(&gnetif);
    }
    else
    {
        // When the netif link is down this function must be called
        netif_set_down(&gnetif);
    }

    // Set the link callback function, this function is called on change of link status
    netif_set_link_callback(&gnetif, ethernetif_update_config);
}
