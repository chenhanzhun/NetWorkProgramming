#include <pthread.h>
#include <sys/types.h>
#include "apue.h"

static void cleanup(void *arg);  //线程清理函数
void *func1(void *arg);
void *func2(void *arg);

int main(void)
{
    pthread_t tid1;
    pthread_t tid2;
    int       err;
    void   *tret;

    err = pthread_create(&tid1, NULL, func1, (void*)1);
    if(err != 0)
        err_quit("can't create thread 1: %s\n", strerror(err));
    err = pthread_create(&tid2, NULL, func2, (void*)1);
    if(err != 0)
        err_quit("can't create thread 2: %s\n", strerror(err));

    err = pthread_join(tid1,&tret);
    if(err != 0)
        err_quit("can't join with thread 1: %s\n", strerror(err));
    printf("thread 1 exit code %d\n", (int)tret);
    err = pthread_join(tid2, &tret);
    if(err != 0)
        err_quit("can't join with thread 2: %s\n", strerror(err));
    printf("thread 2 exit code %d\n",(int)tret);
    exit(0);
}

static void cleanup(void *arg)
{
    printf("cleanup: %s\n", (char*)arg);
}
void *func1(void *arg)
{
   printf("thread 1 start.\n");
   pthread_cleanup_push(cleanup,"thread 1 first handler");
   pthread_cleanup_push(cleanup,"thread 1 second handler");
   printf("thread 1 push complete.\n");
   if(arg)
    return ((void*)1);   //返回终止，将不会调用清理处理程序
   pthread_cleanup_pop(0);
   pthread_cleanup_pop(0);
   return ((void*)1);
}
void *func2(void *arg)
{
   printf("thread 2 start.\n");
   pthread_cleanup_push(cleanup,"thread 2 first handler");
   pthread_cleanup_push(cleanup,"thread 2 second handler");
   printf("thread 2 push complete.\n");
   if(arg)
     pthread_exit((void*)2); //会调用清理处理程序
   pthread_cleanup_pop(0);
   pthread_cleanup_pop(0);
   pthread_exit((void*)2);
}
