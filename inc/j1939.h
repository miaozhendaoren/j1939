#ifndef _J1939_H_
#define _j1939_h_

#include "config.h"
#include "typedef.h"

/* PGN定义 */
#define REQUEST_PGN             (59904)
#define DECLARE_ADDR_PGN        (60928)
#define EEC1_PGN                (61444)
#define TP_PGN                  (60416)

/* ECU地址 */
#define PLATFORM_ADDR           (28)

/*================================= j1939协议栈接口函数 ====================================*/
extern void j1939_init(void);

extern void push_msg_to_stack(SENDDED_MSG_TYPE type);
extern void pull_msg_from_stack(RECVED_MSG *ptr_msg);

extern void enqueue_can_to_rx_fifo(const CAN *ptr_can);
extern void dequeue_can_from_tx_fifo(CAN *ptr_can);

/* 调试辅助函数 */
extern void print_recved_msg(const RECVED_MSG *ptr_recved_msg);
extern INT8 *get_sendded_msg(const SENDDED_MSG_TYPE sended_msg);

#endif /* _j1939_h_ */

