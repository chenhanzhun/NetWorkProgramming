#include <sys/types.h>
#include "apue.h"
#include <signal.h>

static Sigfunc *Msignal(int signo, Sigfunc *func);
static void sig_func(int signo);

int main(void)
{
    if(Msignal(SIGALRM,sig_func) == SIG_ERR)
        err_sys("SIGALRM error");
    if(Msignal(SIGHUP,sig_func) == SIG_ERR)
        err_sys("SIGHUP error");
    printf("kill...\n");
    kill(getpid(),SIGHUP);
    printf("alarm...\n");
    alarm(5);
    pause();
    printf("exit.\n");
    exit(0);
}

static void sig_func(int signo)
{
    if(SIGHUP == signo)
        printf("Recevied kill.\n");
    else if(SIGALRM == signo)
        printf("Recevied alarm.\n");
    else
        printf("Recevied others.\n");
}

static Sigfunc *Msignal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;

    /* 设置信号处理函数 */
    act.sa_handler = func;
    /* 初始化信号集 */
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(signo == SIGALRM)
    {/* 若是SIGALRM信号，则系统不会自动重启 */
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    }
    else
    {/* 其余信号设置为系统会自动重启 */
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }
    /* 调用 sigaction 函数 */
    if(sigaction(signo, &act, &oact) < 0)
        return(SIG_ERR);
    return(oact.sa_handler);
}
