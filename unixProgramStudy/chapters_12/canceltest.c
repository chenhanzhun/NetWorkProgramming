#include "apue.h"
#include <pthread.h>

static void *fun1(void *arg);
static void *fun2(void *arg);

pthread_t tid1, tid2;
int err;

int main(void)
{
    err = pthread_create(&tid1, NULL, fun1, NULL);
    if(err != 0)
        err_quit("can't create thread: %s\n", strerror(err));

    err = pthread_create(&tid2, NULL, fun2, NULL);
    if(err != 0)
        err_quit("can't create thread: %s\n", strerror(err));
    err = pthread_detach(tid1);
    if(err != 0)
        err_quit("detach error: %s\n", strerror(err));
    err = pthread_detach(tid2);
    if(err != 0)
        err_quit("detach error: %s\n", strerror(err));
    exit(0);
}
static void *fun1(void *arg)
{
    err = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if(err != 0)
        err_quit("set state error: %s\n", strerror(err));
    printf("thread 1 starting...\n");
    sleep(15);
    printf("thread 1 returnting...\n");

    err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if(err != 0)
        err_quit("set state error: %s\n", strerror(err));
    printf("thread 1.2 starting...\n");
    pthread_testcancel();
    printf("thread 1.2 returnting...\n");
    pthread_exit((void*)0);
}
static void *fun2(void *arg)
{
    printf("thread 2 starting...\n");
    err = pthread_cancel(tid1);
    if(err != 0)
        err_quit("can't cancel thread 1: %s\n", strerror(err));
    printf("thread 2 returnting...\n");
    pthread_exit((void*)0);
}
