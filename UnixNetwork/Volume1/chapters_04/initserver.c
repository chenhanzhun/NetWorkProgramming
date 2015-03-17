/* 服务器初始化套接字端点 */
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

/* 函数功能：初始化服务器套接字；
 * 返回值：若成功则返回监听套接字，若出错返回-1并设置errno值；
 */

/* type 套接字类型, qlen是监听队列的最大个数 */
int initserver(int type, struct sockaddr *servaddr, socklen_t len, int qlen)
{
    int fd;
    int err = 0;

    /* 采用type类型默认的协议 */
    if((fd = socket(servaddr->sa_family, type, 0)) < 0)
        return -1;/* 出错返回-1*/

    int reuse = 1;

    /* 设置套接字选项 */
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
    {
        err = errno;
        goto errout;
    }
    /* 将地址绑定到一个套接字 */
    if(bind(fd, servaddr, len) < 0)
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
