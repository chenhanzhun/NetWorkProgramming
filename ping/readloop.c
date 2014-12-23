#include "ping.h"

void readloop()
{
    int size;
    char recvbuf[BUFSIZE];
    char controlbuf[BUFSIZE];

    struct msghdr msg;
    struct iovec iov;

    ssize_t n;
    struct timeval tval;

    /* 创建ICMP的原始套接字，必须是root权限 */
    if( (sockfd = socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpprot)) < 0)
    {
        perror("socket error");
        exit(1);
    }
    /* 回收root权限，设置当前用户权限 */
    setuid(getuid());
    /* 初始化IPv6 */
    if(pr->finit)
        (*pr->finit)();

    size = 60 * 1024;
    /* 设置接收缓冲区的大小为60k，主要为了减小接收缓冲区溢出 */
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    /* 发送第一个数据包 */
    sig_alrm(SIGALRM);

    /* 初始化接收缓冲区 */
    iov.iov_base = recvbuf;
    iov.iov_len = sizeof(recvbuf);
    msg.msg_name = pr->sarecv;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = controlbuf;

    for( ; ;)
    {
        /* 接收ICMP数据包 */
        msg.msg_namelen = pr->salen;
        msg.msg_controllen = sizeof(controlbuf);
        /* 从套接字接收数据 */
        n = recvmsg(sockfd, &msg, 0);
        if(n < 0)
        {
            if(errno == EINTR)
                continue;
            else
            {
                perror("recvmsg error");
                exit(1);
            }
        }
        /* 记录接收时间 */
        gettimeofday(&tval, NULL);
        /* 调用处理函数 */
        (*pr->fproc)(recvbuf, n, &msg, &tval);
    }
}
