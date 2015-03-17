#include <signal.h>
/*
int sigsuspend(const sigset_t *sigmask);//返回值：-1，并将errno设置为EINTR；
*/
#include "apue.h"
#include "pr_mask.h"

static void sig_func(int signo);

int main(void)
{
    sigset_t newmask, oldmask, waitmask;

    pr_mask("Program start: ");

    if(signal(SIGINT,sig_func) == SIG_ERR)
        err_sys("signal error");
    sigemptyset(&newmask);//初始化信号集
    sigaddset(&newmask,SIGINT);//添加SIGINT信号

    sigemptyset(&waitmask);
    sigaddset(&waitmask,SIGUSR1);

    //屏蔽信号
    /*
     * Block SIGINT and save current signal mask
     */
    if(sigprocmask(SIG_BLOCK,&newmask,&oldmask) < 0)
        err_sys("SIG_BLOCK error");

    pr_mask("in critical region: ");

    //临时修改进程信号屏蔽字，在捕捉信号之前，将进程挂起等待
    /*
     * pause, allowing all of signals except SIGUSR1
     */
    if(sigsuspend(&waitmask) != -1)
        err_sys("sigsuspend error");
    pr_mask("after return from sigsuspend: ");
    /*
     * reset signal mask which unblocks SIGINT
     */
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL) < 0)
        err_sys("SIG_SETMASK error");
    pr_mask("program exit: ");

    exit(0);
}

static void sig_func(int signo)
{
    pr_mask("\nin sig_func: ");
}
