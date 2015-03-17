#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

typedef void Sigfunc(int);

extern void err_sys(const char *,...);
static Sigfunc *M_signal(int signo, Sigfunc *func);
static Sigfunc *MySignal(int signo, Sigfunc *func);
static void connect_alarm(int);

int Myconnect_timo(int sockfd, const struct sockaddr *saptr, socklen_t salen, int nsec)
{
    Sigfunc *sigfunc;
    int n;

    /* SIGALRM 信号处理函数，并保存现有信号处理函数 */
    sigfunc = MySignal(SIGALRM, connect_alarm);

    /* 设置alarm超时 */
    if(alarm(nsec) != 0)/* 若已经设置了超时，则alarm返回的是当前剩余秒数，否则返回0 */
        printf("alarm was already set\n");/* 提示：已经设置过alarm超时 */

    if( (n = connect(sockfd, saptr, salen)) < 0)
    {/* 由超时处理函数调用中断导致连接失败，则关闭套接字，并设置是由超时导致的失败 */
        close(sockfd);
        if(errno == EINTR)
            errno = ETIMEDOUT;
    }
    /* 关闭 alarm */
    alarm(0);
    /* 恢复原来的处理函数 */
    MySignal(SIGALRM, sigfunc);

    return(n);
}

static void connect_alarm(int signo)
{
    printf("flag: %d\n", signo);
    return;/* just interrupt the connect */
}


static Sigfunc *MySignal(int signo, Sigfunc *func)
{
    Sigfunc *sigfunc;
    if( (sigfunc = M_signal(signo, func)) == SIG_ERR)
        err_sys("signal error");
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
