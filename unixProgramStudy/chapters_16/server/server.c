/* 基于TCP的面向连接的服务器进程 */

#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "serverin.h"

/* 定义等待请求队列的最大数 */
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

/* 外部定义的服务器初始化套接字端点 */
//extern int initserver(int type, const struct sockaddr *addr, socklen_t len, int qlen);

/* 服务器接收连接请求，并向请求客户端发送数据 */
void server(int sockfd);

int main(int argc, char *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;

    int sockfd, err, n;
    char *host;

    if(argc != 1)
        err_quit("usage: %s", argv[0]);
#ifdef _SC_HOST_NAME_MAX
    n = sysconf(_SC_HOST_NAME_MAX);
    if(n < 0) /* best guess */
#endif
        n = HOST_NAME_MAX;
    host = (char *)malloc(n);
    if(NULL == host)
        err_sys("malloc error");
    /* 获取客户端的主机名 */
    if(gethostname(host, n) < 0)
        err_sys("gethostname error");
    daemonize("ruptimed");
    /* 初始化addrinfo结构变量hint */
    hint.ai_flags = AI_CANONNAME;/* 需要一个规范名 */
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_STREAM;/* 有序、可靠、双向的面向连接字节流 */
    hint.ai_protocol = 0;/* 使用默认协议 */
    hint.ai_addrlen = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;/* 表示只有一个addrinfo链表结构 */

    /* 将主机名和服务名映射到一个地址 */
    if((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0)
    {
        syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s", gai_strerror(err));
        exit(1);
    }
    /* 对每个结构addrinfo链表进行以下操作，其实这里只有一个addrinfo结构 */
    for(aip = ailist; aip != NULL; aip = aip->ai_next)
    {
        /* 其中这里才是服务进程套接字编程 */

        if((sockfd = initserver(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0)
        {
            server(sockfd);
            exit(0);
        }
    }
    exit(1);
}
void server(int sockfd)
{
    int clfd, status;
    pid_t pid;

    while(1)
    {
        clfd = accept(sockfd, NULL, NULL);
        if(clfd < 0)
        {
            syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
            exit(1);
        }

        if((pid = fork()) < 0)
        {
            syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
            exit(1);
        }
        else if(pid == 0)
        {
            if(dup2(clfd, STDOUT_FILENO) != STDOUT_FILENO || dup2(clfd, STDERR_FILENO) != STDERR_FILENO)
            {
                syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
                exit(1);
            }
            close(clfd);
            execl("/usr/bin/uptime", "uptime", (char *)0);/* 执行 uptime指令 */
            syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
        }
        else
        {
            close(clfd);
            waitpid(pid, &status, 0);
        }
    }
}
