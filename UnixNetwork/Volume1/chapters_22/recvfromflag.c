/* 函数功能：类似于recvfrom函数；
 * 返回值：
 * 1、返回msg_flags值；
 * 2、返回所接收数据报的目的地址（由IP_RECVDSTADDR套接字选项获取）；
 * 3、返回所接收数据报接口的索引（由IP_RECVIF套接字选项获取）；
 */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

/* 自定义结构体，成员包含目的地址、接口索引 */
struct unp_in_pktinfo{
    struct in_addr  ipi_addr;   /* destination IPv4 address */
    int             ipi_ifindex;/* received interface index */
};

ssize_t recvfrom_flag(int fd, void *ptr, size_t nbytes, int *flags,
        struct sockaddr * sa, socklen_t *salenptr, struct unp_in_pktinfo *pktp)
{
    struct msghdr msg;/* 需要调用recvmsg函数，所以必须定义该结构 */
    struct iovec iov[1];/* 非连续缓冲区，在这里只定义一个缓冲区 */
    ssize_t n;

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    /* 若支持msg_control成员则初始化以下值辅助数据 */
    struct cmsghdr *cmptr;
    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(struct in_addr)) +
            CMSG_SPACE(sizeof(struct unp_in_pktinfo))];
    } control_un;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_flags = 0;
#else
    /* 若不支持msg_control控制信息，则直接初始化为0 */
    bzero(&msg, sizeof(msg));
#endif
    /* 赋值初始化msghdr结构 */
    msg.msg_name = sa;
    msg.msg_namelen = *salenptr;
    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if( (n = recvmsg(fd, &msg, *flags)) < 0)
        return(n);/* 出错返回 */
    /* 若recvmsg调用成功返回，则执行以下程序 */
    *salenptr = msg.msg_namelen;/* 值-结果参数必须返回 */
    if(pktp)
        /* 初始化unp_in_pktinfo结构，置地址为0.0.0.0，置接口索引为0 */
        bzero(pktp, sizeof(struct unp_in_pktinfo));/* 0.0.0.0, i/f = 0 */
#ifndef HAVE_MSGHDR_MSG_CONTROL
    /* 若不支持msg_control控制信息，则把待返回标志置为0，并返回 */
    *flags = 0;/* 值-结果参数返回 */
    return(n);
    /* 以下程序都是处理支持msg_control控制信息的部分 */
#else
    /* 返回标志信息 */
    *flags = msg.msg_flags;/* 值-结果参数返回 */
    if(msg.msg_controllen < sizeof(struct cmsghdr) ||
            (msg.msg_flags & MSG_CTRUNC) || pktp == NULL)
        return(n);
    /* 处理辅助数据 */
    for(cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL; cmptr = CMSG_NXTHDR(&msg, cmptr))
    {
#ifdef IP_RECVDSTADDR
        /* 处理IP_RECVDSTADDR，返回接收数据报的目的地址 */
        /* 其中IPPROTO_IP表示IPv4域 */
        if(cmptr->cmsg_level == IPPROTO_IP &&
                cmptr->cmsg_type == IP_RECVDSTADDR)
        {
            memcpy(&pktp->ipi_addr, CMSG_DATA(cmptr), sizeof(struct in_addr));
            continue;
        }
#endif
#ifdef IP_RECVIF
        /* 处理IP_RECVIF，返回接收数据报的接口索引 */
        if(cmptr->cmsg_level == IPPROTO_IP &&
                cmptr->cmsg_type == IP_RECVIF)
        {
            struct sockaddr_dl *sdl;/* 数据链路地址结构中包含有接口索引成员 */
            sdl = (struct sockaddr_dl *)CMSG_DATA(cmptr);
            pktp->ipi_ifindex = sdl->sdl_index;
            continue;
        }
#endif
        err_quit("unknown ancillary data");
    }
    return(n);
#endif
}
