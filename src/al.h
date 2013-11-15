#ifndef _AL_H_
#define _AL_H_

#include "j1939_internal.h"

extern void al_init(void);
extern INT32 get_need_can_nums(INT32 data_length);

extern void print_msg(const MSG *ptr_msg);

#endif /* _AL_H_ */
