#include "lib.h"

static QUEUE g_tx_fifo;
static QUEUE g_rx_fifo;

static void fifo_init(void);

extern void j1939_init(void)
{
    debug_init();
    fifo_init();

    al_init();
    dl_init();
    pl_init();

    return;
}

extern void debug_init(void)
{
    INT32 rst = 0;

#ifdef PP_DEBUG
    rst = setvbuf(stdout, NULL, _IONBF, 0);
    assert(0 == rst);
    setvbuf(stderr, NULL, _IONBF, 0);
    assert(0 == rst);
#endif

    return;
}

extern void enqueue_can_to_rx_fifo(const CAN *ptr_can)
{
    enqueue(&g_rx_fifo, ptr_can);

    return;
}

extern void enqueue_can_to_tx_fifo(const CAN *ptr_can)
{
    enqueue(&g_tx_fifo, ptr_can);

    return;
}

extern void dequeue_can_from_tx_fifo(CAN *ptr_can)
{
    dequeue(ptr_can, &g_tx_fifo);

    return;
}

extern void dequeue_can_from_rx_fifo(CAN *ptr_can)
{
    dequeue(ptr_can, &g_rx_fifo);

    return;
}

static void fifo_init(void)
{
    queue_init(&g_tx_fifo);
    queue_init(&g_rx_fifo);

    return;
}

