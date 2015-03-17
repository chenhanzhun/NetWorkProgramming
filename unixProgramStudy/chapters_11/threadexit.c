#include "apue.h"
#include <pthread.h>

static void *func1(void *arg);
static void *func2(void *arg);

int main(void)
{
    pthread_t tid1, tid2;
    int err;
    void *tret;

    err = pthread_create(&tid1, NULL, func1, NULL);
    if(err != 0)
        err_quit("can't create thread 1: %s\n", strerror(err));
    err = pthread_create(&tid2, NULL, func2, NULL);
    if(err != 0)
        err_quit("can't create thread 2: %s\n", strerror(err));

    err = pthread_join(tid1,&tret);
    if(err != 0)
        err_quit("can't join with thread 1: %s\n", strerror(err));
    printf("thread 1 exit code %d\n", (int)tret);

    err = pthread_join(tid2,&tret);
    if(err != 0)
        err_quit("can't join with thread 2: %s\n", strerror(err));
    printf("thread 2 exit code %d\n", (int)tret);

    exit(0);
}

static void *func1(void *arg)
{
    printf("thread 1 returning\n");
    return((void *)1);
}

static void *func2(void *arg)
{
    printf("thread 2 exiting\n");
    pthread_exit((void*)2);
}
