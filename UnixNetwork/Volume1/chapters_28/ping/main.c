#include "ping.h"


/* 初始化IPv4结构 */
struct proto proto_v4 = {proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP};

#ifdef IPV6
/* 若存在IPv6，则初始化IPv6结构 */
struct proto proto_v6 = {proc_v6, send_v6, init_v6, NULL, NULL, 0, IPPROTO_ICMPV6};
#endif
typedef void Sigfunc(int);
extern int datalen = 56;       /* data that goes with ICMP echo request */

extern Sigfunc *MySignal(int signo, Sigfunc *func);
extern struct addrinfo *host_serv(const char *host, const char *serv, int family, int socktype);
extern char *Sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
extern void *Calloc(size_t n, size_t size);
void statistics(int signo);

int main(int argc, char **argv)
{
    int n;
    struct addrinfo *ai;
    char *h;

    opterr = 0;     /* don't want getopt() writing to stderr */

    /* 只实现ping的一个参数选项-v供查询 */
    /* 有关getopt函数的使用可以查阅相关资料 */
    while( (n = getopt(argc, argv, "v")) != -1)
    {
        switch(n)
        {
            case 'v':
                verbose++;
                break;
            case '?':
                printf("unrecognize option: %c\n", n);
                exit(1);
        }
    }

    if(optind != argc-1)
    {
        perror("usage: ping [ -v ] <hostname>");
        exit(1);
    }

    host = argv[optind];

    pid = getpid() & 0xffff;    /* ICMP ID field is 16 bits */

    MySignal(SIGALRM, sig_alrm);
    MySignal(SIGINT, statistics);

    /* 将主机名和服务名映射到一个地址，并返回指向addrinfo的指针 */
    ai = host_serv(host, NULL, 0, 0);
    /* 将网络字节序的地址转换为字符串格式地址，并返回该字符串的指针 */
    h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);

    /* 显示PING的主机名、地址与数据字节数 */
    printf("PING %s (%s) %d bytes of data.\n", ai->ai_canonname ? ai->ai_canonname : h, h, datalen);

    /* initialize according to protocol */
    if(ai->ai_family == AF_INET)
    {
        pr = &proto_v4;/* proto结构指针pr指向对应域的结构，这里是IPv4域的结构 */
#ifdef IPV6
    }else if(ai->family == AF_INET6)
    {
        pr = &proc_v6;
        if(IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr)))
        {
            perror("connot ping IPv4-mapped IPv6 address");
            exit(1);
        }
#endif
    }else
    {
        printf("unknown address family %d", ai->ai_family);
        exit(1);
    }

    pr->sasend = ai->ai_addr;/* 发送地址赋值 */
    pr->sarecv = (struct sockaddr *)Calloc(1, ai->ai_addrlen);
    pr->salen = ai->ai_addrlen;/* 地址的大小 */

    /* 处理数据 */
    readloop();

    exit(0);

}
/* 显示发送和接收数据报的个数，并计算丢包率 */
void statistics(int signo)
{
    printf("\n----------- %s ping statistics -----------\n", Sock_ntop_host(pr->sarecv, pr->salen));
    int lost = 100*(nsent-nrecv)/nsent;
    printf("%d packets transmitted, %d received, %d packet lost\n", nsent, nrecv, lost);
    close(sockfd);
    exit(1);
}
