/* 数据传输 */
/*
 * 函数功能：发送数据；
 * 返回值：若成功则返回发送的字节数，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

ssize_t send(int sockfd, void *buf, size_t nbytes, int flags);
/*
 * 说明
 * 该函数的功能类似与write函数，除了有标识符flags之外，其他的相同；
 * flags标识符的取值如下：
 * （1）MSG_DONTROUTE   勿将数据路由除本地网络
 * （2）MSG_DONTWAIT    允许非阻塞操作
 * （3）MSG_EOR         如果协议支持，此为记录结束
 * （4）MSG_OOB         如果协议支持，发送带外数据
 *
 * 若send成功返回，并不必然表示连接另一端的进程接收数据，只能说数据已经无错误地发送到网络；
 *
 * 对于支持为报文设限的协议，若报文超过协议所支持的最大尺寸，send失败并将errno设为EMSGSIZE；
 * 对于字节流协议，send会阻塞直到整个数据被传输；
 */

ssize_t sendto(int sockfd, void *buf, size_t nbytes, int flags,
                const struct sockaddr *destaddr, socklen_t destlen);
/*
 * 说明
 * 该函数通过指定目标地址允许在无连接的套接字之间发送数据，无连接的套接字之间通信不能使用send函数，除非connect时预先设定目标地址；
 *
 */

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
/*
 * 说明
 * 该函数可以使用不止一个的选择来通过套接字发送数据，可以指定多重缓冲区传输数据，类似与readv函数；
 * msghdr结构至少包含以下成员：
 */
struct msghdr
{
    void        *msg_name;      /* optional address */
    socklen_t   msg_namelen;    /* address size in bytes */
    struct iovec *msg_iov;      /* array of IO buffers */
    int         msg_iovlen;     /* number of elements in array */
    void        *msg_control;   /* ancillary data */
    socklen_t   msg_controllen; /* number of ancillary bytes */
    int         msg_flags;      /* flags for recevied message */
};
/* 数据传输 */
/*
 * 函数功能：接收数据；
 * 返回值：以字节计数的消息长度，若无可用消息或对方已经按序结束则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
/*
 * 说明
 * 该函数的功能类似与read函数，除了有标识符flags之外，其他的相同；
 * flags标识符的取值如下：
 * （1）MSG_PEEK        返回报文内容而不真正取走报文
 * （2）MSG_TRUNC       即使报文被截断，要求返回的是报文的实际长度
 * （3）MSG_WAITALL     等待直到所有数据可用
 * （4）MSG_OOB         如果协议支持，发送带外数据
 *
 */

ssize_t recvfrom(int sockfd, void *buf, size_t nbytes, int flags,
                struct sockaddr *addr, socklen_t addrlen);
/*
 * 说明
 *
 * 若addr非空，它将包含数据发送者的套接字端点地址，该函数常用于无连接套接字；
 */

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
