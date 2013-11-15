/**********************************************************************************************************
 *                      Wuhan zhongyuan electronic applied electronic R&D center
 *                                      All rights reserved.2012
 *
 ==========================================================================================================
 *
 * file name:                sender.c
 *
 * description:              模拟J1939协议上发送设备
 *
 * controller:               Freescale i.MX536
 *
 * compiler:                 cygwin gcc4.5.3
 *
 ==========================================================================================================
 *
 * author:                   pp
 *
 * completion date:          2012-8-22
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
    SENDER_APP_PARA app_para;
    SENDER_INT_PARA int_para;
    const void *threads_para[THREAD_NUMS];

    j1939_init();

#if (DEBUG_TEST & PP_DEBUG)
    assert(NULL != threads);
    printf("%s,%d: %s to bus(%s).\n", __FILE__, __LINE__, "sender", argv[1]);
    printf("%s,%d:argc=%d\n", __FILE__, __LINE__, argc);
    for(i=0; i<argc; i++)
    {
        printf("%s,%d:argv[%d]=%s\n", __FILE__, __LINE__, i, argv[i]);
    }
#endif

    fifo = argv[1];

    threads[0] = app_thread;
    threads[1] = int_thread;

    app_para.type = sender;
    app_para.msgs[0] = tx_engine_speed;
    int_para.type = sender;
    int_para.write = fifo;

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

    SENDER_APP_PARA *ptr_para = (SENDER_APP_PARA *)arg;
    SENDDED_MSG_TYPE *msgs = ptr_para->msgs;

    assert(NULL != arg);

    while(true)
    {
        push_msg_to_stack(msgs[0]);
        assert(NULL != get_sendded_msg(msgs[0]));

        sleep_us = PUSH_SLEEP_US_MAX * 1.0 * rand() / RAND_MAX;
        rst = usleep(sleep_us);
        assert(0 == rst);

        printf("push the %d msg sleep:%dus type:%s\n", i++, sleep_us, get_sendded_msg(msgs[0]));
    }

    return NULL;
}

static void *int_thread(void *arg)
{
    INT32 fd_bus_file = 0;
    INT32 rst = 0;
    CAN can = {0};
    SENDER_INT_PARA *ptr_para = (SENDER_INT_PARA *)arg;
    const INT8 *file_path = ptr_para->write;

    assert(NULL != arg);

    fd_bus_file = open(file_path, O_WRONLY | O_APPEND | O_SYNC);
    if(-1 == fd_bus_file)
    {
        printf("%s,%d:%s:%s.\n", __FILE__, __LINE__, strerror(errno), file_path);
        return NULL;
    }

    while(true)
    {
        dequeue_can_from_tx_fifo(&can);

        rst = write(fd_bus_file, (void *)&can, sizeof(CAN)); 
#if (DEBUG_TEST & PP_DEBUG)
        printf("%s,%d: sendded %d bytes to bus(%s).\n", __FILE__, __LINE__, rst, file_path);
#endif 
        assert(sizeof(CAN) == rst);
    }

    close(fd_bus_file); 

    return NULL;
}

