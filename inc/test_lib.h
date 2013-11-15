#ifndef _TEST_LIB_H_
#define _TEST_LIB_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#include "typedef.h"
#include "j1939.h"

/* #define RECVER2SENDER                   ("recver2sender.can") */
#define SENDER2RECVER                   ("sender2recver.can")
#define SENDER2RECVER_LOCK              ("sender2recver.lock")

#define THREAD_NUMS                     (2)

#define MSG_LENGTH                      (10)
#define IDLE_SLEEP_SECONDS              (10*1000*1000L)
#define WHILE_TRUE_SLEEP_SECONDS        (1 * 1000 * 1000L * (1.0 * rand() / RAND_MAX))
#define PUSH_SLEEP_US_MAX               (1000000)
#define PULL_SLEEP_US_MAX               (1000000)

typedef enum _ECU_TYPE{
    recver,
    sender
}ECU_TYPE;

typedef struct _SENDER_APP_PARA{
    ECU_TYPE type;
    SENDDED_MSG_TYPE msgs[MSG_LENGTH];
}SENDER_APP_PARA;

typedef struct _SENDER_INT_PARA{
    ECU_TYPE type;
    const INT8 *write;
}SENDER_INT_PARA;

typedef struct _RECVER_APP_PARA{
    ECU_TYPE type;
}RECVER_APP_PARA;

typedef struct _RECVER_INT_PARA{
    ECU_TYPE type;
    const INT8 *read;
}RECVER_INT_PARA;

extern INT32 ecu_main(const INT8 *fifo, ECU_TYPE type);

extern INT32 mk_threads(
        const THREAD_FUNC *threads,
        pthread_t *threads_id,
        const void **threads_para,
        INT32 thread_nums);

extern void *sender_app_thread(void *arg);
extern void *sender_int_thread(void *arg);
extern void *recver_app_thread(void *arg);
extern void *recver_int_thread(void *arg);

#endif /* _TEST_LIB_H_ */
