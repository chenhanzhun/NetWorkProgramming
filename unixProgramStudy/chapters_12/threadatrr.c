#include <apue.h>
#include <pthread.h>

static int makethread(void *(*fn)(void *), void *arg);
static void *fun(void *arg);

int main(void)
{
    int err;

    err = makethread(fun, NULL);
    if(err != 0)
        err_quit("can't create thread: %s\n", strerror(err));
    sleep(3);
}

static int makethread(void *(*fn)(void *), void *arg)
{
    pthread_t tid;
    pthread_attr_t attr;
    int err;

    err = pthread_attr_init(&attr);
    if(err != 0)
        return(err);
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//创建以分离状态启动的线程
    if(0 == err)
        err = pthread_create(&tid, &attr, fn, arg);
    pthread_attr_destroy(&attr);
    return(err);
}

static void *fun(void *arg)
{
    pthread_t tid;
    pthread_attr_t attr;
    size_t size;
    int state;
    void *addr;

    tid = pthread_self();
    pthread_getattr_np(tid, &attr);
    pthread_attr_getdetachstate(&attr, &state);
    printf("detachstate = ");
    if(state == PTHREAD_CREATE_DETACHED)
        printf("PTHREAD_CREATE_DETACHED\n");
    else if(state == PTHREAD_CREATE_JOINABLE)
        printf("PTHREAD_CREATE_JOINABLE\n");

    pthread_attr_getstack(&attr, &addr, &size);
    printf("stack address = %p\n", addr);
    printf("stack size = 0x%x bytes\n", size);
    pthread_attr_getguardsize(&attr, &size);
    printf("guard size = %d bytes\n",size);

    return((void *)0);
}
