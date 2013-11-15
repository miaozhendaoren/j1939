#include "queue.h"

static BOOL is_full(const QUEUE *pq);
static BOOL is_empty(const QUEUE *pq);
static INT32 get_size(const QUEUE *pq);
static INT32 add_index(INT32 index);

#ifdef PP_DEBUG_QUEUE
static void * enqueue_pthread(void *arg);
static void * dequeue_pthread(void *arg);
static void print_queue(const QUEUE *pq);
static pthread_mutex_t g_mutex;
#endif

extern void queue_init(QUEUE *pq)
{
    assert(QUEUE_BUF_SIZE > 0);
    INT32 rst = 0;

    pq->size = 0;
    pq->head = 0;
    pq->tail = 0;

    rst = pthread_mutex_init(&pq->mutex, NULL);
    assert(0==rst);

    return;
}

extern void enqueue(QUEUE *pq, const DATA_T *pd)
{
    INT32 rst = 0;
    DATA_T *buf = NULL;

    while(is_full(pq))
    {
        usleep(QUEUE_SLEEP_TIME);
    }

    buf = &(pq->buf[pq->tail]);


    rst = pthread_mutex_lock(&pq->mutex);
    assert(0 == rst);

    memmove(buf, pd, sizeof(DATA_T));

    pq->tail = add_index(pq->tail);
    pq->size += 1;

    rst = pthread_mutex_unlock(&pq->mutex);
    assert(0 == rst);

    return;
}

extern void dequeue(DATA_T *pd, QUEUE *pq)
{
    INT32 rst = 0;
    DATA_T *buf = NULL;

    while(is_empty(pq))
    {
        usleep(QUEUE_SLEEP_TIME);
    }

    buf = &(pq->buf[pq->head]);

    rst = pthread_mutex_lock(&pq->mutex);
    assert(0 == rst);
    memmove(pd, buf, sizeof(DATA_T));

    pq->head = add_index(pq->head);
    pq->size -= 1;

    rst = pthread_mutex_unlock(&pq->mutex);
    assert(0 == rst);

    return;
}

static BOOL is_full(const QUEUE *pq)
{
    if(QUEUE_BUF_SIZE == get_size(pq))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static BOOL is_empty(const QUEUE *pq)
{
    if(0 == get_size(pq))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static INT32 get_size(const QUEUE *pq)
{
    return pq->size;
}

static INT32 add_index(INT32 index)
{
    return (index+1) % QUEUE_BUF_SIZE;
}

#ifdef PP_DEBUG_QUEUE
extern INT32 main(void)
{
    INT32 rst = 0;
    QUEUE q;

    pthread_t pid_enqueue;
    pthread_t pid_dequeue;

    queue_init(&q);
    assert(is_empty(&q));

    rst = pthread_mutex_init(&g_mutex, NULL);
    assert(0 == rst);

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    pthread_create(&pid_enqueue, NULL, enqueue_pthread, &q);
    pthread_create(&pid_dequeue, NULL, dequeue_pthread, &q);

    while(true)
    {
        sleep(10000);
    }

    return 0;
}

static void * enqueue_pthread(void * arg)
{
    INT32 rst = 0;
    INT32 i = 0;
    INT32 sleep_us = 0;
    DATA_T data = 0;
    QUEUE *pq = (QUEUE *)arg;

    while(true)
    {
        sleep_us = 10*1.0 * rand() /RAND_MAX;
        usleep(sleep_us);

        data = i;
        enqueue(pq, &data);
        
        rst = pthread_mutex_lock(&g_mutex);
        assert(0 == rst);

        printf("----------------- begin enqueue ---------------------\n");
        printf("%s,%d: sleep_us=%d, %dtimes enqueue: %d, queue size: %d.\n",
                __FILE__, __LINE__, sleep_us, i, data, get_size(pq));
        print_queue(pq);
        printf("----------------- end   enqueue ---------------------\n");

        rst = pthread_mutex_unlock(&g_mutex);
        assert(0 == rst);

        i++;
    }

    return NULL;
}

static void * dequeue_pthread(void *arg)
{
    INT32 rst = 0;
    INT32 i = 0;
    INT32 sleep_us = 0;
    DATA_T data = 0;
    QUEUE *pq = (QUEUE *)arg;

    while(true)
    {
        sleep_us = 1*1.0 * rand() /RAND_MAX;
        usleep(sleep_us);

        dequeue(&data, pq); 
        
        rst = pthread_mutex_lock(&g_mutex);
        assert(0 == rst);

        printf("----------------- begin dequeue ---------------------\n");
        printf("%s,%d: sleep_us: %d, %dtimes dequeue: %d, queue size: %d.\n",
                __FILE__, __LINE__, sleep_us, i, data, get_size(pq));
        print_queue(pq);
        printf("----------------- end   dequeue ---------------------\n"); 
        
        rst = pthread_mutex_unlock(&g_mutex);
        assert(0 == rst);

        i++;
    }

    return NULL;
}

static void print_queue(const QUEUE *pq)
{
    INT32       i = 0;
    INT32       index = 0;

    if(is_empty(pq))
    {
        printf("%s,%d: queue(%p) is empty.\n", __FILE__, __LINE__, (void *)pq);
        return;
    }
    
    index = pq->head;
    do{
        printf("q[%d]=%d,", i++, pq->buf[index]);
        index = add_index(index);
    }while(index != pq->tail);
    
    printf("\n");

    return;
}
#endif

