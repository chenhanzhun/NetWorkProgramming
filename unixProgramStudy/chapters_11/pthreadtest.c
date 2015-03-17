#include "apue.h"
#include <pthread.h>

pthread_t ntid;

static void printids(const char *);
static void *func(void*);

int main(void)
{
    int err;

    err = pthread_create(&ntid, NULL, func, NULL);
    if(err != 0)
        err_quit("can't create thread: %s\n",strerror(err));
    printids("main thread: ");
    sleep(1);
    exit(0);
}

static void printids(const char *s)
{
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = pthread_self();

    printf("%s pid %u tid %u (0x%x)\n",s,(unsigned int)pid,(unsigned int)tid,(unsigned int)tid);
}

static void *func(void *arg)
{
    printids("new thread: ");
    return((void*)0);
}
