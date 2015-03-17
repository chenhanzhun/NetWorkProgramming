#include "apue.h"
#include <pthread.h>
#include <signal.h>

pthread_mutex_t     lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(void)
{
    printf("preparing locks...\n");
    pthread_mutex_lock(&lock1);
    pthread_mutex_lock(&lock2);
}
void parent(void)
{
    printf("parent unlocking locks...\n");
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
}
void child(void)
{
    printf("child unlocking locks...\n");
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
}
void* thread_func(void *arg)
{
    printf("thread started...\n");
    pause();
    return 0;
}
int main(void)
{
    pid_t       pid;
    pthread_t   tid;
    int err;

    err = pthread_atfork(prepare,parent,child);
    if(err != 0)
        err_exit(err, "can't install fork handlers");
    err = pthread_create(&tid,NULL,thread_func,NULL);
    if(err != 0)
        err_exit(err, "can't create thread");
    sleep(2);
    printf("parent about to fork.\n");
    pid = fork();
    if(pid == -1)
        err_quit("fork failed: %s\n", strerror(err));
    if(pid == 0)
        printf("child returned from fork.\n");
    else
        printf("parent returned form fork.\n");
    exit(0);
}
