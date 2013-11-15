#ifndef _DL_H_
#define _DL_H_

#include "j1939_internal.h"

extern void dl_init(void);
extern void build_pdu(PDU *ptr_pdu, INT32 *ptr_length, const MSG *ptr_msg);
extern void parse_pdu(MSG *ptr_msg, const PDU *ptr_pdu, INT32 length);

extern BOOL need_follow_pdu(const PDU *ptr_pdu);

extern void print_pdu(const PDU *ptr_pdu);
#endif /* _DL_H_ */
