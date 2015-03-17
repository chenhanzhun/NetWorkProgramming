#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <signal.h>
#include <errno.h>

#define QLEN 1024
typedef void Sigfunc(int);

extern void err_sys(const char *, ...);
extern void err_quit(const char *, ...);
extern void str_echo(int);
static Sigfunc *MySignal(int signo, Sigfunc *func);
static Sigfunc *M_signal(int signo, Sigfunc *func);
static void sig_chld(int);

int main(int argc, char **argv)
{
    int sockfd, conndfd, size;
    socklen_t len;
    pid_t childpid;
    struct sockaddr_un cliaddr, servaddr;

    if(argc != 2)
        err_quit("usage: %s <pathname>", argv[0]);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;

    strcpy(servaddr.sun_path, argv[1]);

    /* 创建一个Unix域套接字 */
    if( (sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    /* 若路径名在文件系统已存在，则bind会出错；所以先调用unlink删除要绑定的路径名，防止bind出错 */
    unlink(argv[1]);

    /* 将路径名bind绑定到该套接字上 */
    size = offsetof(struct sockaddr_un, sun_path) + strlen(servaddr.sun_path);
    if(bind(sockfd, (struct sockaddr *)&servaddr, size) < 0)
        err_sys("bind error");

    /* 监听套接字 */
    if(listen(sockfd, QLEN) < 0)
    {
        close(sockfd);
        err_sys("listen error");
    }

    /* 信号处理 */
    MySignal(SIGCHLD, sig_chld);

    for( ; ;)
    {
        len = sizeof(cliaddr);
        if( (conndfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len)) < 0)
        {
            if(errno == EINTR)
                continue;
            else
                err_sys("accept error");
        }
    }

    if( (childpid = fork()) == 0)
    {
        close(sockfd);
        str_echo(conndfd);
        exit(0);
    }
    close(conndfd);
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
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
