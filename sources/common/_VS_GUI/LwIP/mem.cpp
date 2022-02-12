#include "defines.h"
#include <lwip/mem.h>
#include <lwip/pbuf.h>


void *mem_malloc(mem_size_t)
{
    return nullptr;
}

void  mem_free(void *)
{

}

struct pbuf *pbuf_alloc(pbuf_layer, u16_t, pbuf_type)
{
    return nullptr;
}


void pbuf_ref(struct pbuf *)
{

}


u8_t pbuf_free(struct pbuf *)
{
    return 0;
}


void pbuf_chain(struct pbuf *, struct pbuf *)
{

}


err_t pbuf_take(struct pbuf *, const void *, u16_t)
{
    return 0;
}
