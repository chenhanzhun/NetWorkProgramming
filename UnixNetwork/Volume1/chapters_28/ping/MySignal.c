#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
typedef void Sigfunc(int);

static Sigfunc *M_signal(int signo, Sigfunc *func);
Sigfunc *MySignal(int signo, Sigfunc *func)
{
    Sigfunc *sigfunc;
    if( (sigfunc = M_signal(signo, func)) == SIG_ERR)
    {
        perror("signal error");
        exit(1);
    }
    return (sigfunc);
}
static Sigfunc *M_signal(int signo, Sigfunc *func)
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
