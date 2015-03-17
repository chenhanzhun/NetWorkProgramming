/* 主机名与地址之间转换 */

/*
 * 函数功能：主机名与地址之间转换；
 * 返回值：若成功则返回主机结构指针，若出错则返回NULL；
 * 函数原型：
 */
#include <netdb.h>

struct hostent *gethostbyname(const char *hostname);//将主机名转换为数字地址；
struct hostent *gethostaddr(const char *addr, size_t len, int family);//将数字地址转换为主机名；

/* 函数功能：获取主机信息；
 * 函数原型：
 */
struct hostent *gethostent(void);/* 获取主机信息，并返回hostent结构指针 */
void sethostent(int stayopen);/* 设置主机信息 */
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
    char    *h_name;        /* official name of host */
    char    **h_aliases;    /* pointer to alternate host name array */
    int     h_addrtype;     /* address type: AF_INET */
    int     h_length;       /* length in bytes of address: 4 */
    char    **h_addr_list;  /* pointer to array of IPv4 address */
};

/* 服务名与端口号之间的转换 */
/*
 * 函数功能：服务名与端口号之间的转换；
 * 返回值：若成功则返回指针，若出错则返回NULL；
 * 函数原型：
 */
#include <netdb.h>
struct servent *getservbyname(const char *servname, const char *protoname);
struct servent *getservbyport(int port, const char *protoname);
struct servent *getservent(void);

void setservent(int stayopen);
void endservent(void);
/*
 * protoname参数若为空，则返回取决与实现，若为非空，则指定协议名称；
 *
 * 其中servent 结构至少包含以下成员：
 */
struct servent
{
    char    *s_name;        /* official service name */
    char    **s_aliases;    /* pointer to alternate service name array */
    int     s_port;         /* port number */
    char    *s_proto;       /* name of protocol */
};

/* IPv6、IPv4 都可使用 */
/*
 * 函数功能：将 服务名与端口号 和 主机名与地址 之间转换；
 * 返回值：若成功则返回0，若出错则返回非0错误编码；
 * 函数原型：
 */
#include <netdb.h>
#include <sys/socket.h>
int getaddrinfo(const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **result);

void freeaddrinfo(struct addrinfo *ai);/* 把从getaddrinfo函数动态分配的成员结构内存返回给系统，参数ai是由函数getaddrinfo返回的第一个addrinfo结构 */

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

/*
 * 说明：
 * addrinfo结构成员：
 * ai_flags 取值如下：
 * （1）AI_PASSIVE      套接字将用于被动打开；
 * （2）AI_CANONNAME    告知getaddrinfo函数返回主机的规范名字；
 * （3）AI_NUMERICHOST  防止任何类型的名字到地址映射，hostname必须是一个地址串；
 * （4）AI_NUMERICSERV  防止任何类型的名字到服务映射，service必须是一个十进制端口号数串；
 * （5）AI_V4MAPPED     若同时指定ai_family值为AF_INET6，若没有可用的AAAA记录，则返回与A记录对应的IPv4映射的IPv6地址；
 * （6）AI_ALL          若同时指定AI_V4MAPPED标志，除了返回与AAAA记录对应的IPv6地址外，还返回与A记录对应的IPv4映射的IPv6地址；
 * （7）AI_ADDRCONFIG   按照所在主机的配置选择返回地址类型；
 */
