#ifndef _AL_H_
#define _AL_H_

/*
 * 实现协议         PGN          byte       精度
1、里程             62977        0-3        0.1(km)/bit
2、车速             62980        1-2        1/256(km/h) / bit
3、转速             62980        5-6        0.125(rpm) / bit
4、水温             62980        3-4        1(℃) / bit

5、剩余油量         62980        0          (百分数) / bit
6、即时燃油经济性   62978        3-4        1/512 (km/kg) /bit
7、燃油消耗速度     62978        5-6        0.05(L/h) / bit
8、累计油耗         62981        0-3        0.5(L) / bit

9、机油压力         62979        0-1        4(kpa) / bit
*/

#include "j1939_internal.h"

extern void al_init(void);
extern INT32 get_need_can_nums(INT32 data_length);

extern void print_msg(const MSG *ptr_msg);

#endif /* _AL_H_ */
