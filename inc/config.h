#ifndef _CONFIG_H_
#define _CONFIG_H_

/******************************************************************************
 * 调试级别:
 *
 * bit0：是否记录到log文件
 *      0 不记录
 *      1 记录
 *
 * bit1: 测试框架是否输出调试信息
 *      0 不输出
 *      1 输出
 *
 * bit2: 协议接口是否输出调试信息
 *      0 不输出
 *      1 输出
 *
 * bit3: 协议内部函数是否输出调试信息
 *      0 不输出
 *      1 输出
 *
 * bit4: 协议中间数据是否输出
 *      0 不输出
 *      1 输出
 *
 *****************************************************************************/
#define DEBUG_NONE              (0x00)
#define DEBUG_TEST              (0x02)
#define DEBUG_DETAIL            (0x04)

//#define PP_DEBUG                (DEBUG_DETAIL | DEBUG_TEST)
//#define PP_DEBUG                (DEBUG_TEST)
#define PP_DEBUG                (DEBUG_NONE)

#ifndef PP_DEBUG
#define PP_DEBUG_QUEUE
#endif

#define DEBUG_LOG_NAME          ("log.debug")

/* 调试输出一行的字节数 */
#define FORMMAT_SIZE            (500)
#define LINE_SIZE               (50)

/* 队列用环形缓冲实现，该宏定义环形缓冲大小 */
#define QUEUE_BUF_SIZE          (1)

/* 缓冲满后的延时(us) */
#define QUEUE_SLEEP_TIME        (10*1000U)
#define FIFO_MAX_FULL_TIMES     (10)

/* J1939应用层接收消息的最大长度(包数 *8字节),详细定义参见J1939协议第五部分 */
#define MAX_MUL_CAN_NUMS        (28)
/* J1939应用层发送消息的最大长度,详细定义参见J1939协议第五部分 */
#define MSG_MAX_LENGTH          (MAX_MUL_CAN_NUMS*8)

/* 退出前休眠时间 */
#define SLEEP_ITME_BEFORE_EXIT  (1)

#endif /* _CONFIG_H_ */

