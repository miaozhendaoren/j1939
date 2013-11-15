#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "j1939_internal.h"

extern void queue_init(QUEUE *pq);
extern void enqueue(QUEUE *pq, const DATA_T *pd);
extern void dequeue(DATA_T *pd, QUEUE *pq);

#endif /* _QUEUE_H_ */
