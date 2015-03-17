/* 函数功能：操作描述符，设置已打开的描述符属性；
 * 函数原型：
 */
#include <sys/ioctl.h>
/* Perform the I/O control operation specified by REQUEST on FD.
   One argument may follow; its presence and type depend on REQUEST.
   Return value depends on REQUEST.  Usually -1 indicates error.  */
int ioctl (int fd, unsigned long int request, .../* void *arg */);
/*
 * 说明：
 * 把和网络相关的请求request划分为以下 6 类：
 * （1）套接字操作
 * （2）文件操作
 * （3）接口操作
 * （4）ARP高速缓存操作
 * （5）路由表操作
 * （6）流系统
 */
/*
*fcntl函数
*功能：操纵文件描述符，设置已打开的文件的属性*/
int fcntl(int fd, int cmd, ... /* arg */ );
/*说明：
 * fcntl 函数提供了与网络编程相关的特性如下：
 * （1）非阻塞式IO；标志：O_NONBLOCK
 * （2）信号驱动式IO；标志：O_ASYNC
 * （3）套接字属性；标志：F_GETOWN、F_SETOWN
 *
 * cmd的取值可以如下：
 * 复制文件描述符
 *   F_DUPFD (long)
 * 设置/获取文件描述符标志
 *   F_GETFD (void)
 *   F_SETFD (long)
 * 设置/获取文件状态标志
 *   F_GETFL (void)
 *   F_SETFL (long)
 * 获取/设置文件锁
 *   F_GETLK
 *   F_SETLK，F_SETLKW
 */


/* 结构定义如下：*/
struct ifconf {
    int ifc_len; /* size of buffer, value-result */
    union {
        caddr_t ifcu_buf; /* input from user->kernel */
        struct ifreq *ifcu_req; /* return from kernel->user */
    }ifc_ifcu;
};
#define ifc_buf     ifc_ifcu.ifcu_buf
#define ifc_req     ifc_ifcu.ifcu_req
#define IFNAMSIZ    16
struct ifreq {
    char ifr_name[IFNAMSIZ];
    union {
        struct sockaddr ifru_addr;
        struct sockaddr ifru_dstaddr;
        struct sockaddr ifru_broadaddr;
        short ifru_flags;
        int ifru_metric;
        caddr_t ifru_data;
    }ifr_ifru;
};
#define ifr_addr        ifr_ifru.ifru_addr
#define ifr_dstaddr     ifr_ifru.ifru_dstaddr
#define ifr_broadaddr   ifr_ifru.broadaddr
#define ifr_flags       ifr_ifru.ifru_flags
#define ifr_metric      ifr_ifru.ifru_metric
#define ifr_data        ifr_ifru.ifru_data
