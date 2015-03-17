/* IO多路转接*/

/*
 * 函数功能：获取准备好的描述符数；
 * 返回值：准备就绪的描述符数，若超时则返回0，出错则返回-1；
 * 函数原型：
 */
#include <sys/select.h>
int select(int maxfdpl, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *tvptr);
/*
 * 说明：
 * 参数maxfdpl是“最大描述符加1”；
 * 参数readfds、writefds、exceptfds是指向描述符集的指针，即描述符可读、可写或处于异常条件的；
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

#include <sys/select.h>
int  FD_ISSET(int fd, fd_set *fdset); //测试描述符fd是否在描述符集中设置；若fd在描述符集中则返回非0值，否则返回0
void FD_CLR(int fd, fd_set *fdset); //清除在fdset中指定的位fd；
void FD_SET(int fd, fd_set *fdset); //设置fd在fdset中指定的位；
void FD_ZERO(fd_set *fdset); //清除整个fdset；即所有描述符位都为0；

/*
 * 函数功能：获取准备好的描述符数；
 * 返回值：准备就绪的描述符数，若超时则返回0，出错则返回-1；
 * 函数原型：
 */
#include <sys/select.h>
int pselect(int maxfdpl, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *tsptr,
        const sigset_t *sigmask);

/*
 * 函数功能：和select函数类似；
 * 函数原型：
 */
#include <poll.h>
int poll(struct pollfd fdarray[], nfds_t nfds, int timeout);
/*
 * 说明：
 * timeout == -1;   永远等待。
 * timeout == 0;    不等待，测试所有的描述符并立即返回。
 * timeout > 0;     等待timeout毫秒，当指定的描述符之一已经准备好，或指定的时间值已经超过时立即返回。
 */
