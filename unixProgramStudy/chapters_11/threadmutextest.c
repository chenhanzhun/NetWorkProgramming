#include "threadmutex.h"
#include "apue.h"
#include <pthread.h>
#include <sys/types.h>

static void *thread_fun1(void *arg);
static void *thread_fun2(void *arg);

int main(void)
{
    pthread_t tid1, tid2;
    int err;
    void *ret;
    struct foo *obj;
    obj = foo_alloc();

    err = pthread_create(&tid1, NULL, thread_fun1, (void*)obj);
    if(err != 0)
        err_quit("pthread_create error: %s\n", strerror(err));

    err = pthread_create(&tid2, NULL, thread_fun2, (void*)obj);
    if(err != 0)
        err_quit("pthread_create error: %s\n", strerror(err));

    pthread_join(tid1, &ret);
    printf("thread 1 exit code is: %d\n", (int)ret);
    pthread_join(tid2, &ret);
    printf("thread 2 exit code is: %d\n", (int)ret);

    exit(0);
}

static void *thread_fun1(void *arg)
{
    struct foo *fp = (struct foo *)arg;
    printf("thread 1 starting...\n");
    foo_release(fp);
    printf("thread 1 returning...\n");
    pthread_exit((void*)1);
}

static void *thread_fun2(void *arg)
{
    struct foo *fp = (struct foo *)arg;
    printf("thread 2 starting...\n");
    foo_hold(fp);
    foo_hold(fp);
    printf("thread 2 returning...\n");
    pthread_exit((void*)2);
}
