/**********************************************************************************************************
 *                      Wuhan zhongyuan electronic applied electronic R&D center
 *                                      All rights reserved.2012
 *
 ==========================================================================================================
 *
 * file name:                recver.c
 *
 * description:              模拟J1939协议上的车载平台
 *
 * controller:               Freescale i.MX536
 *
 * compiler:                 cygwin gcc4.5.3
 *
 ==========================================================================================================
 *
 * author:                   pp
 *
 * completion date:          2012-8-1
 *
 ==========================================================================================================*/
#include "test_lib.h"

static void *app_thread(void *arg);
static void *int_thread(void *arg);

extern int main(int argc, char *argv[])
{
    INT32 rst = 0;
    INT8 *fifo = NULL;
#if (DEBUG_TEST & PP_DEBUG)
    INT32 i = 0;
#endif

    pthread_t threads_id[THREAD_NUMS];
    THREAD_FUNC threads[THREAD_NUMS];
    RECVER_APP_PARA app_para;
    RECVER_INT_PARA int_para;
    const void *threads_para[THREAD_NUMS];

    j1939_init();

#if (DEBUG_TEST & PP_DEBUG)
    assert(NULL != argv);
    printf("%s,%d: %s from bus(%s).\n", __FILE__, __LINE__, "recver", argv[1]);

    printf("%s,%d:argc=%d\n", __FILE__, __LINE__, argc);
    for(i=0; i<argc; i++)
    {
        printf("%s,%d:argv[%d]=%s\n", __FILE__, __LINE__, i, argv[i]);
    }
#endif

    fifo = argv[1];

    threads[0] = app_thread;
    threads[1] = int_thread;

    app_para.type = recver;

    int_para.type = recver;
    int_para.read = fifo;

    threads_para[0] = (void *)(&app_para);
    threads_para[1] = (void *)(&int_para);

    rst = mk_threads(threads, threads_id, threads_para, THREAD_NUMS);
    assert(0 == rst);

    while(true)
    {
        sleep(IDLE_SLEEP_SECONDS);
    }

    return 0;
}

static void *app_thread(void *arg)
{
    INT32 i = 1;
    INT32 rst = 0;
    INT32 sleep_us = 0;
    RECVED_MSG recv_msg = {0};

    /* 不使用参数 */
    assert(NULL != arg);

    while(true)
    {
        pull_msg_from_stack(&recv_msg);

        sleep_us = PULL_SLEEP_US_MAX * 1.0 * rand() / RAND_MAX;
        rst = usleep(sleep_us);
        assert(0 == rst);

        printf("pull the %d msg sleep:%dus from bus.\n", i++, sleep_us);
        print_recved_msg(&recv_msg);
    }

    return NULL;
}

static void *int_thread(void *arg)
{

    INT32 rst = 0;
    CAN can;

    INT32 fd_bus_file = 0;
    RECVER_INT_PARA *ptr_para = (RECVER_INT_PARA *)arg;
    const INT8 *file_path = ptr_para->read;

    assert(NULL != arg);

    fd_bus_file = open(file_path, O_RDONLY | O_SYNC);
    if(-1 == fd_bus_file)
    {
        printf("%s,%d:%s:%s\n", __FILE__, __LINE__, strerror(errno), file_path);
        return NULL;
    }

    while(true)
    { 
        rst = read(fd_bus_file, &can, sizeof(CAN));
#if (DEBUG_TEST & PP_DEBUG)
        printf("%s,%d: recved %d bytes from bus(%s).\n", __FILE__, __LINE__, rst, file_path);
#endif
        assert(sizeof(CAN) == rst);

        enqueue_can_to_rx_fifo(&can);
    }

    close(fd_bus_file);

    return NULL;
}

