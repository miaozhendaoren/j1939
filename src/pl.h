#ifndef _PL_H_
#define _PL_H_

#include "j1939_internal.h"

extern void pl_init(void);

extern void build_and_enqueue_cans(const PDU *ptr_pdu, INT32 length);
extern void dequeue_and_parse_cans(PDU *ptr_pdu, INT32 *ptr_length);

extern void print_can(const CAN *ptr_can);

#endif /* _PL_H_ */
