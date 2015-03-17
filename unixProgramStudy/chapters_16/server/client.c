/* 基于TCP协议面向连接的客户端进程 */

#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include "connectre.h"

/* 地址的最长大小 */
#define MAXADDRLEN 256
/* 缓冲区大小 */
#define BUFLEN 128

/* 外部函数定义，该函数功能是实现套接字连接 */
//extern int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t len);

/* 打印客户端所接收的数据信息 */
void print_uptime(int sockfd);

int main(int argc, char *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;

    int sockfd, err;
    if(argc != 2)
        err_quit("usage: %s hostname", argv[0]);
    /* 初始化addrinfo结构变量hint */
    hint.ai_flags = 0;
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_STREAM;/* 有序、可靠、双向的面向连接字节流 */
    hint.ai_protocol = 0;/* 使用默认协议 */
    hint.ai_addrlen = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;/* 表示只有一个addrinfo链表结构 */

    /* 将主机名和服务名映射到一个地址 */
    if((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(err));
    /* 对每个结构addrinfo链表进行以下操作，其实这里只有一个addrinfo结构 */
    for(aip = ailist; aip != NULL; aip = aip->ai_next)
    {
        /* 其中这里才是客户端套接字编程 */

        /* 步骤1：创建套接字描述符 */
        if((sockfd = socket(aip->ai_family, SOCK_STREAM, 0)) < 0)
            err =errno;
        /* 步骤2：connect请求连接 */
        if(connect_retry(sockfd, aip->ai_addr, aip->ai_addrlen) < 0)
            err = errno;/* 请求连接失败 */
        else /* 请求连接成功，则准备传输数据 */
        {
            /* 接收来自服务器进程的消息，并打印输出 */
            print_uptime(sockfd);
            exit(0);/* 正常退出 */
        }
    }
    /* 异常退出 */
    fprintf(stderr, "can't connect to %s: %s\n",argv[1], strerror(err));
    exit(1);
}

void print_uptime(int sockfd)
{
    int n;
    char buf[BUFLEN];

    /* 接收数据 */
    while((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
        /* 把接收到的数据输出到终端 */
        write(STDOUT_FILENO, buf, n);
    if(n < 0)
        err_sys("recv error");
}
