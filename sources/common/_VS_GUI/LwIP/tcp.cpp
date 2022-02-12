#include "defines.h"
#include <lwip/tcp.h>


tcp_pcb *tcp_new()
{
    return nullptr;
}


void tcp_arg(struct tcp_pcb *, void *)
{

}


void tcp_recv(struct tcp_pcb *, tcp_recv_fn)
{

}


void tcp_sent(struct tcp_pcb *, tcp_sent_fn)
{

}


void tcp_err(struct tcp_pcb *, tcp_err_fn)
{

}


void tcp_accept(struct tcp_pcb *, tcp_accept_fn)
{

}


void tcp_poll(struct tcp_pcb *, tcp_poll_fn, u8_t)
{

}


void tcp_recved(struct tcp_pcb *, u16_t)
{

}


err_t tcp_bind(struct tcp_pcb *, const ip_addr_t *, u16_t)
{
    return 0;
}


struct tcp_pcb *tcp_listen_with_backlog(struct tcp_pcb *, u8_t)
{
    return nullptr;
}


void tcp_abort(struct tcp_pcb *)
{

}


err_t tcp_close(struct tcp_pcb *)
{
    return 0;
}


err_t tcp_write(struct tcp_pcb *, const void *, u16_t, u8_t)
{
    return 0;
}


void tcp_setprio(struct tcp_pcb *, u8_t)
{

}
