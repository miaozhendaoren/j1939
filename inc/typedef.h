#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <pthread.h>

#include "config.h"

/*=================================  类型定义 ==========================================*/
typedef unsigned char                           BOOL;
typedef unsigned char                           UINT8;
typedef char                                    INT8;
typedef unsigned short                          UINT16;
typedef short                                   INT16;
typedef unsigned int                            UINT32;
typedef int                                     INT32;
typedef unsigned long                           UINT64;
typedef long                                    INT64;
typedef double                                  F64;
typedef float                                   F32;
typedef void *                                  VOID_PTR;
typedef void *                                  (*THREAD_FUNC)(void *arg);

#ifndef  NULL
#define  NULL                                   ((void*)0)
#endif

#ifndef false
#define false                                   (0)
#endif
#ifndef true
#define true                                    (!false)
#endif

/*==============================   j1939协议应用层消息类型   ====================================*/

/* GUI层(QT)可发送的消息类型 */
typedef enum _SENDDED_MSG_TYPE{
    tx_req_addr,
    tx_declare_addr,
    tx_unable_declare_addr,
    tx_req_engine_speed,
    tx_engine_speed
}SENDDED_MSG_TYPE;

/* J1939应用层消息 */
typedef struct _MSG{
    INT8  data_length;
    UINT8  dp;
    UINT8  pf;
    UINT8  ps;
    UINT8  p;
    UINT32 pgn;
    UINT8  data[MSG_MAX_LENGTH];
}MSG;

/* GUI层(QT)请求诊断时获取的消息结构 */
typedef struct _DTC{
    UINT32 spn;
    UINT8 fmi;
    UINT8 oc;
}DTC;

/* GUI层(QT)接收的消息结构 */
typedef struct _RECVED_MSG{
    F32 total_distance;             /* 1、里程             62977        0-3        0.1(km)/bit */
    F32 speed;                      /* 2、车速             62980        1-2        1/256(km/h) / bit */
    F32 roate_speed;                /* 3、转速             62980        5-6        0.125(rpm) / bit */
    INT32 water_temperature;        /* 4、水温             62980        3-4        1(℃) / bit */
    UINT8 oil_left;                 /* 5、剩余油量         62980        0          (百分数) / bit */
    F32 oil_valid;                  /* 6、即时燃油经济性   62978        3-4        1/512 (km/kg) /bit */
    F32 oil_used;                   /* 7、燃油消耗速度     62978        5-6        0.05(L/h) / bit */
    F32 total_oil_use;              /* 8、累计油耗         62981        0-3        0.5(L) / bit */
    INT32 oil_press;                /* 9、机油压力         62979        0-1        4(kpa) / bit */
}RECVED_MSG;

/* 链路层PDU */
typedef struct _PDU{
    UINT8                 p_r_dp;
    UINT8                 pf;
    UINT8                 ps;
    UINT8                 sa;
    UINT8                 data[8];
}PDU;

/* 物理层can包 */
typedef struct _CAN{
    UINT32        id;
    UINT8         dlc;
    UINT8         data[8];
}CAN;                             /* CAN包 */

#ifdef PP_DEBUG_QUEUE
typedef INT32   DATA_T;
#else
typedef CAN     DATA_T;
#endif

typedef struct _QUEUE{
    DATA_T buf[QUEUE_BUF_SIZE];
    INT32 size;
    INT32 head;
    INT32 tail;
    pthread_mutex_t mutex;
}QUEUE;

#endif /* _TYPEDEF_H_ */
