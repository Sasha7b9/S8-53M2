// 2022/2/11 19:22:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "lwip/err.h"
#include "lwip/netif.h"

/* Exported types ------------------------------------------------------------*/
err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);
void ethernetif_set_link(struct netif *netif);
void ethernetif_update_config(struct netif *netif);
void ethernetif_notify_conn_changed(struct netif *netif);

extern uint gEthTimeLastEthifInput;
