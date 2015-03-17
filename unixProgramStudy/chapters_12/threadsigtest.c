#include "apue.h"
#include <pthread.h>
#include <signal.h>

int quitflags;
sigset_t mask;

//初始化互斥量、条件变量
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait = PTHREAD_COND_INITIALIZER;

void *thr_fun(void *arg);

int main(void)
{
    int err;
    sigset_t oldmask;
    pthread_t tid;

    //初始化信号集，添加两个信号SIGINT、SIGQUIT
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    //在主线程设置信号屏蔽字，使得所以线程都阻塞信号集的信号
    err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
    if(err != 0)
        err_quit("SIG_BLOCK error: %s\n", strerror(err));
    //创建新的线程
    err = pthread_create(&tid, NULL, thr_fun, 0);
    if(err != 0)
        err_quit("can't create thread: %s\n", strerror(err));

    //对主线程进行加锁
    pthread_mutex_lock(&lock);
    //等待条件变量为真
    while(quitflags == 0)
        pthread_cond_wait(&wait, &lock);
    //对主线程解锁操作
    pthread_mutex_unlock(&lock);

    quitflags = 0;

    //打开信号屏蔽字
    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
    exit(0);
}

void *thr_fun(void *arg)
{
    int err, signo;

    for(; ;)
    {
        //在新建线程中等待信号发生
        err = sigwait(&mask, &signo);
        if(err != 0)
            err_quit("sigwait error: %s\n", strerror(err));

        switch(signo)
        {
            case SIGINT:
                printf("\ninterrupt\n");
                break;
            case SIGQUIT:
                pthread_mutex_lock(&lock);
                quitflags = 1;
                pthread_mutex_unlock(&lock);
                pthread_cond_signal(&wait);
                return(0);
            default:
                printf("unexpected signal %d\n", signo);
                exit(1);
        }
    }
}
