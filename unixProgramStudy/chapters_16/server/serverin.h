/* 服务器初始化套接字端点 */
#ifndef SERVERIN_H
#define SERVERIN_H
#include "apue.h"
#include <sys/socket.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

/* type 套接字类型, qlen是监听队列的最大个数 */
int initserver(int type, const struct sockaddr *addr, socklen_t len, int qlen)
{
    int fd;
    int err = 0;

    /* 采用type类型默认的协议 */
    if((fd = socket(addr->sa_family, type, 0)) < 0)
        return -1;/* 出错返回-1*/

    int reuse = 1;

    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
    {
        err = errno;
        goto errout;
    }
    /* 将地址绑定到一个套接字 */
    if(bind(fd, addr, len) < 0)
    {
        err =errno;
        goto errout;/* 跳转到出错输出语句 */
    }
    /* 若套接字类型type是面向连接(SOCK_STREAM, SOCK_SEQPACKET)的，则执行以下语句 */
    if(type == SOCK_STREAM || type == SOCK_SEQPACKET)
    {
        /* 监听套接字连接队列 */
        if(listen(fd, qlen) < 0)
        {
            err = errno;
            goto errout;
        }
    }
    return (fd);

errout:
    close(fd);
    errno = err;
    return -1;
}

/* 初始化守护进程 */

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    umask(0);

    if(getrlimit(RLIMIT_NOFILE, &rl) < 0)
        err_quit("%s: can't get file limit", cmd);

    if((pid =fork()) < 0)
        err_quit("%s: can't fork", cmd);
    else if(pid != 0)
        exit(0);
    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: can't ignore SIGHUP");
    if((pid =fork()) < 0)
        err_quit("%s: can't fork", cmd);
    else if(pid != 0)
        exit(0);
    if(chdir("/") < 0)
        err_quit("%s: can't change directory to /");

    if(rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for(i = 0; i < (int)rl.rlim_max; i++)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpectrd file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }

}
#endif
