/* IO多路复用 */

/*
 * 函数功能：
 * 返回值：准备就绪的描述符数，若超时则返回0，出错则返回-1；
 * 函数原型：
 */
#include <sys/select.h>
int select(int maxfdpl, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *tvptr);
/*
 * 说明：
 * 参数maxfdpl是“最大描述符加1”，即指定待测试的描述符个数；
 * 参数readfds、writefds、exceptfds是指向描述符集的指针，即让内核测试读、写或异常条件的描述符；
 * 时间参数有三种取值：
 * tvptr == NULL;
 *      永远等待；若捕获到信号则中断此无限期等待；当所指定的描述符中的一个已准备好或捕获到信号则返回；
 *      若捕获到信号，则select返回-1，errno设置为EINTR；
 *
 * tvptr->tv_sec == 0 && tvptr->tv_usec == 0;
 *      完全不等待；测试所有描述符并立即返回，这是得到多个描述符的状态而不阻塞select函数的轮回方法；
 *
 * tvptr->sec != 0 || tvptr->usec != 0;
 *      等待指定的秒数和微妙数；当指定的描述符已准备好，或超过指定的时间立即返回；
 *      若超过指定的时间还没有描述符准备好，则返回0；
 *
 * tvptr的结构如下：
 */
struct timeval
{
    long tv_sec;    /* seconds */
    long tv_usec;   /* and microseconds */
};


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
