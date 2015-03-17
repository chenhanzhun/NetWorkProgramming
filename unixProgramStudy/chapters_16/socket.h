/* 套接字 */

/*
 * 函数功能：创建套接字描述符；
 * 返回值：若成功则返回套接字描述符，若出错返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
/*
 * 说明：
 * socket类似与open对普通文件操作一样，都是返回描述符，后续的操作都是基于该描述符；
 * domain表示套接字的通信域，不同的取值决定了socket的地址类型，其一般取值如下：
 * （1）AF_INET         IPv4因特网域
 * （2）AF_INET6        IPv6因特网域
 * （3）AF_UNIX         Unix域
 * （4）AF_UNSPEC       未指定
 *
 * type确定socket的类型，常用类型如下：
 * （1）SOCK_STREAM     有序、可靠、双向的面向连接字节流
 * （2）SOCK_DGRAM      长度固定的、无连接的不可靠报文传递
 * （3）SOCK_RAW        IP协议的数据报接口
 * （4）SOCK_SEQPACKET  长度固定、有序、可靠的面向连接的报文传递
 *
 * protocol指定协议，常用取值如下：
 * （1）0               选择type类型对应的默认协议
 * （2）IPPROTO_TCP     TCP传输协议
 * （3）IPPROTO_UDP     UDP传输协议
 * （4）IPPROTO_SCTP    SCTP传输协议
 * （5）IPPROTO_TIPC    TIPC传输协议
 *
 */
/*
 * 函数功能：关闭套接字上的输入或输出；
 * 返回值：若成功则返回0，若出错返回-1；
 * 函数原型：
 */
#include <sys/socket.h>
int shutdown(int sockfd, int how);
/*
 * 说明：
 * sockfd表示待操作的套接字描述符；
 * how表示具体操作，取值如下：
 * （1）SHUT_RD     关闭读端，即不能接收数据
 * （2）SHUT_WR     关闭写端，即不能发送数据
 * （3）SHUT_RDWR   关闭读、写端，即不能发送和接收数据
 *
 */
/*
 * 函数功能：主机字节序和网络字节序之间的转换；
 * 返回值：返回对应类型表示的字节序；
 * 函数原型：
 */
#include <arpa/inet.h>
uint32_t htonl(uint32_t hostint32);//返回值：以网络字节序表示的32位整型数；
uint16_t htons(uint16_t hostint16);//返回值：以网络字节序表示的16位整型数；

uint32_t ntohl(uint32_t netint32);//返回值：以主机字节序表示的32位整型数；
uint16_t ntohs(uint16_t netint16);//返回值：以主机字节序表示的16位整型数；
/*
 * 说明：
 * 从以上函数我们可以发现：
 * h表示“主机（host）”字节序，n表示“网络（network）”字节序；
 * l表示“长（long）”整型，s表示“短（short）”整型；
 *
 */
/*
 * 函数功能：字节操作；
 * 函数原型：
 */
#include <strings.h>
void bzero(void *dest, size_t nbytes);//将dest所存储的数据前nbytes字节初始化为0；
void bcopy(const void *str, void *dest, size_t nbytes);//将str所存储的数据前nbytes字节复制到dest中；
int bcmp(const void *ptr1, const void *ptr2, size_t nbytes);//比较两个字符的前nbytes字节的大小；

void *memset(void *dest, int c, size_t len);//将dest所存储的数据前len字节初始化为c；
void *memcopy(void *dest,const void *src, size_t nbytes);//将src所存储的数据前nbytes字节复制到dest中；
int memcmp(const void *ptr1, const void *ptr2, size_t nbytes);//比较两个字符的前nbytes字节的大小；

/* 地址数据结构 */

/* 通用的地址数据结构 */
struct sockaddr
{
    uint8_t         sa_len;     /* total length */
    sa_family_t     sa_family;  /* address family */
    char            sa_data[];  /* variable-length address */
};
/* Linux 下的地址数据结构 */
struct sockaddr
{
    uint8_t         sa_len;     /* total length */
    sa_family_t     sa_family;  /* address family */
    char            sa_data[14];  /* length address */
};

#include<arpa/inet.h>
const char *inet_ntop(int domain, const void *restrict addr, char *restrict str, socklen_t size);
//若成功则返回地址字符串指针，出错则返回NULL；

int inet_pton(int domain, const char *restrict str, void *restrict addr);
//若成功则返回1，格式无效则返回0，出错则返回-1；
/*
 * 说明：
 * inet_ntop 是将网络字节序的二进制地址转换成文本字符串格式；
 * inet_pton 是将文本字符串格式转换成网络字节序的二进制地址；
 * 参数domain只能取值：AF_INET 或 AF_INET6；
 */

/* 地址查询 */

/*
 * 函数功能：获取给定计算机的主机信息；
 * 返回值：若成功则返回主机结构指针，若出错则返回NULL；
 * 函数原型：
 */
#include <netdb.h>

struct hostent *gethostbyname(const char *hostname);//将主机名地址转换为数字地址；
struct hostent *gethostaddr(const char *addr, size_t len, int family);//将主机数字地址转换为名字地址；
struct hostent *gethostent(void);

void sethostent(int stayopen);
void endhostent(void);
/*
 * 说明：
 * 若主机数据文件没有打开，gethostent会打开它，该函数返回文件的下一条目；
 * 函数sethostent会打开文件，若文件已打开，那么将其回绕；
 * 函数endhostent将关闭文件；
 * 其中hostent结构至少包含如下成员数据：
 */
struct hostent
{
    char    *h_name;        /* name of host */
    char    **h_aliases;    /* pointer to alternate host name array */
    int     h_addrtype;     /* address type */
    int     h_length;       /* length in bytes of address */
    char    **h_addr_list;  /* pointer to array of network address */
};

/*
 * 函数功能：获取网络名和网络号；
 * 返回值：若成功则返回指针，出错返回NULL；
 * 函数原型：
 */
#include <netdb.h>
struct netent *getnetbyaddr(uint32_t net, int type);
struct netent *getnetbyname(const char *name);
struct netent *getnetent(void);

void setnetent(int stayopen);
void endnetent(void);
/*
 * netent 结构至少包含以下成员：
 */
struct netent
{
    char    *n_name;        /* network name */
    char    *n_aliases;     /* alternate network name array pointer */
    int     n_addrtype;     /* address type */
    uint32_t n_net;         /* network number */
};

#include <netdb.h>
struct protoent *getprotobyname(const char *name);
struct protoent *getprotobynumber(int proto);
struct protoent *getprotoent(void);
//以上三个函数返回值：若成功则返回指针，出错返回NULL；

void setprotornt(int stayopen);
void endprotornt(void);
/*
 * protoent 结构至少包含以下成员：
 */
struct protoent
{
    char    *p_name;    /* protocol name */
    char    **p_aliases;/* pointer to alternate protocol name array */
    int     p_proto;    /* protocol number */
};

/*
 * 函数功能：服务名与端口号之间的映射；
 * 返回值：若成功则返回指针，若出错则返回NULL；
 * 函数原型：
 */
#include <netdb.h>
struct servent *getservbyname(const char *name, const char *proto);
struct servent *getservbyport(int port, const char *proto);
struct servent *getservent(void);

void setservent(int stayopen);
void endservent(void);
/*
 * 其中servent 结构至少包含以下成员：
 */
struct servent
{
    char    *s_name;        /* service name */
    char    **s_aliases;    /* pointer to alternate service name array */
    int     s_port;         /* port number */
    char    *s_proto;       /* name of protocol */
};
/*
 * 函数功能：将服务名和主机名映射到一个地址；
 * 返回值：若成功则返回0，若出错则返回非0错误编码；
 * 函数原型：
 */
#include <netdb.h>
#include <sys/socket.h>
int getaddrinfo(const char *host, const char *service, const struct addrinfo *hint, struct addrinfo **res);

void freeaddrinfo(struct addrinfo *ai);

const char *gai_strerror(int error);//若getaddrinfo出错时，错误消息只能由该函数输出；

/*
 * 说明：
 * 该函数需要提供主机名或服务名，若只提供其中一个，则另一个必须指定为NULL；
 * addrinfo是一个结构链表，其定义如下：
 */
struct addrinfo
{
    int         ai_flags;       /* customize behavior */
    int         ai_family;      /* address family */
    int         ai_socktype;    /* socket type */
    int         ai_protocol;    /* protocol */
    socklen_t   ai_addrlen;     /* length in bytes of address */
    struct sockaddr *ai_addr;   /* address */
    char        *ai_canonname;  /* canonical name of host */
    struct addrinfo *ai_next;   /* next in list */
};
/*
 * 函数功能：将地址转换成服务名或主机名；
 * 返回值：若成功则返回0，若出错则返回非0值；
 * 函数原型：
 */
#include <netdb.h>
#include <sys/socket.h>
int getnameinfo(const struct sockadd *addr, socklen_t alen, char * host, socklen_t hostlen,
        char * service, socklen_t servlen, unsigned int flags);
