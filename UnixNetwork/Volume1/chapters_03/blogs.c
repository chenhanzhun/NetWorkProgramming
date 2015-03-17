/* 套接字编程简介 */

/* 通用套接字地址结构 */
/* 定义于 <sys/socket.h> 头文件中 */

struct sockaddr{

    uint8_t     sa_len;
    sa_family_t sa_family;  /* address family: AF_xxx value */
    char        sa_data[14];/* protocol-specific address */
};
/*
 * 说明：
 * 当套接字地址作为一个参数传递进任何套接字函数时，套接字地址结构总是以引用形式（也就是以指向该结构的指针）来传递；
 * 因此，必须把特定协议域的套接字地址结构强制转换为通用套接字地址结构类型；
 */

/* IPv4 套接字地址结构 */
/* 定义于 <netinet/in.h> 头文件中 */

struct in_addr{

    in_addr_t   s_addr; /* 32-bit IPv4 address */
                        /* network byte ordered */
};

struct sockaddr_in{

    uint8_t         sin_len;    /* length of struct (16) */
    sa_family_t     sin_family; /* AF_INET */
    in_port_t       sin_port;   /* 16-bit TCP or UDP port number */
                                /* network byte ordered */
    struct in_addr  sin_addr;   /* 32-bit IPv4 address */
                                /* network byte ordered */
    char            sin_zero[8];/* unused */
};
/* 说明：
 * sin_len 是简化了长度可变套接字地址结构的处理，POSIX 规范并不要求有此成员；
 * sin_bzero 使所以套接字地址结构大小至少为 16 字节；
 * s_addr、sin_family、sin_port 是 POSIX 数据类型。in_addr_t 数据类型必须是一个至少 32 位的无符号整型，in_port_t 必须是一个至少 16 位的无符号整型，
 * 而 sa_family_t 通常是一个 8 位无符号整数，但在不支持长度字段的实现中，它则是 16 位无符号整数；
 * IPv4 地址和 TCP 或 UDP 端口号在套接字地址结构中是以 网络字节序来 存储的；
 */

/* IPv6 套接字地址结构 */
/* 定义于 <netinet/in.h> 头文件中 */

struct in6_addr{

    uint8_t     s6_addr[16];    /* 128-bit IPv6 address */
                                /* network byte ordered */
};

#define SIN6_LEN                    /* required for compile-time tests */
struct sockaddr_in6{

    uint8_t         sin6_len;       /* length of this struct (28) */
    sa_family_t     sin6_family;    /* AF_INET6 */
    in_port_t       sin6_port;      /* transport layer port# */
                                    /* network byte ordered */
    uint32_t        sin6_flowinfo;  /* flow information, undefined */
    struct in6_addr sin6_addr;      /* IPv6 address */
                                    /* network byte ordered */
    uint32_t        sin6_scope_id;  /* set of interfaces for a scope */
};
/* 说明：
 * 若系统支持套接字地址结构中的长度字段，则 SIN6_LEN 常值必须定义；
 * sin6_flowinfo 字段分成两个字段：低序 20 位是流标，高序 12 位保留；
 * 对于具备范围的地址，sin6_scope_id 字段标识其范围；
 */

/* 为了使用 IPv6 的使用，定义了新的通用套接字地址结构 */
/* 定义于 <netinet/in.h> 头文件中 */

struct sockaddr_storage{

    uint8_t         ss_len;     /* length of this struct (implementation dependent) */
    sa_family_t     ss_family;  /* address family: AF_xxx value */
    /* implementation-dependent elements to provide:
     * 1) alignment sufficient to fulfill the alignment requirments of all socket address types that the system supports. 
     * 2) enough storage to hold any type of socket address that the system suports.
     */
};
/* 说明：
 * 若系统支持任何套接字地址结构有对齐需要，则该新通用地址结构能够满足最苛刻的对齐要求；
 * 新地址结构能够容纳系统支持的任何套接字地址结构；
 */



/* 套接字 */

/*
 * 函数功能：创建套接字描述符；
 * 返回值：若成功则返回套接字非负描述符，若出错返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int socket(int family, int type, int protocol);
/*
 * 说明：
 * socket类似与open对普通文件操作一样，都是返回描述符，后续的操作都是基于该描述符；
 * family 表示套接字的通信域，不同的取值决定了socket的地址类型，其一般取值如下：
 * （1）AF_INET         IPv4因特网域
 * （2）AF_INET6        IPv6因特网域
 * （3）AF_UNIX         Unix域
 * （4）AF_ROUTE        路由套接字
 * （5）AF_KEY          密钥套接字
 * （6）AF_UNSPEC       未指定
 *
 * type确定socket的类型，常用类型如下：
 * （1）SOCK_STREAM     有序、可靠、双向的面向连接字节流套接字
 * （2）SOCK_DGRAM      长度固定的、无连接的不可靠数据报套接字
 * （3）SOCK_RAW        原始套接字
 * （4）SOCK_SEQPACKET  长度固定、有序、可靠的面向连接的有序分组套接字
 *
 * protocol指定协议，常用取值如下：
 * （1）0               选择type类型对应的默认协议
 * （2）IPPROTO_TCP     TCP传输协议
 * （3）IPPROTO_UDP     UDP传输协议
 * （4）IPPROTO_SCTP    SCTP传输协议
 * （5）IPPROTO_TIPC    TIPC传输协议
 *
 */

/* 套接字的基本操作 */

/*
 * 函数功能：将协议地址绑定到一个套接字；其中协议地址包含IP地址和端口号；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
/*
 * 说明：
 * sockfd 为套接字描述符；
 * addr是一个指向特定协议地址结构的指针；
 * addrlen是地址结构的长度；
 */
/*
 * 函数功能：获取已绑定到一个套接字的地址；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *alenp);
/*
 * 说明：
 * 调用该函数之前，设置alenp为一个指向整数的指针，该整数指定缓冲区sockaddr的大小；
 * 返回时，该整数会被设置成返回地址的大小，如果该地址和提供的缓冲区长度不匹配，则将其截断而不报错；
 */
/*
 * 函数功能：获取套接字对方连接的地址；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *alenp);
/*
 * 说明：
 * 该函数除了返回对方的地址之外，其他功能和getsockname一样；
 */
/*
 * 函数功能：建立连接，即客户端使用该函数来建立与服务器的连接；
 * 返回值：若成功则返回0，出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
/*
 * 说明：
 * sockfd是系统调用的套接字描述符，即由socket函数返回的套接字描述符；
 * servaddr是目的套接字的地址，该套接字地址结构必须包含目的IP地址和目的端口号，即想与之通信的服务器地址；
 * addrlen是目的套接字地址的大小；
 *
 * 如果sockfd没有绑定到一个地址，connect会给调用者绑定一个默认地址，即内核会确定源IP地址，并选择一个临时端口号作为源端口号；
 */
/*
 * 函数功能：接收连接请求；
 * 函数原型：
 */
#include <sys/socket.h>

int listen(int sockfd, int backlog);//若成功则返回0，若出错则返回-1；
/*
 * sockfd是套接字描述符；
 * backlog是该进程所要入队请求的最大请求数量；
 */

/* 函数功能：从已完成连接队列队头返回下一个已完成连接；若已完成连接队列为空，则进程进入睡眠；
 * 函数原型：
 */
int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);//返回值：若成功返回套接字描述符，出错返回-1；
/*
 * 说明：
 * 参数 cliaddr 和 addrlen 用来返回已连接的对端（客户端）的协议地址；
 *
 * 该函数返回套接字描述符，该描述符连接到调用connect函数的客户端；
 * 这个新的套接字描述符和原始的套接字描述符sockfd具有相同的套接字类型和地址族，而传给accept函数的套接字描述符sockfd没有关联到这个链接，
 * 而是继续保持可用状态并接受其他连接请求；
 * 若不关心客户端协议地址，可将cliaddr和addrlen参数设置为NULL，否则，在调用accept之前，应将参数cliaddr设为足够大的缓冲区来存放地址，
 * 并且将addrlen设为指向代表这个缓冲区大小的整数指针；
 * accept函数返回时，会在缓冲区填充客户端的地址并更新addrlen所指向的整数为该地址的实际大小；
 *
 * 若没有连接请求等待处理，accept会阻塞直到一个请求到来；
 */

/* 函数功能：创建子进程；
 * 返回值：
 * （1）在子进程中，返回0；
 * （2）在父进程中，返回新创建子进程的进程ID；
 * （3）若出错，则范回-1；
 * 函数原型：
 */
#include <unistd.h>
pid_t fork(void);
/* 说明：
 * 该函数调用一次若成功则返回两个值：
 * 在调用进程（即父进程）中，返回新创建进程（即子进程）的进程ID；
 * 在子进程返回值是0；
 * 因此，可以根据返回值判断进程是子进程还是父进程；
 */

/* exec 序列函数 */

/*
 * 函数功能：把当前进程替换为一个新的进程，新进程与原进程ID相同；
 * 返回值：若出错则返回-1，若成功则不返回；
 * 函数原型：
 */
#include <unistd.h>
int execl(const char *pathname, const char *arg, ...);
int execv(const char *pathnam, char *const argv[]);
int execle(const char *pathname, const char *arg, ... , char *const envp[]);
int execve(const char *pathnam, char *const argv[], char *const envp[]);
int execlp(const char *filename, const char *arg, ...);
int execvp(const char *filename, char *const argv[]);
/*  6 个函数的区别如下：
 * （1）待执行的程序文件是 文件名 还是由 路径名 指定；
 * （2）新程序的参数是 一一列出 还是由一个 指针数组 来引用；
 * （3）把调用进程的环境传递给新程序 还是 给新程序指定新的环境；
 */

/* 函数功能：关闭套接字，若是在 TCP 协议中，并终止 TCP 连接；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <unistd.h>
int close(int sockfd);

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
