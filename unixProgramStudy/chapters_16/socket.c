#include "apue.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void print_family(struct addrinfo *aip);
void print_type(struct addrinfo *aip);
void print_flags(struct addrinfo *aip);
void print_protocol(struct addrinfo *aip);

int main(int argc, char **argv)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;

    /* 定义IPv4的套接字地址结构 */
    struct sockaddr_in *sinp;

    const char *addr;
    int err;
    char abuf[INET_ADDRSTRLEN];

    if(argc != 3)
        err_quit("usage: %s nodename service", argv[0]);
    /* 初始化addrinfo结构变量hint*/
    hint.ai_family = 0;
    hint.ai_socktype = 0;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_flags = AI_CANONNAME;//需要一个规范名，而不是别名；
    hint.ai_addr = NULL;
    hint.ai_next = NULL;//表示只有一个addrinfo链表结构；
    hint.ai_canonname = NULL;

    /* 将主机名和服务名映射到一个地址 */
    if((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0)
        err_quit("getaddrinfo error: %s\n", gai_strerror(err));

    /* 打印主机和服务信息 */
    for(aip = ailist; aip != NULL; aip = aip->ai_next)
    {
        print_family(aip);
        print_type(aip);
        print_protocol(aip);
        print_flags(aip);

        printf("\n\thost %s", aip->ai_canonname ?aip->ai_canonname:"-");
        if(aip->ai_family == AF_INET)
        {
            /* 获取IP地址，并把网络字节序的二进制地址转换为文本字符串地址 */
            sinp = (struct sockaddr_in *)aip->ai_addr;
            addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf, INET_ADDRSTRLEN);
            printf(" address %s", addr ? addr:"unknown");
            printf(" port %d", ntohs(sinp->sin_port));
        }
        printf("\n");
    }
    exit(0);
}
void print_family(struct addrinfo *aip)
{
    printf(" family-- ");
    switch(aip->ai_family)
    {
        case AF_INET://IPv4
            printf("inet");
            break;
        case AF_INET6://IPv6
            printf("inet6");
            break;
        case AF_UNIX://UNIX域
            printf("Unix");
            break;
        case AF_UNSPEC://未指定
            printf("unspecified");
            break;
        default:
            printf("unknown");
    }
}
void print_type(struct addrinfo *aip)
{
    printf(" type.. ");
    switch(aip->ai_socktype)
    {
        case SOCK_STREAM:
            printf("stream");
            break;
        case SOCK_DGRAM:
            printf("datagram");
            break;
        case SOCK_RAW:
            printf("raw");
            break;
        case SOCK_SEQPACKET:
            printf("seqpacket");
            break;
        default:
            printf("unknown (%d)", aip->ai_socktype);
    }
}

void print_protocol(struct addrinfo *aip)
{
    printf(" protocol++ ");
    switch(aip->ai_protocol)
    {
        case IPPROTO_TCP:
            printf("TCP");
            break;
        case IPPROTO_UDP:
            printf("UDP");
            break;
        case IPPROTO_SCTP:
            printf("SCTP");
            break;
        case 0:
            printf("default");
            break;
        default:
            printf("unknown (%d)", aip->ai_protocol);
    }
}
void print_flags(struct addrinfo *aip)
 {
     printf(" flags ");
     if(aip->ai_flags == 0)
         printf("0");
     else
     {
         if(aip->ai_flags & AI_PASSIVE)
             printf(" passive ");
         if(aip->ai_flags & AI_CANONNAME)
             printf(" canon ");
         if(aip->ai_flags & AI_NUMERICHOST)
             printf(" numhost ");
     }
 }
