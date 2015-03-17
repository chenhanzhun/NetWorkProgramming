#include "apue.h"
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

static void sig_quit(int signo);

int main()
{
    sigset_t    newmask,oldmask,pendmask;
    if(signal(SIGQUIT,sig_quit) == SIG_ERR)
    {
        err_sys("signal() error");
        exit(-1);
    }
    //初始化信号集
    sigemptyset(&newmask);
    //添加一个SIGQUIT信号
    sigaddset(&newmask,SIGQUIT);
    //将newmask信号集设置为阻塞，原信号集保存在oldmask中
    if(sigprocmask(SIG_BLOCK,&newmask,&oldmask) == -1)
    {
        err_sys("SIG_BLOCK error");
        exit(-1);
    }
    sleep(5);
    //获取阻塞的信号集
    if(sigpending(&pendmask) == -1)
    {
        err_sys("sigpending() error");
        exit(-1);
    }
    //判断SIGQUIT是否是阻塞的
    if(sigismember(&pendmask,SIGQUIT))
        printf("\nSIGQUIT is pending.\n");
    //恢复原始的信号集
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL) == -1)
    {
        err_sys("SIG_SETMASK error");
        exit(-1);
    }
    printf("SITQUIT unblocked\n");
    sleep(5);
    exit(0);
}

static void sig_quit(int signo)
{
    printf("caught SIGQUIT.\n");
    if(signal(SIGQUIT,SIG_DFL) == SIG_ERR)
    {
        err_sys("can't reset SIGQUIT");
        exit(-1);
    }
}
