// 2022/03/29 16:54:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/LAN/LAN.h"
#include "Hardware/LAN/SocketTCP.h"
#include "Hardware/LAN/ethernetif.h"
#include "Hardware/LAN/app_ethernet.h"
#include "Settings/Settings.h"
#include "SCPI/SCPI.h"
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>
#include <cstring>
#include <cstdarg>


namespace LAN
{
    bool cableIsConnected;
    bool clientIsConnected;
}


struct netif gnetif;


static void Netif_Config(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    /* Add the network interface */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /* Registers the default network interface */
    netif_set_default(&gnetif);

    if (netif_is_link_up(&gnetif))
    {
        /* When the netif is fully configured this function must be called */
        netif_set_up(&gnetif);
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&gnetif);
    }

    /* Set the link callback function, this function is called on change of link status */
    netif_set_link_callback(&gnetif, ethernetif_update_config);
}


void LAN::Init()
{
    /* Initialize the LwIP stack */
    lwip_init();

    /* Configure the Network interface */
    Netif_Config();

    /* tcp echo server Init */
    TCP::Init(SCPI::AppendNewData);

    /* Notify user about the netwoek interface config */
    User_notification(&gnetif);
}


void LAN::Update()
{
    // Read a received packet from the Ethernet buffers and send it to the lwIP for handling
    ethernetif_input(&gnetif);

    // Handle timeouts
    sys_check_timeouts();
}
