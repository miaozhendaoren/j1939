#include "test_lib.h"

extern INT32 mk_threads(
        const THREAD_FUNC *threads,
        pthread_t *threads_id,
        const void **threads_para,
        INT32 thread_nums)
{
    INT32 i = 0;
    INT32 err = 0;

    assert(NULL != threads);
    assert(NULL != threads_id);
    assert(NULL != threads_para);

    for(i=0;i<thread_nums;i++)
    {
        assert(NULL != threads[i]);

        err = pthread_create(
                &threads_id[i],
                NULL, 
                threads[i],
                (void *)(threads_para[i]));

        if(0 != err)
        {
            printf("%s,%d:can't create thread: %s\n", __FILE__, __LINE__, strerror(errno));
            return errno;
        } 
    }
    return 0;
}

